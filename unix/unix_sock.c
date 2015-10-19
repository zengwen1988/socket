/*
 * NAME
 *   unix_sock.c - unix socket operations
 *
 * DESC
 *   - funcs all in c
 *   - support: linux / osx / ios
 *
 * CC
 *   ...gcc / g++
 *
 * CREATED
 *   2015-10-19 1 292 02:04:45 -0400 by yuiwong
 *
 * V
 *   - 1.0.0.0 - TODO
 *
 * LICENSE
 *   LGPLv3
 *	 This file is part of socket.
 *	 socket is free software:
 *	 you can redistribute it and/or modify it under the terms of the GNU
 *	 General Public License as published by the Free Software Foundation,
 *	 either version 3 of the License, or (at your option) any later version.
 *   socket is distributed in the hope that it will be
 *	 useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	 GNU General Public License for more details.
 *	 You should have received a copy of the GNU General Public License
 *	 along with socket.
 *	 If not, see <http://www.gnu.org/licenses/>.
 */


#include <unix/sock.h>

#include <stdlib.h>
#include <pthread.h>

#include <unistd.h> /* usleep */
#include <fcntl.h> /* F_GETFL .. */
#include <sys/socket.h> /* SOL_SOCKET .. */
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include <c_log.h> /* log2stream */


/*
 * NAME get_sockfd_by_protocol - start to connect to ip:port
 *
 * DESC
 *   - get a connecting sockfd
 *
 * PARAMS
 *   - ip: host order IPv4
 *   - port: host order
 *
 * RETURN
 *   - success: sockfd >= 0
 *   - fail: -errno
 */
int get_sockfd_by_protocol (uint32_t ip, uint16_t port)
{

	int ret;
	struct sockaddr_in sa;
	int sockfd;

	/* gethostbyname */


	sa.sin_addr.s_addr = htonl(ip);


#	if defined(UNIX_SOCK_DEBUG)
	log2stream(stdout, "ip: 0x%x addr: 0x%x", ip, sa.sin_addr.s_addr);
#	endif

	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);

	/* SOCK_STREAM -- here is TCP FIXME use all params */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		ret = errno;

		log2stream(stderr, "socket fail");

		goto sock_fail;
	}

#	if defined(UNIX_SOCK_DEBUG)
	log2stream(stdout, "before connect");
#	endif

	set_sock_block(sockfd, 0);

	/* connect */
	ret = connect(sockfd, (const struct sockaddr *)&sa, sizeof(sa));

	if ((ret < 0) && (EINPROGRESS != errno)) {
		ret = errno;

		log2stream(stderr, "connect fail");

		goto conn_fail;
	}

	log2stream(stdout, "start to connect: %08x:%u success: %d", ip, port,
		sockfd);

	/* success */
	return sockfd;

conn_fail:
	set_sock_block(sockfd, 1);
	close(sockfd);
	sockfd = -1;

sock_fail:

	return (int32_t)-ret;

}


ssize_t recv_from_sockfd (int32_t sockfd, uint8_t * buf, int32_t start,
	size_t max, uint32_t wr_us, uint32_t r_us)
{

	struct timeval timeout, timeout_r;
	fd_set readfds;
	int nfds, ret;

#if defined(UNIX_SOCK_DEBUG)
	fprintf(stdout,
		"+%d: fd: %d buf: %p start: %d max: %zu\n", __LINE__, sockfd,
		buf, start, max);
	printf("wr_us: %u r_us: %u\n", wr_us, r_us);
#	endif

	/* set select timeout */
	timeout.tv_sec = wr_us / 1e6;
	timeout.tv_usec = wr_us % (int)1e6;

	/* set recv fd */
	FD_ZERO(&readfds);
	FD_SET((int)sockfd, &readfds);
	nfds = (int)sockfd + 1;

	/* selecting */
	ret = select(nfds, &readfds, NULL, NULL, &timeout);
	if (-1 == ret) {
		/* error */
		error2stream(stderr, "select fail");
		ret = -1;
		goto end;
	} else if (0 == ret) {
		/* no ready and timeout */
		/* log2stream(stdout, "timeout"); */
		ret = -1002;
		goto end;
	}

	/* set recv */
	timeout_r.tv_sec = r_us / 1e6;
	timeout_r.tv_usec = r_us % (int)1e6;

	/* FIXME: check ret */
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout_r,
		(socklen_t)sizeof(struct timeval));

	/* XXX-FIXED: MSG_WAITALL -> MSG_DONTWAIT iOS ! */
	ret = (int)recv(sockfd, buf + start, max, MSG_DONTWAIT);

	if ((ret < 0) && (EAGAIN == errno)) {
		/* recv timeout */
		ret = -1003;
		goto end;
	} else if ((ret < 0) && (EWOULDBLOCK == errno)) {
		/* recv: EWOULDBLOCK */
		error2stream(stderr, "recv fail");
		ret = -2;
		goto end;
	} else if (ret <0) {
		ret = -3;
		error2stream(stderr, "recv fail");
		goto end;
	} else if (0 == ret) {
		error2stream(stdout, "peer disconnected");
	}

