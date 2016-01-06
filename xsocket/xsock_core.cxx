/* ==========================================================================
 *
 * NAME xsocket/xsock_client_core.cc
 *   All client socket core here
 *
 * DESC
 *   - This project all (socket related only) on STL and NO C++ 11
 *   - Funcs xsocket::sock::* and class xsocket::SockClientCore
 *
 * CREATED
 *   2015-10 by Yui Wong
 *   E-mail: yuiwong@126.com
 *
 * VERSIONS
 *   - 1.0.0.1 - 2015-10-24 PM +0800
 *   First stable verion
 *
 * LICENSE
 *   GNU LESSER GENERAL PUBLIC LICENSE Version 3
 *
 *   This file is part of SOCKET.
 *   SOCKET is free software: you can redistribute it
 *   and/or modify it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation, either version 3 of the
 *   License, or (at your option) any later version.
 *   SOCKET is distributed in the hope that it will be
 *   useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with SOCKET.
 *   If not, see <http://www.gnu.org/licenses/>.
 *
 * ==========================================================================
 */

#include <xsocket/sock_core.hxx>

#include <errno.h> /* errno */
#include <cstring>

#if !defined(WIN32)
#	include <unistd.h>
#	include <sys/socket.h> /* SOL_SOCKET .. */
#else
#	include <windows.h>
#endif
#include <fcntl.h> /* F_GETFL .. */

#include <posix/simp_sync_mutx.h>
#include <x_logfile.hxx>
#include <simple_net_conv.h>


/*
 * NAME func xsocket::core::GetSockfdByHostN
 *   get sockfd by IP and PORT and both are Net order
 */
int xsocket::core::GetSockfdByHost (const xsocket::NetProtocol& host)
{
	int ret;
	struct sockaddr_in sa;
	int sockfd;
	uint32_t ipv4;

	xlog::AppendV2("xsocket::core::GetSockfdByHost", __FILE__, __LINE__, 0);

	/*
	 * conv and check
	 * FIXME: support IPv6
	 */
	ret = ipv4_to_netu32_r(&ipv4, host.ip);
	if (0 != ret) {
		xlog::AppendV2("Invalid IPv4", __FILE__, __LINE__, ret);
		return ret;
	}

	sa.sin_addr.s_addr = ipv4;

	sa.sin_family = AF_INET;
	sa.sin_port = htons(host.port);

	/* SOCK_STREAM -- here is TCP FIXME use all params */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0) {
		ret = errno;
		goto sock_fail;
	}

	xsocket::core::SetSocketBlock(sockfd, false);

	/* connect */
	ret = connect(sockfd, (const struct sockaddr *)&sa, sizeof(sa));

#	if defined(EINPROGRESS)
	if ((ret < 0) && (EINPROGRESS != errno)) {
#	else
	if ((ret < 0) && (115 != errno)) {
#	endif
		ret = errno;

		goto conn_fail;
	}

	xsocket::core::SetSocketBlock(sockfd, true);

	/* success */
	return sockfd;

conn_fail:
	xsocket::core::SetSocketBlock(sockfd, true);
	xsocket::core::CloseSocket(sockfd);
	sockfd = -1;

sock_fail:
	if (ret > 0) {
		ret = -1 * ret;
	}

	return ret;
}


/*
 * NAME func xsocket::core::SendData
 *
 * DESC
 *   - For both client and server
 *   - Send start is: buf + start
 *   - NOTE: should use valid arguments
 */
/* static uint8_t __send_data_unlocked = 1; */
ssize_t xsocket::core::SendData (int sockfd, const uint8_t * data, int start,
	size_t _count)
{
	int ret;
	ssize_t wo = 0;/* already wrote */
	ssize_t w;
	ssize_t count = static_cast<ssize_t>(_count), fi = 0;

	/* CAPTURE_LOCK(__send_data_unlocked); */
	while (count > wo) {
#		if !defined(WIN32)
		w = write(sockfd, data + start + (int)wo, count - wo);
#		else
		w = send(sockfd, (const char *)(data + start + (int)wo), count - wo,
			0);/* 0x40 no block, now 0 */
#		endif
		xlog::AppendV2("sent", __FILE__, __LINE__, sockfd, XLOG_LEVEL_I);

		if (w < 0) {
			ret = -errno;
			if (ret >= 0) {
				ret = -1;
			}
			fi = (ssize_t)ret;
			goto end;
		} else {
			wo += w;
		}
	}
	fi = wo;
end:
	/* RELEASE_LOCK(__send_data_unlocked); */
	return fi;
}


/*
 * NAME func xsocket::core::SetSocketBlock
 *
 * RETURN
 *   - success: 0
 *   - fail: -errno
 */
int xsocket::core::SetSocketBlock(int sockfd, bool block)
{

	int ret;

#if	!defined(WIN32)
	int flags;

	flags = fcntl(sockfd, F_GETFL, 0);
	if (flags < 0) {
		return -errno;
	}

	if (0 == block) {
		ret = fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
	} else {
		flags &= ~ O_NONBLOCK;
		ret = fcntl(sockfd, F_SETFL, flags);
	}
#else
	/* Set to none block */
	unsigned long nb;
	if (block) {
		nb = 0;
	} else {
		nb = 1;
	}
	ret = ioctlsocket(sockfd, FIONBIO, (unsigned long *)&nb);
	/* if ret is SOCKET_ERROR => fail */
#endif

	if (ret < 0) {
		return -errno;
	} else {
		return 0;
	}

}

