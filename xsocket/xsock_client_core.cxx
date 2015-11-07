/* ==========================================================================
 *
 * NAME xsocket/xsock_client_core.cxx
 *   Client socket core here
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

#include <xsocket/basic_sock_type.hxx>
#include <xsocket/sock_core.hxx>
#include <xsocket/on_client_socket.hxx>
#include <xsocket/sock_client_params.hxx>

#include <errno.h> /* errno */
#include <cstring>

#include <unistd.h>
#include <fcntl.h> /* F_GETFL .. */
#include <sys/socket.h> /* SOL_SOCKET .. */
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netdb.h> /* gethostbyname */

#if defined(XSOCKET_LOGLEVEL)
#	include <x_logfile.hxx>
#endif


xsocket::OnClientSocket::OnClientSocket (void)
{
	this->_gc = new xsocket::OnClientSocketGC();
}

xsocket::OnClientSocket::~OnClientSocket ()
{
	if (NULL != this->_gc) {
		delete this->_gc;
		this->_gc = NULL;
	}
}

/*
 * NAME member func xsocket::core::ConnectToServerBySockfd::run
 *   - connect thread routine
 *
 * DESC
 *  - when success: delete arg
 *  - when fail: arg valid => delete arg, on_socket valid => delete on_socket
 */
void * xsocket::core::internal::ConnectToServerBySockfd::run (void * arg)
{
	int code;
	int ret;
#if defined(__APPLE__)
	int set;
#	endif

	int sockfd;
	uint32_t timeout;
	xsocket::internal::SockConnParams * cps;
	xsocket::OnClientSocket * on_socket = NULL;
	xsocket::NetProtocol target;
	xsocket::SockClientRecviveParams * rcvparams = NULL;
	fd_set fdwrite;
	SockOnConnnect on_con;
	struct timeval tv_select;
	char msg[128];

	/* check */
	if (NULL == arg) {
		return (void *)-xsocket::error::SOCKARG_INVAL;
	}

	/* dump */
	cps = (xsocket::internal::SockConnParams *)arg;
	on_socket = cps->on_client_socket();

	if (NULL == on_socket) {
		delete cps;
		cps = NULL;
		return (void *)-xsocket::error::NO_CLIENT_CB;
	}

	/* real dump*/
	sockfd = cps->sockfd();
	timeout = cps->timeout();
	memcpy(&target, cps->target(), sizeof(xsocket::NetProtocol));

	code = 0;
	on_con.code = 0;
	memcpy(&(on_con.info), &target, sizeof(xsocket::NetProtocol));
	on_con.sockfd = sockfd;

	/* wait(select) writeable */
	FD_ZERO(&fdwrite);
	FD_SET(sockfd, &fdwrite);

	tv_select.tv_sec = timeout;
	tv_select.tv_usec = 0;

	ret = select(sockfd + 1, NULL, &fdwrite, NULL, &tv_select);

	if (ret < 0) {
		ret = errno;

#if		!defined(NO_X_LOGFILE)
		xlog::AppendV2("select writable fail", __FILE__, __LINE__, -ret);
#endif

		code = -ret;
		goto select_fail;
	} else if (0 == ret) {
		/* timeout */
		code = -ETIMEDOUT;

#if		!defined(NO_X_LOGFILE)
		xlog::AppendV2("connnect timeout", __FILE__, __LINE__, -ETIMEDOUT);
#endif

		goto select_to;
	} else {
		/* final check sokcet error */
		int errlen = sizeof(ret);
		getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &ret,
			(socklen_t *)&errlen);

		if (0 != ret) {
			/* connect fail */
			errno = ret;
			code = -ret;
#if			defined(XSOCKET_LOGLEVEL) && (XSOCKET_LOGLEVEL >= 0x10)
			snprintf(msg, 127, "FAIL: select: %s", strerror(ret));
			msg[127] = '\0';
			xlog::AppendV2(msg, __FILE__, __LINE__, code);
#endif

			goto select_fail;
		}
	}

	/* final ok */

#if defined(__APPLE__)
	set = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set,
		sizeof(int));
#	endif

	rcvparams = new xsocket::SockClientRecviveParams(sockfd,
		on_socket, target);

	/* start on received thread */
	ret = xsocket::core::ClientHelper::startReceiveFromPeer(rcvparams);
	if (0 != ret) {
#if		!defined(NO_X_LOGFILE)
		xlog::AppendV2("FAIL: start_receive_from_peer", __FILE__, __LINE__,
			ret);
#endif
		code = ret;
		delete rcvparams;
		rcvparams = NULL;
	} else {
		/* success */
		code = 0;
	}
	rcvparams = NULL;

	/* feedback */
	on_con.code = code;
	on_socket->onConnect(on_con);

	if (0 != code) {
#if		defined(XSOCKET_LOGLEVEL) && (XSOCKET_LOGLEVEL >= 0x10)
		xlog::AppendV2("i'll delete on_socket", __FILE__, __LINE__, ret);
#endif
		delete on_socket;
		on_socket = NULL;
	}

#if	!defined(NO_X_LOGFILE)
	xlog::AppendV2("end success", __FILE__, __LINE__, ret);
#endif

	/* final delete */
	if (NULL != cps) {
		delete cps;
		cps = NULL;
	}

	return (void *)0;/* success */