end:

	return (ssize_t)ret;/* success or fail */

}


static void * receive_routine (struct _sock_recv_t * params)
{

	int ret;
	int sockfd;
	/* pthread_t tid; */
	uint8_t teminate;
	should_teminate_recv_t * should_teminate_recv;
	on_received_t * on_received;
	will_finish_t * will_finish;
	uint8_t * buf;
	sock_will_finish_t fiparams;
	sock_recved_t redparams;

	/* dump */
	sockfd = params->sockfd;
	/* tid = params->tid; */
	on_received = params->on_received;
	will_finish = params->will_finish;
	should_teminate_recv = params->should_teminate_recv;
	fiparams.info.peer_ip = params->info.peer_ip;
	fiparams.info.peer_port = params->info.peer_port;
	redparams.info.peer_ip = params->info.peer_ip;
	redparams.info.peer_port = params->info.peer_port;

	usleep(5 * 1e3); /* 5 mesc */
	free(params);
	params = NULL;

	fiparams.sockfd = sockfd;

	teminate = should_teminate_recv(sockfd);
	buf = malloc(sizeof(uint8_t) * 4096);
	redparams.data = buf;
	redparams.sockfd = sockfd;

	while (0 == teminate)
	{
		bzero(buf, 4096);

		ret = recv_from_sockfd(sockfd, buf, 0, 4096, 15 * 1e6, 15 * 1e6);

		if ((ret < 0) && (ret > -1000)) {
			log2stream(stdout, "will exit: %d", ret);
			fiparams.code = ret;/* fail */
			will_finish(fiparams);
			return NULL;
		} else if (0 == ret) {
			/* disconnected */
			log2stream(stdout, "will exit");
			fiparams.code = 0;
			will_finish(fiparams);
			return NULL;
		} else if (ret > 0) {
			/* success */
			redparams.count = ret;
			on_received(redparams);
		} else {
			/* < -1000 timeout */
		}

		teminate = should_teminate_recv(sockfd);
	}

	fiparams.code = 1;/* user terminate */
	will_finish(fiparams);

	return (void *)0;/* success */

}


static int start_receive_from_peer (struct _sock_recv_t * params)
{

	int ret;
	pthread_t tid;

#if defined(UNIX_SOCK_DEBUG)
	log2stream(stdout, "start");
#endif

	/* let thread to do recv */
	ret = pthread_create(&tid, NULL,
		(void *(*)(void *))receive_routine, params);

	if (0 != ret) {
		errno = ret;

		error2stream(stderr, "start receive fail");

		return -ret;
	} else {
		/* success */
		params->tid = tid;
		log2stream(stdout, "start receive success: tid: %lu", tid);
		return 0;
	}

}


/*
 * NAME: connect by socket fd  - oneshot
 */
