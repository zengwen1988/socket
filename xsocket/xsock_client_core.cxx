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

#if !defined(NO_X_LOGFILE)
#	include <x_logfile.hxx>
#endif

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
#if			!defined(NO_X_LOGFILE)
			xlog::AppendV2("select fail", __FILE__, __LINE__, code);
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
		xlog::AppendV2("start_receive_from_peer fail", __FILE__, __LINE__,
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


#if 0
/*
 * DESC
 *   - callback onConnect:
 *   .code: 0 when all success
 *   .code: > 0: with some error can be ignore
 *   .code: < 0: fatal error (-errno):
 *   -EINVAL: argument error
 */
void * XSockClientUtil::connectByDomain (XSockConnParamsDomain * domain)
{

	int code;
	int ret;
#if defined(__APPLE__)
	int set;
#	endif
	struct hostent * hptr;
	char ipstr[32];
	uint32_t ipn;
	uint16_t portn;
	char dom[1024];
	XOnClientSocket * onSocket;
	sock_on_conn_t cparams;
	uint16_t timeout;
	int sockfd;
	fd_set fdwrite;
	struct timeval tv_select;
	XSockReceiveParams * rcvparams = NULL;

	/* dump */
	portn = htons(domain->getPort());
	bzero(dom, 1024);
	strncpy(dom, domain->domain, 1023);
	timeout = domain->timeout;
	onSocket = domain->onSocket;

	/*5 ms */
	usleep(5 * 1e3);
	delete domain;
	domain = NULL;

	code = 0;

	/* parse dns */
	hptr = gethostbyname(dom);

	if ((NULL == hptr) || (NULL == hptr->h_addr)) {
		code = -EINVAL;
		goto gethostfail;
	}

	if (AF_INET != hptr->h_addrtype) {
		code = -EINVAL;
		goto gethosttypefail;
	}

	/* fisrt addr */
	bzero(ipstr, 32);
	inet_ntop(hptr->h_addrtype, hptr->h_addr, ipstr, sizeof(ipstr));
	ipstr[31] = '\0';

#	if defined(ENABLE_SOCK_DEBUG)
	clogf_append(ipstr);
#	endif

	ipn = inet_addr(ipstr);
	sockfd = get_sockfd_by_ipn(ipn, portn);

	if (sockfd < 0) {
		code = sockfd;
		goto getsockfd_fail;
	}

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

		code = errno;
		goto select_fail;
	} else if (0 == ret) {
		/* timeout */
		code = ETIMEDOUT;

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
#if			!defined(NO_X_LOGFILE)
			xlog::AppendV2("select fail", __FILE__, __LINE__, code);
#endif

			goto select_fail;
		}
	}

#	if defined(ENABLE_SOCK_DEBUG)
	clogf_append("connect success");
#	endif

	/* final ok */

#if defined(__APPLE__)
	set = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set,
		sizeof(int));
