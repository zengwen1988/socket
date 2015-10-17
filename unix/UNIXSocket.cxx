#include <unix/OnSocket.h>
#include <unix/Socket.h>
#include <unix/SocketClientHelper.h>
#include <unix/SocketClientUtil.h>

#include <unistd.h> /* usleep */
#include <fcntl.h> /* F_GETFL .. */
#include <sys/socket.h> /* SOL_SOCKET .. */
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>


#include <c_log.h> /* log2stream */


int UNIXSocket::startConnectByProtocol (uint32_t ip, uint16_t port) {

	int ret;
	struct sockaddr_in sa;
	int sockfd = -1;

	/* gethostbyname */

	sa.sin_addr.s_addr = htonl(ip);

#	if defined(UNIX_SOCK_DEBUG)
	log2stream(stdout, "addr: 0x%x", sa.sin_addr.s_addr);
#	endif

	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);

	/* SOCK_STREAM -- here is TCP */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		ret = errno;

		log2stream(stderr, "socket fail");

		goto sock_fail;
	}

#	if defined(UNIX_SOCK_DEBUG)
	log2stream(stdout, "before connect");
#	endif

	UNIXSocketClientUtil::setSocketBlock(sockfd, false);

	/* connect */
	ret = connect(sockfd, (const struct sockaddr *)&sa, sizeof(sa));

	if ((ret < 0) && (EINPROGRESS != errno)) {
		ret = errno;

		log2stream(stderr, "connect fail");

		goto conn_fail;
	}

	log2stream(stdout, "start to connect: %d success", sockfd);

	/* success */
	return sockfd;

conn_fail:
	UNIXSocketClientUtil::setSocketBlock(sockfd, true);
	close(sockfd);
	sockfd = -1;

sock_fail:

	return (int32_t)-ret;

}


/*
 * NAME: __connect_s2 - oneshot
 */
static void * __connect_s2 (ConnectRoutineParams * params) {

	int ret;
	int sockfd;
	uint16_t timeout;
#if defined(__APPLE__)
	int set;
#	endif
	uint32_t ip;
	uint16_t port;
	int code;
	fd_set fdwrite;
	pthread_t tid;
	struct timeval tv_select;
	UNIXOnSocket * onSocket;
	unix_sock_on_conn_params_t onConnParams;

	sockfd = params->getSockfd();
	timeout = params->getTimeout();
	onSocket = params->getOnSocket();
	ip = params->getServerIP();
	port = params->getServerPort();
	tid = params->getTid();

	usleep(5 * 1e3); /* 5 mesc */
	delete params;
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
	UNIXSocketClientUtil::setSocketBlock(sockfd, true);

#if defined(__APPLE__)
	set = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set,
		sizeof(int));
#	endif



	onConnParams.sockfd = sockfd;
	onConnParams.pairIP = ip;
	onConnParams.pairPort = port;

	/* start on received thread */
	ret = UNIXSocketClientHelper::startReceiveFromServer(sockfd, onSocket,
		ip, port);
	if (0 != ret) {
		error2stream(stderr, "startReceiveFromServer fail");
		code = ret > 0 ? ret : -ret;
	} else {
		code = 0;
	}


	onConnParams.code = code;
	onSocket->onConnect(onConnParams);

	log2stream(stdout, "tid: %lu end success: %d", tid, ret);

	return (void *)0;/* success */

select_to:
select_fail:

	UNIXSocketClientUtil::setSocketBlock(sockfd, true);
	close(sockfd);
	sockfd = -1;

	onConnParams.code = -1;
	onSocket->onConnect(onConnParams);

	log2stream(stdout, "tid: %lu end fail", tid);

	return (void *)-1;/* fail */

}


connect_routine_t * UNIXSocket::getS2ConnectRoutine () {
	return  __connect_s2;
}


static void * __receive_routine (RecveiveParams * params) {

	int ret;
	int sockfd;
	uint32_t ip;
	uint16_t port;
	UNIXOnSocket * onSocket;
	pthread_t tid;
	uint8_t * data;


	/* dump */
	sockfd = params->getSockfd();
	ip = params->getServerIP();
	port = params->getServerPort();
	onSocket = params->getOnSocket();
	tid = params->getTid();

	usleep(5 * 1e3); /* 5 mesc */
	delete params;
	params = NULL;

	return (void *)0;/* success */

}


receive_routine_t * UNIXSocket::getReceiveRoutine() {
	return __receive_routine;
}