/*
 * Shut down all or part of the connection open on socket FD.
 *
 * HOW determines what to shut down:
 *   SHUT_RD   = No more receptions;
 *   SHUT_WR   = No more transmissions;
 *   SHUT_RDWR = No more receptions or transmissions.
 *
 * Returns 0 on success, -1 for errors.
 */
int xsocket::core::ShutdownSocket (int sockfd,
	xsocket::ShutdownHow::Type how)
{
	int ret = shutdown(sockfd, (int)how);
	int e = errno;

	char msg[64];
	snprintf(msg, 63, "FAIL: shutdown: %s", strerror(e));
	xlog::AppendV2(msg, __FILE__, __LINE__, ret);

	if (0 != ret) {
		return (0 != e) ? -e : -1;
	} else {
		return ret;
	}
}

/*
 * NAME func xsocket::core::CloseSocket
 *
 * RETURN
 *   - success: 0
 *   - fail: -errno
 */
int xsocket::core::CloseSocket (int sockfd)
{
#if !defined(WIN32)
	return close(sockfd);
#else
	return closesocket(sockfd);
#endif
}

int xsocket::core::RecvFromSockfd (int sockfd, uint8_t * buf,
	int32_t start, size_t max,
	uint32_t wr_us, uint32_t r_us)
{
	static uint32_t show = 0;
	struct timeval timeout, timeout_r;
	fd_set readfds;
	int nfds, ret;

	++show;
	if (0 == (show % 5)) {
		char dmsg[256];
		snprintf(dmsg, 255,
			"fd: %d. buf: %p. start: %d. max: %zu. wr_us: %u. r_us: %u",
			sockfd, buf, start, max, wr_us, r_us);
		dmsg[255] = '\0';
		xlog::AppendV2(dmsg, __FILE__, __LINE__, 0, XLOG_LEVEL_I);
	}

	/* set select timeout */
	timeout.tv_sec = (long int)(wr_us / 1e6);
	timeout.tv_usec = wr_us % (int)1e6;

	/* set recv fd */
	FD_ZERO(&readfds);
	FD_SET((uint32_t)sockfd, &readfds);
	nfds = (int)sockfd + 1;

	/* selecting */
	ret = select(nfds, &readfds, NULL, NULL, &timeout);
	if (-1 == ret) {
		if (EINTR == errno) {
			/* Should again */
			ret = -1004;
			goto end;
		}
		/* error */
		xlog::AppendV2("select fail", __FILE__, __LINE__, -errno);
		ret = -1;
		goto end;
	} else if (0 == ret) {
		/* no ready and timeout */
		/* log2stream(stdout, "timeout"); */
		ret = -1002;
		goto end;
	}

	/* set recv */
	timeout_r.tv_sec = (long int)(r_us / 1e6);
	timeout_r.tv_usec = r_us % (int)1e6;

	/* FIXME: check ret */
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,
#		if !defined(WIN32)
		&timeout_r,
		(socklen_t)sizeof(struct timeval));
#		else
		(const char *)(&timeout_r),
		sizeof(struct timeval));
#		endif

	/* XXX-FIXED: MSG_WAITALL -> MSG_DONTWAIT iOS ! */
#	if !defined(WIN32)
	ret = (int)recv(sockfd, buf + start, max,
		MSG_DONTWAIT);
#	else
	ret = (int)recv(sockfd, (char *)(buf + start), max,
		0);
#	endif

	/* FIXME: check if here ok */
	if ((ret < 0)
#		if !defined(WIN32)
		&& (EAGAIN == errno)) {
#		else
		&& (11 == errno)) {
#		endif
		/* recv timeout */
		ret = -1003;
		goto end;
	} else if ((ret < 0)
#		if !defined(WIN32)
		&& (EWOULDBLOCK == errno)) {
#		else
		&& (11 == errno)) {
#		endif
		/* recv: EWOULDBLOCK */
		xlog::AppendV2("recv fail", __FILE__, __LINE__, -errno);
		ret = -2;
		goto end;
	} else if (ret <0) {
		xlog::AppendV2("recv fail", __FILE__, __LINE__, -errno);
		ret = -3;
		goto end;
	} else if (0 == ret) {
		xlog::AppendV2("Peer disconnected", __FILE__, __LINE__, 1);
	}

end:
	return (ssize_t)ret;/* success or fail */
}


int xsocket::core::InitSocketEnvironment (void)
{
#if defined(WIN32)
	WSADATA  d;

	/* init windows socket */
	return WSAStartup(MAKEWORD(2, 2), &d);
#endif

	return 0;
}

void xsocket::core::DeinitSocketEnvironment (void)
{
#if defined(WIN32)
	WSACleanup();
#endif
}