#	endif

	rcvparams = new XSockReceiveParams();
	rcvparams->onSocket = onSocket;
	rcvparams->peerIP = ntohl(ipn);
	rcvparams->peerPort = ntohs(portn);
	rcvparams->sockfd = sockfd;

	/* start on received thread */
	ret = XSockClientUtil::startReceiveFromPeer(rcvparams);
	if (0 != ret) {
#if		!defined(NO_X_LOGFILE)
		xlog::AppendV2("start_receive_from_peer fail", __FILE__, __LINE__,
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

	memset(&cparams, 0, sizeof(sock_on_conn_t));
	strncpy(cparams.info.ip, ipstr, SOCK_MAX_IP_LEN);
	cparams.info.port = ntohs(portn);
	cparams.code = code;
	cparams.sockfd = sockfd;
	onSocket->onConnect(cparams);

#if	!defined(NO_X_LOGFILE)
	xlog::AppendV2("end success", __FILE__, __LINE__, ret);
#endif

	return (void *)0;/* success */

select_to:
select_fail:

	cparams.code = code;
	cparams.sockfd = -1;
	onSocket->onConnect(cparams);

	if (NULL != rcvparams) {
		delete rcvparams;
		rcvparams = NULL;
	}
	return (void *)-1;

getsockfd_fail:
#if	!defined(NO_X_LOGFILE)
	xlog::AppendV2("get sock fd fail", __FILE__, __LINE__, -1);
#endif
	goto fail;

gethosttypefail:
#if	!defined(NO_X_LOGFILE)
	xlog::AppendV2("unsupport type", __FILE__, __LINE__, -2);
#endif
	goto fail;

gethostfail:
#if	!defined(NO_X_LOGFILE)
	xlog::AppendV2("parse dns fail", __FILE__, __LINE__, -3);
#endif

fail:
	cparams.code = -1;
	cparams.sockfd = -1;
	onSocket->onConnect(cparams);
	return (void *)-1;

}


/*
 * RETURN
 *   - success: 0
 *   - fail: -errno
 */
int XSockClientUtil::startConnectByDomain (const XSockStartConnParamsD * ps)
{
	int ret;
	pthread_t tid;
	XSockConnParamsDomain * domainps;
	char dmsg[256];

	/* check */
	if ((NULL == ps)
		|| (NULL == ps->getOnSocket())
		|| (strlen(ps->getDomain()) <= 0)) {
#if		!defined(NO_X_LOGFILE)
		xlog::AppendV2("invalid domain or params", __FILE__, __LINE__,
			-EINVAL);
#endif
		return -EINVAL;
	}

	if (NULL == ps->getOnSocket()) {
#if		!defined(NO_X_LOGFILE)
		xlog::AppendV2("invalid callback", __FILE__, __LINE__, -EINVAL);
#endif
		return -EINVAL;
	}

	domainps = new XSockConnParamsDomain();
	domainps->onSocket = ps->getOnSocket();
	domainps->port = ps->getPort();
	domainps->timeout = ps->getTimeout();
	bzero(domainps->domain, 1024);
	strncpy(domainps->domain, ps->getDomain(), 1024);
	domainps->domain[1023] = '\0';

	ret = pthread_create(&tid, NULL,
		(void *(*)(void *))XSockClientUtil::connectByDomain, domainps);

	if (0 != ret) {
		errno = ret;
#if		!defined(NO_X_LOGFILE)
		xlog::AppendV2("start connect 2 fail", __FILE__, __LINE__, -ret);
#endif

		delete domainps;
		domainps = NULL;
		goto sconnfail;
	} else {
		/*  success */
		domainps->tid = tid;
#if 	defined(ENABLE_SOCK_DEBUG)
		snprintf(dmsg, 255,
			"start connect success: tid: %lu", (unsigned long)tid);
		dmsg[255] = 0;
		clogf_append(dmsg);
#		endif
		return 0;
	}

sconnfail:
	snprintf(dmsg, 255,
		"tid: %lu end fail", (unsigned long)tid);
	dmsg[255] = 0;
#if	!defined(NO_X_LOGFILE)
	xlog::AppendV2(dmsg, __FILE__, __LINE__, -ret);
#endif
	return -ret;/* fail */

}


/*
 * NAME
 *   receiveRoutine - receive routine
 *
 * SYNOPSIS
 *   protected: static void * receiveRoutine(XSockReceiveParams * params);
 */
void * XSockClientUtil::receiveRoutine (XSockReceiveParams * params)
{

	int ret;
	int sockfd;
	pthread_t tid;
	/* uint32_t ip;
	uint16_t port;
	*/
	bool teminate;
	XOnClientSocket * onSocket;
	uint8_t * buf;
	sock_will_finish_t fiparams;
	char dmsg[256];

	if (NULL == params) {
#if		!defined(NO_X_LOGFILE)
		xlog::AppendV2("ERRO: NULL XSockReceiveParams params !!", __FILE__,
			__LINE__, -EINVAL);
#endif
		return (void *)-EINVAL;
	}

	/* dump */
	sockfd = params->getSockfd();
	onSocket = params->getOnSocket();
	/* ip = params->getPeerIP();
	port = params->getPeerPort();
	*/
	int to = 20;
	do {
		tid = params->getTid(); /* get tid */
		usleep(2 * 1e3);/* 2 msec */
	} while ((0 == tid) && (--to > 0));

	delete params;
	params = NULL;

	if (0 == tid) {
#if		!defined(NO_X_LOGFILE)
		xlog::AppendV2("ERRO: 0 tid !!", __FILE__, __LINE__, -EINVAL);
#endif
		return (void *)-EINVAL;
	}

#if	defined(ENABLE_SOCK_DEBUG)
	/* show tid when debug */
	snprintf(dmsg, 255,
		"tid: %lu begin", (unsigned long)tid);
	dmsg[255] = 0;
	clogf_append(dmsg);
#	endif

	fiparams.sockfd = sockfd;/* fill FI sockfd */

	teminate = onSocket->shouldTeminateRecv(sockfd);
	buf = new uint8_t[4096];
	// redparams.data = buf;
	// redparams.sockfd = sockfd;

	while (0 == teminate)
	{
		bzero(buf, 4096);

		ret = recv_from_sockfd(sockfd, buf, 0, 4096, 15 * 1e6, 15 * 1e6);

		if ((ret < 0) && (ret > -1000)) {
#if			!defined(NO_X_LOGFILE)
			xlog::AppendV2("FAIL: will finish", __FILE__, __LINE__, ret);
#endif
			fiparams.code = ret;/* fail */
			onSocket->willFinish(fiparams);
			return NULL;
		} else if (0 == ret) {
			/* disconnected */
#if			!defined(NO_X_LOGFILE)
			xlog::AppendV2("will finish: peer disconnected", __FILE__,
				__LINE__, 0);
#endif
			fiparams.code = 0;
			onSocket->willFinish(fiparams);
			return NULL;
		} else if (ret > 0) {
			/* success */
			// redparams.count = ret;
			onSocket->onReceived(sockfd, buf, ret);
		} else {
			/* < -1000 timeout */
		}

		teminate = onSocket->shouldTeminateRecv(sockfd);
	}

#if	!defined(NO_X_LOGFILE)
	xlog::AppendV2("will finish: user terminate", __FILE__, __LINE__, 1);
#endif
	fiparams.code = 1;/* user terminate */
	onSocket->willFinish(fiparams);

	return (void *)0;/* success */

}


int XSockClientUtil::startReceiveFromPeer (XSockReceiveParams * params)
{

	int ret;
	pthread_t tid;
	char dmsg[256];

	params->tid = 0;

	/* let thread to do recv */
	ret = pthread_create(&tid, NULL,
		(void *(*)(void *))XSockClientUtil::receiveRoutine, params);

	if (0 != ret) {
		errno = ret;

#if		!defined(NO_X_LOGFILE)
		xlog::AppendV2("start receive fail", __FILE__, __LINE__, -ret);
#endif

		return -ret;
	} else {
		/* success */
		params->tid = tid;
		snprintf(dmsg, 255,
			"start receive success: tid: %lu", (unsigned long)tid);
		dmsg[255] = 0;
		clogf_append(dmsg);
		return 0;
	}

}


/*
 * NAME connect by socket fd  - oneshot
 */
void * XSockClientUtil::connectBySockfd (XSockConnParams * params)
{

	int ret;
	int sockfd;
	uint16_t timeout;
#if defined(__APPLE__)
	int set;
#	endif
	int code;
	uint32_t ip;
	uint16_t port;
	fd_set fdwrite;
	pthread_t tid;
	struct timeval tv_select;
	XOnClientSocket * onSocket;
	XSockReceiveParams * rcvparams = NULL;
	sock_on_conn_t cparams;
	char dmsg[256];

	if (NULL == params) {
#if		!defined(NO_X_LOGFILE)
		xlog::AppendV2("null params", __FILE__, __LINE__, -EINVAL);
#endif
		return (void *)-EINVAL;
	}

	/* dump */
	ip = params->getIP();
	onSocket = params->getOnSocket();
	port = params->getPort();
	sockfd = params->getSockfd();
	timeout = params->getTimeout();

	usleep(5 * 1e3); /* 5 mesc */
	tid = params->getTid();

	delete params;
	params = NULL;

	snprintf(dmsg, 255, "tid: %lu begin", (unsigned long)tid);
	dmsg[255] = 0;
	clogf_append(dmsg);

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

		goto select_fail;
	} else if (0 == ret) {
		/* timeout */
		ret = ETIMEDOUT;

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
#if			!defined(NO_X_LOGFILE)
			xlog::AppendV2("select fail", __FILE__, __LINE__, -ret);
#endif

			goto select_fail;
		}
	}

