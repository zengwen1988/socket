#include <unix/Socket.h>
#include <unix/OnSocket.h>

#include <c_log.h>

#include <unistd.h>
#include <pthread.h>
#include <fcntl.h> /* F_GETFL .. */
#include <sys/socket.h> /* SOL_SOCKET .. */
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>


void * UNIXSocket::receiveRoutine (UNIXSockReceiveParams * params)
{

	int ret;
	int sockfd;
	/* pthread_t tid; */
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
	/* tid = params->tid; */

	usleep(5 * 1e3); /* 5 mesc */
	delete params;
	params = NULL;

	fiparams.sockfd = sockfd;

	teminate = onSocket->shouldTeminateRecv(sockfd);
	buf = new uint8_t[4096];
	// redparams.data = buf;
	// redparams.sockfd = sockfd;

	while (0 == teminate)
	{
		bzero(buf, 4096);

		ret = recv_from_sockfd(sockfd, buf, 0, 4096, 15 * 1e6, 15 * 1e6);

		if ((ret < 0) && (ret > -1000)) {
			log2stream(stdout, "will exit: %d", ret);
			fiparams.code = ret;/* fail */
			onSocket->willFinish(fiparams);
			return NULL;
		} else if (0 == ret) {
			/* disconnected */
			log2stream(stdout, "will exit");
			fiparams.code = 0;
			onSocket->willFinish(fiparams);
			return NULL;
		} else if (ret > 0) {
			/* success */
			// redparams.count = ret;
			onSocket->onReceived(buf, ret);
		} else {
			/* < -1000 timeout */
		}

		teminate = onSocket->shouldTeminateRecv(sockfd);
	}

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
 * NAME: connect by socket fd  - oneshot
 */
// static void * connectBySockfd (UNIXSockConnParams * params)
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
