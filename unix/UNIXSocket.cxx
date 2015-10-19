#include <unix/Socket.h>
#include <unix/OnSocket.h>

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

#include <c_log.h>


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
int UNIXSocket::startConnectByIP (uint32_t ip, uint16_t port)
{
	return get_sockfd_by_ipn(htonl(ip), htons(port));
}


int UNIXSocket::startConnBySockfd (int sockfd,
	const UNIXSockStartConnParams * ps)
{
	int ret;
	pthread_t tid;

#if	defined(UNIX_SOCK_DEBUG)
	log2stream(stdout, "start");
#endif

	/* check */
	if ((sockfd < 0) || (NULL == ps)) {
		log2stream(stderr, "invalid sockfd or conn params");
		return -EINVAL;
	}

	if (NULL == ps->getOnSocket()) {
		log2stream(stderr, "invalid callback");
		return -EINVAL;
	}

	UNIXSockConnParams * dps = new UNIXSockConnParams();
	dps->sockfd = sockfd;
	dps->ip = ps->getIP();
	dps->onSocket = ps->getOnSocket();
	dps->port = ps->getPort();
	dps->timeout = ps->getTimeout();

	ret = pthread_create(&tid, NULL,
		(void *(*)(void *))UNIXSocket::connectBySockfd, dps);

	if (0 != ret) {
		errno = ret;
		error2stream(stderr, "start connect 2 fail");

		delete dps;
		dps = NULL;
		goto sconnfail;
	} else {
		/*  success */
		dps->tid = tid;
#if 	defined(UNIX_SOCK_DEBUG)
		log2stream(stdout, "start connect success: tid: %lu", tid);
#		endif
		return 0;
	}

sconnfail:
	log2stream(stdout, "tid: %lu end fail", tid);
	return -ret;/* fail */

}