static void * connect_by_sock_fd (struct _sock_conn_t * params)
{

	int ret;
	int sockfd;
	uint16_t timeout;
#if defined(__APPLE__)
	int set;
#	endif
	int code;
	fd_set fdwrite;
	pthread_t tid;
	struct timeval tv_select;
	on_connect_t * on_connect;
	will_finish_t * will_finish;
	on_received_t * on_received;
	should_teminate_recv_t * should_teminate_recv;
	struct _sock_recv_t * rcvparams = NULL;
	sock_info_t info;
	sock_on_conn_t oncparams;


	if (NULL == params) {
		log2stream(stderr, "null params");
		return (void *)-EINVAL;
	}

	/* dump */
	sockfd = params->sockfd;
	timeout = params->timeout;
	tid = params->tid;
	on_connect = params->on_sock.on_connect;
	will_finish = params->on_sock.will_finish;
	on_received = params->on_sock.on_received;
	should_teminate_recv = params->on_sock.should_teminate_recv;
	memcpy(&info, &(params->info), sizeof(sock_info_t));

	usleep(5 * 1e3); /* 5 mesc */
	free(params);
	params = NULL;

	log2stream(stdout, "tid: %lu begin", tid);

	/* wait(select) writeable */
	FD_ZERO(&fdwrite);
	FD_SET(sockfd, &fdwrite);

	tv_select.tv_sec = timeout;
	tv_select.tv_usec = 0;

	ret = select(sockfd + 1, NULL, &fdwrite, NULL, &tv_select);

	if (ret < 0) {
		ret = errno;

		log2stream(stderr, "select writable fail");

		goto select_fail;
	} else if (0 == ret) {
		/* timeout */
		ret = ETIMEDOUT;

		log2stream(stderr, "connnect timeout");

		goto select_to;
	} else {
		/* final check sokcet error */
		int errlen = sizeof(ret);
		getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &ret,
			(socklen_t *)&errlen);

		if (0 != ret) {
			/* connect fail */
			log2stream(stderr, "select fail");

			goto select_fail;
		}
	}

#	if defined(UNIX_SOCK_DEBUG)
	log2stream(stdout, "connect success");
#	endif

	/* final ok */

#if defined(__APPLE__)
	set = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set,
		sizeof(int));
#	endif

	rcvparams = (struct _sock_recv_t *)
		malloc(sizeof(struct _sock_recv_t));
	/* FIXME: check and feedback malloc fail */

	rcvparams->sockfd = sockfd;
	rcvparams->will_finish = will_finish;
	rcvparams->on_received = on_received;
	rcvparams->should_teminate_recv = should_teminate_recv;
	memcpy(&(rcvparams->info), &info, sizeof(sock_info_t));

	/* start on received thread */
	ret = start_receive_from_peer(rcvparams);
	if (0 != ret) {
		error2stream(stderr, "start_receive_from_peer fail");
		code = 1;
		if (NULL != rcvparams) {
			free(rcvparams);
		}
	} else {
		code = 0;
	}
	rcvparams = NULL;

	oncparams.code = code;
	oncparams.sockfd = sockfd;
	on_connect(oncparams);

	log2stream(stdout, "tid: %lu end success: %d", tid, ret);

	return (void *)0;/* success */

select_to:
select_fail:

	oncparams.code = -1;
	oncparams.sockfd = -1;
	on_connect(oncparams);

	if (NULL != rcvparams) {
		free(rcvparams);
		rcvparams = NULL;
	}
	log2stream(stdout, "tid: %lu end fail", tid);

	return (void *)-1;/* fail */

}

/**/
int start_conn_by_sock_fd (int sockfd, const sock_start_conn_t * ps)
{

	int ret;
	pthread_t tid;
	sock_start_conn_t d;
	struct _sock_conn_t * dd;


	/* check */
	if ((sockfd < 0) || (NULL == ps)) {
		log2stream(stderr, "invalid sockfd or conn params");
		return -EINVAL;
	}

	memcpy(&d, ps, sizeof(sock_start_conn_t));

	if ((NULL == d.on_sock.on_connect)
		|| (NULL == d.on_sock.will_finish)
		|| (NULL == d.on_sock.on_received)
		|| (NULL == d.on_sock.should_teminate_recv)) {
		log2stream(stderr, "invalid callback");
		return -EINVAL;
	}

	dd = (struct _sock_conn_t*)malloc(sizeof(struct _sock_conn_t));

	if (NULL == dd) {
		error2stream(stderr, "malloc fail");
		return  -ENOMEM;
	}

	dd->sockfd = sockfd;
	dd->timeout = d.timeout;
	memcpy(&(dd->on_sock), &(d.on_sock), sizeof(on_sock_t));
	memcpy(&(dd->info), &(d.info), sizeof(sock_info_t));

	ret = pthread_create(&tid, NULL,
		(void *(*)(void *))connect_by_sock_fd, dd);

	if (0 != ret) {
		errno = ret;
		error2stream(stderr, "start connect 2 fail");

		free(dd);
		dd = NULL;
		goto sconnfail;
	} else {
		/*  success */
		dd->tid = tid;
#if 	defined(UNIX_SOCK_DEBUG)
		log2stream(stdout, "start connect success: tid: %lu", tid);
#		endif
		return 0;
	}

sconnfail:
	log2stream(stdout, "tid: %lu end fail", tid);
	return -ret;/* fail */

}
