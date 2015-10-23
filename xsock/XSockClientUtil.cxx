#include <SockClientUtil.hxx>

#include <OnClientSocket.hxx>

#include <unistd.h>
#include <pthread.h>
#include <fcntl.h> /* F_GETFL .. */
#include <sys/socket.h> /* SOL_SOCKET .. */
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netdb.h> /* gethostbyname */

#include <c_logfile.h>
#include <errno.h> /* errno */


/*
 * NAME startConnectByIP start connect by IP and port
 *
 * SYNOPSIS
 *   public: static int startConnectByIP(uint32_t ip, uint16_t port);
 *
 * PARAMS
 *   - ip: host order
 *   - port: host order
 *
 * RETURN
 *   - sockfd(>= 0) when success
 *   - -errno < 0 when fail
 */
int XSockClientUtil::startConnectByIP (uint32_t ip, uint16_t port)
{
	return get_sockfd_by_ipn(htonl(ip), htons(port));
}


int XSockClientUtil::startConnBySockfd (int sockfd,
	const XSockStartConnParams * ps)
{
	int ret;
	pthread_t tid;
	char dmsg[256];

#if	defined(ENABLE_SOCK_DEBUG)
	clogf_append_v2("XSockClientUtil::startConnBySockfd", __FILE__, __LINE__,
		0);
#endif

	/* check */
	if ((sockfd < 0) || (NULL == ps)) {
		clogf_append_v2("invalid sockfd or conn params", __FILE__, __LINE__,
			-EINVAL);
		return -EINVAL;
	}

	if (NULL == ps->getOnSocket()) {
		clogf_append_v2("invalid callback", __FILE__, __LINE__, -EINVAL);
		return -EINVAL;
	}

	XSockConnParams * dps = new XSockConnParams();
	dps->sockfd = sockfd;
	dps->ip = ps->getIP();
	dps->onSocket = ps->getOnSocket();
	dps->port = ps->getPort();
	dps->timeout = ps->getTimeout();

	ret = pthread_create(&tid, NULL,
		(void *(*)(void *))XSockClientUtil::connectBySockfd, dps);

	if (0 != ret) {
		errno = ret;
		snprintf(dmsg, 255,
			"start connect 2 fail: %s", strerror(ret));
		dmsg[255] = 0;

		delete dps;
		dps = NULL;
		goto sconnfail;
	} else {
		/*  success */
		dps->tid = tid;
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
	clogf_append_v2(dmsg, __FILE__, __LINE__, -ret);
	return -ret;/* fail */

}


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

		clogf_append_v2("select writable fail", __FILE__, __LINE__, -ret);

		code = errno;
		goto select_fail;
	} else if (0 == ret) {
		/* timeout */
		code = ETIMEDOUT;

		clogf_append_v2("connnect timeout", __FILE__, __LINE__, -ETIMEDOUT);

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
			clogf_append_v2("select fail", __FILE__, __LINE__, code);

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
		clogf_append_v2("start_receive_from_peer fail", __FILE__, __LINE__,
			ret);
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

	clogf_append_v2("end success", __FILE__, __LINE__, ret);

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
	clogf_append_v2("get sock fd fail", __FILE__, __LINE__, -1);
	goto fail;

gethosttypefail:
	clogf_append_v2("unsupport type", __FILE__, __LINE__, -2);
	goto fail;

gethostfail:
	clogf_append_v2("parse dns fail", __FILE__, __LINE__, -3);

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
		clogf_append_v2("invalid domain or params", __FILE__, __LINE__,
			-EINVAL);
		return -EINVAL;
	}

	if (NULL == ps->getOnSocket()) {
		clogf_append_v2("invalid callback", __FILE__, __LINE__, -EINVAL);
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
		clogf_append_v2("start connect 2 fail", __FILE__, __LINE__, -ret);

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
	clogf_append_v2(dmsg, __FILE__, __LINE__, -ret);
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
		clogf_append_v2("ERRO: NULL XSockReceiveParams params !!", __FILE__,
			__LINE__, -EINVAL);
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
		clogf_append_v2("ERRO: 0 tid !!", __FILE__, __LINE__, -EINVAL);
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
			clogf_append_v2("FAIL: will finish", __FILE__, __LINE__, ret);
			fiparams.code = ret;/* fail */
			onSocket->willFinish(fiparams);
			return NULL;
		} else if (0 == ret) {
			/* disconnected */
			clogf_append_v2("will finish: peer disconnected", __FILE__,
				__LINE__, 0);
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

	clogf_append_v2("will finish: user terminate", __FILE__, __LINE__, 1);
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

		clogf_append_v2("start receive fail", __FILE__, __LINE__, -ret);

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
		clogf_append_v2("null params", __FILE__, __LINE__, -EINVAL);
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

		clogf_append_v2("select writable fail", __FILE__, __LINE__, -ret);

		goto select_fail;
	} else if (0 == ret) {
		/* timeout */
		ret = ETIMEDOUT;

		clogf_append_v2("connnect timeout", __FILE__, __LINE__, -ETIMEDOUT);

		goto select_to;
	} else {
		/* final check sokcet error */
		int errlen = sizeof(ret);
		getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &ret,
			(socklen_t *)&errlen);

		if (0 != ret) {
			/* connect fail */
			clogf_append_v2("select fail", __FILE__, __LINE__, -ret);

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
		clogf_append_v2("start_receive_from_peer fail", __FILE__, __LINE__,
			ret);
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
	clogf_append_v2(dmsg, __FILE__, __LINE__, -1);

	return (void *)-1;/* fail */

}