select_to:
select_fail:
	on_con.code = code;
	on_socket->onConnect(on_con);

	if (NULL != on_socket) {
#if		defined(XSOCKET_LOGLEVEL) && (XSOCKET_LOGLEVEL >= 0x10)
		xlog::AppendV2("i'll delete on_socket", __FILE__, __LINE__, code);
#endif
		delete on_socket;
		on_socket = NULL;
	}

	if (NULL != rcvparams) {
		delete rcvparams;
		rcvparams = NULL;
	}

	/* final delete */
	if (NULL != cps) {
		delete cps;
		cps = NULL;
	}

	return (void *)-1;
}


/*
 * NAME member func xsocket::SockClientCore::startConnectBySockfd
 *
 * PARAMS
 *   TODO
 *
 * RETURN
 *   - 0 when success
 *   - -xsocket::error or -errno when fail
 */
int xsocket::core::ClientHelper::startConnectBySockfd (int sockfd,
	xsocket::SockStartConnParams * params)
{
	int ret;

	/* check */
	if (sockfd < 0) {
		return -xsocket::error::SOCKFD_INVAL;
	}
	if (NULL == params) {
		return -xsocket::error::SOCKARG_INVAL;
	}

	/*
	 * 1 start success: cps will be release in thread
	 * 2 start fail: cps will be release here
	 */
	xsocket::internal::SockConnParams * cps
		= new xsocket::internal::SockConnParams(sockfd,
		params->timeout(),
		params->target(),
		params->on_client_socket());

	xsocket::core::internal::ConnectToServerBySockfd& rr = cps->routine();
	ret = rr.start(cps);

	if (0 != ret) {
		rr.nowFinish();
		delete cps;
		cps = NULL;

		goto sconnfail;
	} else {
		/*  success */
		return 0;
	}

sconnfail:
	return -ret;/* fail */
} /* xsocket::SockClientCore::startConnectBySockfd */


/*
 * NAME member func xsocket::SockClientCore::startReceiveFromPeer
 *
 * PARAMS
 *   TODO
 *
 * RETURN
 *   - 0 when success
 *   - -xsocket::error or -errno when fail
 */
int xsocket::core::ClientHelper::startReceiveFromPeer (
	xsocket::SockClientRecviveParams * params)
{
	int ret;

	/* check */
	if (NULL == params) {
		return -xsocket::error::SOCKARG_INVAL;
	}

	/* TODO: check more */

	xsocket::core::internal::ClientRecevier& cr = params->routine();
	ret = cr.start(params);

	if (0 != ret) {
		cr.nowFinish();
		delete params;
		params = NULL;
	} else {
		/*  success */
		ret = 0;
	}

	return ret;
} /* xsocket::SockClientCore::startConnectBySockfd */


void * xsocket::core::internal::ClientRecevier::run (void * arg)
{
	int ret;
	int sockfd;
	bool teminate;
	xsocket::OnClientSocket * on_socket;
	uint8_t * buf;
	xsocket::SockDidFinish fb;
	xsocket::SockClientRecviveParams * scr;
	xsocket::SockRecved rcv;

	if (NULL == arg) {
#if		!defined(NO_X_LOGFILE)
		xlog::AppendV2("ERRO: NULL XSockReceiveParams params !!", __FILE__,
			__LINE__, -EINVAL);
#endif
		return (void *)-xsocket::error::SOCKARG_INVAL;
	}

	scr = (xsocket::SockClientRecviveParams *)arg;

	/* dump */
	sockfd = scr->sockfd();
	on_socket = scr->on_socket();

	fb.fd = sockfd;/* fill FI sockfd */

	teminate = on_socket->shouldTeminateRecv(sockfd);
	buf = new uint8_t[4096];
	// redparams.data = buf;
	// redparams.sockfd = sockfd;

	rcv.fd = sockfd;
	memcpy(&(rcv.info), scr->from(), sizeof(xsocket::NetProtocol));

	while (0 == teminate)
	{
		bzero(buf, 4096);

		ret = xsocket::core::RecvFromSockfd(
			sockfd, buf, 0, 4096, 15 * 1e6, 15 * 1e6);

		if ((ret < 0) && (ret > -1000)) {
#if			!defined(NO_X_LOGFILE)
			xlog::AppendV2("FAIL: will finish", __FILE__, __LINE__, ret);
#endif
			fb.code = ret;/* fail */
			on_socket->didFinish(fb);

			goto end;
		} else if (0 == ret) {
			/* disconnected */
#if			!defined(NO_X_LOGFILE)
			xlog::AppendV2("will finish: peer disconnected", __FILE__,
				__LINE__, 0);
#endif
			fb.code = 0;
			on_socket->didFinish(fb);
			goto end;
		} else if (ret > 0) {
			/* success */
			// redparams.count = ret;
			rcv.data = buf;
			rcv.count = ret;
			on_socket->onReceived(rcv);
		} else {
			/* < -1000 timeout */
		}

		teminate = on_socket->shouldTeminateRecv(sockfd);
	}

#if	!defined(NO_X_LOGFILE)
	xlog::AppendV2("will finish: user terminate", __FILE__, __LINE__, 1);
#endif
	fb.code = 1;/* user terminate */
	on_socket->didFinish(fb);

end:
	if (NULL != scr) {
		delete scr;
		scr = NULL;
	}

	if (NULL != on_socket) {
#if		defined(XSOCKET_LOGLEVEL) && (XSOCKET_LOGLEVEL >= 0x18)
		xlog::AppendV2("i'll gc on socket", __FILE__, __LINE__, 0);
#endif
		if (NULL != on_socket->gc()) {
			on_socket->gc()->gc(on_socket);
		}
		/* else has released */
		on_socket = NULL;
	}

	return (void *)0;/* success */
}