void * UNIXSocket::connectByDomain (UNIXSockConnParamsDomain * domain)
{

	int code;
	int ret;
	struct hostent * hptr;
	char ipstr[32];
	uint32_t ipn;
	uint16_t portn;
	char dom[1024];
	UNIXOnSocket * onSocket;
	sock_on_conn_t cparams;
	uint16_t timeout;
	int sockfd;
	fd_set fdwrite;
	struct timeval tv_select;
	UNIXSockReceiveParams * rcvparams = NULL;

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

	/* parse dns */
	hptr = gethostbyname(dom);

	if ((NULL == hptr) || (NULL == hptr->h_addr)) {
		goto gethostfail;
	}

	if (AF_INET != hptr->h_addrtype) {
		goto gethosttypefail;
	}

	/* fisrt addr */
	bzero(ipstr, 32);
	inet_ntop(hptr->h_addrtype, hptr->h_addr, ipstr, sizeof(ipstr));
	ipstr[31] = '\0';

#	if defined(UNIX_SOCK_DEBUG)
	log2stdout("ip: %s", ipstr);
#	endif

	ipn = inet_addr(ipstr);
	sockfd = get_sockfd_by_ipn(ipn, portn);

	if (sockfd < 0) {
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

	rcvparams = new UNIXSockReceiveParams();
	rcvparams->onSocket = onSocket;
	rcvparams->peerIP = ntohl(ipn);
	rcvparams->peerPort = ntohs(portn);
	rcvparams->sockfd = sockfd;

	/* start on received thread */
	ret = UNIXSocket::startReceiveFromPeer(rcvparams);
	if (0 != ret) {
		error2stream(stderr, "start_receive_from_peer fail");
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

	log2stdout("end success: %d", ret);

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
	return (void *)-1;

getsockfd_fail:
	error2stream(stderr, "get sock fd fail");
	goto fail;

gethosttypefail:
	error2stream(stderr, "unsupport type");
	goto fail;

gethostfail:
	error2stream(stderr, "parse dns fail");

fail:
	cparams.code = -1;
	cparams.sockfd = -1;
	onSocket->onConnect(cparams);
	return (void *)-1;

}


int UNIXSocket::startConnectByDomain (const UNIXSockStartConnParamsD * ps)
{
	int ret;
	pthread_t tid;
	UNIXSockConnParamsDomain * domainps;

#if	defined(UNIX_SOCK_DEBUG)
	show_trace();
#endif

	/* check */
	if ((NULL == ps)
		|| (NULL == ps->getOnSocket())
		|| (strlen(ps->getDomain()) <= 0)) {
		log2stream(stderr, "invalid domain or params");
		return -EINVAL;
	}

	if (NULL == ps->getOnSocket()) {
		log2stream(stderr, "invalid callback");
		return -EINVAL;
	}

	domainps = new UNIXSockConnParamsDomain();
	domainps->onSocket = ps->getOnSocket();
	domainps->port = ps->getPort();
	domainps->timeout = ps->getTimeout();
	bzero(domainps->domain, 1024);
	strncpy(domainps->domain, ps->getDomain(), 1024);
	domainps->domain[1023] = '\0';

	ret = pthread_create(&tid, NULL,
		(void *(*)(void *))UNIXSocket::connectByDomain, domainps);

	if (0 != ret) {
		errno = ret;
		error2stream(stderr, "start connect 2 fail");

		delete domainps;
		domainps = NULL;
		goto sconnfail;
	} else {
		/*  success */
		domainps->tid = tid;
#if 	defined(UNIX_SOCK_DEBUG)
		log2stream(stdout, "start connect success: tid: %lu", tid);
#		endif
		return 0;
	}

sconnfail:
	log2stream(stdout, "tid: %lu end fail", tid);
	return -ret;/* fail */

}


/*
 * NAME
 *   receiveRoutine - receive routine
 *
 * SYNOPSIS
 *   protected: static void * receiveRoutine(UNIXSockReceiveParams * params);
 */
void * UNIXSocket::receiveRoutine (UNIXSockReceiveParams * params)
{

	int ret;
	int sockfd;
	pthread_t tid;
	/* uint32_t ip;
	uint16_t port;
	*/
	bool teminate;
	UNIXOnSocket * onSocket;
	uint8_t * buf;
	sock_will_finish_t fiparams;

	/* dump */
	sockfd = params->getSockfd();
	onSocket = params->getOnSocket();
	/* ip = params->getPeerIP();
	port = params->getPeerPort();
	*/
	usleep(5 * 1e3); /* 5 mesc */
	tid = params->getTid();/* get tid */

	delete params;
	params = NULL;

#if	defined(UNIX_SOCK_DEBUG)
	/* show tid when debug */
	log2stdout("tid: %lu begin", tid);
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
			log2stdout("will exit: fail: %d", ret);
			fiparams.code = ret;/* fail */
			onSocket->willFinish(fiparams);
			return NULL;
		} else if (0 == ret) {
			/* disconnected */
			log2stream(stdout, "will exit: peer disconnected");
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

	log2stream(stdout, "will exit: user terminate");
	fiparams.code = 1;/* user terminate */
	onSocket->willFinish(fiparams);

	return (void *)0;/* success */

}


int UNIXSocket::startReceiveFromPeer (UNIXSockReceiveParams * params)
{

	int ret;
	pthread_t tid;

#if defined(UNIX_SOCK_DEBUG)
	log2stream(stdout, "start");
#endif

	/* let thread to do recv */
	ret = pthread_create(&tid, NULL,
		(void *(*)(void *))UNIXSocket::receiveRoutine, params);

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
 * NAME connect by socket fd  - oneshot
 */
void * UNIXSocket::connectBySockfd (UNIXSockConnParams * params)
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
	UNIXOnSocket * onSocket;
	UNIXSockReceiveParams * rcvparams = NULL;
	sock_on_conn_t cparams;

	if (NULL == params) {
		log2stream(stderr, "null params");
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

	rcvparams = new UNIXSockReceiveParams();
	rcvparams->onSocket = onSocket;
	rcvparams->peerIP = ip;
	rcvparams->peerPort = port;
	rcvparams->sockfd = sockfd;

	/* start on received thread */
	ret = UNIXSocket::startReceiveFromPeer(rcvparams);
	if (0 != ret) {
		error2stream(stderr, "start_receive_from_peer fail");
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

	log2stream(stdout, "tid: %lu end success: %d", tid, ret);

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

	log2stream(stdout, "tid: %lu end fail", tid);

	return (void *)-1;/* fail */

}