#	if defined(ENABLE_SOCK_DEBUG)
	clogf_append("connect success");
#	endif

	/* final ok */

#if defined(__APPLE__)
	set = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_NOSIGPIPE, (void *)&set,
		sizeof(int));
#	endif

	rcvparams = new XSockReceiveParams();
	rcvparams->onSocket = onSocket;
	rcvparams->peerIP = ip;
	rcvparams->peerPort = port;
	rcvparams->sockfd = sockfd;

	/* start on received thread */
	ret = XSockClientUtil::startReceiveFromPeer(rcvparams);
	if (0 != ret) {
#if		!defined(NO_X_LOGFILE)
		xlog::AppendV2("start_receive_from_peer fail", __FILE__, __LINE__,
			ret);
#endif
		code = 1;
		delete rcvparams;
		rcvparams = NULL;
	} else {
		code = 0;
	}
	rcvparams = NULL;

	cparams.code = code;
	cparams.sockfd = sockfd;
	onSocket->onConnect(cparams);

	snprintf(dmsg, 255,
		"tid: %lu end success", (unsigned long)tid);
	dmsg[255] = 0;
	clogf_append(dmsg);

	return (void *)0;/* success */

select_to:
select_fail:

	cparams.code = -1;
	cparams.sockfd = -1;
	onSocket->onConnect(cparams);

	if (NULL != rcvparams) {
		delete rcvparams;
		rcvparams = NULL;
	}

	snprintf(dmsg, 255,
		"tid: %lu end fail", (unsigned long)tid);
	dmsg[255] = 0;
#if	!defined(NO_X_LOGFILE)
	xlog::AppendV2(dmsg, __FILE__, __LINE__, -1);
#endif

	return (void *)-1;/* fail */

}
#endif


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
	xsocket::SockWillFinish fb;
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

	fb.sockfd = sockfd;/* fill FI sockfd */

	teminate = on_socket->shouldTeminateRecv(sockfd);
	buf = new uint8_t[4096];
	// redparams.data = buf;
	// redparams.sockfd = sockfd;

	rcv.fd_receiver = sockfd;
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
			on_socket->willFinish(fb);

			goto end;
		} else if (0 == ret) {
			/* disconnected */
#if			!defined(NO_X_LOGFILE)
			xlog::AppendV2("will finish: peer disconnected", __FILE__,
				__LINE__, 0);
#endif
			fb.code = 0;
			on_socket->willFinish(fb);
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
	on_socket->willFinish(fb);

end:
	if (NULL != scr) {
		delete scr;
		scr = NULL;
	}

	return (void *)0;/* success */
}
