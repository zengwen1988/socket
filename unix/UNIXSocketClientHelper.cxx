#include <unix/Socket.h>
#include <unix/SocketClientHelper.h>
#include <unix/SocketClientUtil.h>



#include <unistd.h> /* close */
#include <pthread.h> /* pthread_create */

#include <c_log.h>


int UNIXSocketClientHelper::startConnectToServer (
	const UNIXSockConnParams * params)
{

	int ret;
	int sockfd;
	uint32_t ip;
	uint16_t port;
	uint16_t timeout;
	pthread_t * tid, tid2;
	UNIXOnSocket * onSocket;
	ConnectRoutineParams * cparams;
	void * socketRoutine;

#if defined(UNIX_SOCK_DEBUG)
	log2stream(stdout, "start");
#endif

	/* check */
	if ((NULL == params)
		|| (NULL == params->getOnSocket())) {

		errno = EINVAL;
		log2stream(stderr, "null or invalid params");
		return -EINVAL;goto conn2fail;

	}

	ip = params->getIP();
	port = params->getPort();
	onSocket = params->getOnSocket();
	timeout = params->getTimeout();

	sockfd = UNIXSocket::startConnectByProtocol(ip, port);

	if (sockfd < 0) {
		ret = -sockfd;
		log2stream(stderr, "startConnectByProtocol fail");
		goto conn_fail;
	}

	/* ready conn params */
	cparams = new ConnectRoutineParams();
	cparams->sockfd = sockfd;
	cparams->timeout = timeout;
	cparams->onSocket = onSocket;
	cparams->serverIP = ip;
	cparams->serverPort = port;
	tid = &(cparams->tid);

	/* let thread to do connect */
	socketRoutine = (void *)(UNIXSocket::getS2ConnectRoutine());
	ret = pthread_create(tid, NULL,
		(void *(*)(void *))socketRoutine, cparams);
	tid2 = *tid;
	tid = NULL;

	if (0 != ret) {
		errno = ret;

		log2stream(stderr, "start connect 2 fail");

		goto conn2fail;
	} else {
		/* success */
		log2stream(stdout, "start connect success: tid: %lu", tid2);
		return 0;
	}

conn2fail:

	UNIXSocketClientUtil::setSocketBlock(sockfd, true);
	close(sockfd);
	sockfd = -1;

conn_fail:

	return -ret;

}


int UNIXSocketClientHelper::startReceiveFromServer (
	int sockfd, UNIXOnSocket * onSocket, uint32_t serverIP,
	uint16_t serverPort) {

	int ret;
	pthread_t * tid, tid2;
	void * socketRoutine;
	RecveiveParams * rparams;

#if defined(UNIX_SOCK_DEBUG)
	log2stream(stdout, "start");
#endif

	/* check */
	if (NULL == onSocket) {
		log2stream(stderr, "null onSocket");
		return  -EINVAL;
	}

	/* ready conn params */
	rparams = new RecveiveParams();
	rparams->sockfd = sockfd;
	rparams->onSocket = onSocket;
	rparams->serverIP = serverIP;
	rparams->serverPort = serverPort;
	tid = &(rparams->tid);

	/* let thread to do recv */
	socketRoutine = (void *)(UNIXSocket::getReceiveRoutine());
	ret = pthread_create(tid, NULL,
		(void *(*)(void *))socketRoutine, rparams);
	tid2 = *tid;
	tid = NULL;

	if (0 != ret) {
		errno = ret;

		error2stream(stderr, "start receive fail");

		return -ret;
	} else {
		/* success */
		log2stream(stdout, "start receive success: tid: %lu", tid2);
		return 0;
	}

}
