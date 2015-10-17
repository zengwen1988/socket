#include <unix/SocketClientHelper.h>
#include <unix/SocketClientUtil.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include <unistd.h> /* close */
#include <pthread.h> /* pthread_create */

#include <c_log.h>


typedef int(on_connect_t)(unix_sock_on_conn_params_t);

class ConnectRoutineParams {

friend class UNIXSocketClientHelper;

public: int getSockfd () const {
	return this->sockfd;
}


public: uint16_t getTimeout () const {
	return this->timeout;
}


public: on_connect_t * getOnConnect () const {
	return this->onConnect;
}


public: uint32_t getServerIP () const {
	return this->serverIP;
}


public: uint16_t getServerPort () const {
	return this->serverPort;
}


public: pthread_t getTid () const {
	return this->tid;
}


protected: int sockfd;
protected: uint16_t timeout;/* seconds */
protected: int(*onConnect)(unix_sock_on_conn_params_t);

protected: uint32_t serverIP;/* host order */
protected: uint16_t serverPort;/* host order */

protected: pthread_t tid;

};


static void * connectRoutine (ConnectRoutineParams * params)
{

	int ret;
	int sockfd;
	uint16_t timeout;
#if defined(__APPLE__)
	int set;
#	endif
	uint32_t ip;
	uint16_t port;
	fd_set fdwrite;
	pthread_t tid;
	struct timeval tv_select;
	on_connect_t * onConnect;
	unix_sock_on_conn_params_t onConnParams;

	sockfd = params->getSockfd();
	timeout = params->getTimeout();
	onConnect = params->getOnConnect();
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


	onConnParams.code = 0;
	onConnParams.fd = sockfd;
	onConnParams.pairIP = ip;
	onConnParams.pairPort = port;

	ret = onConnect(onConnParams);

	log2stream(stdout, "tid: %lu end success: %d", tid, ret);

	return (void *)0;/* success */

select_to:
select_fail:

	UNIXSocketClientUtil::setSocketBlock(sockfd, true);
	close(sockfd);
	sockfd = -1;

	log2stream(stdout, "tid: %lu end fail", tid);

	return (void *)-1;/* fail */

}


int UNIXSocketClientHelper::startConnectToServer (
	const UNIXSockConnParams * params)
{

	int ret;
	struct sockaddr_in sa;
	int sockfd = -1;
	uint32_t ip;
	uint16_t port;
	uint16_t timeout;
	pthread_t * tid, tid2;
	on_connect_t * onConnect;
	ConnectRoutineParams * cparams;

#if defined(UNIX_SOCK_DEBUG)
	log2stream(stdout, "start");
#endif

	/* check */
	if ((NULL == params)
		|| (NULL == params->onConnect)) {

		errno = EINVAL;
		log2stream(stderr, "null or invalid params");
		return -EINVAL;

	}

	ip = params->ip;
	port = params->port;
	onConnect = params->onConnect;
	timeout = params->timeout;

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

	/* ready conn params */
	cparams = new ConnectRoutineParams();
	cparams->sockfd = sockfd;
	cparams->timeout = timeout;
	cparams->onConnect = onConnect;
	cparams->serverIP = ip;
	cparams->serverPort = port;
	tid = &(cparams->tid);

	/* let thread to do connect */
	ret = pthread_create(tid, NULL,
		(void *(*)(void *))connectRoutine, cparams);
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

conn_fail:
	UNIXSocketClientUtil::setSocketBlock(sockfd, true);
	close(sockfd);
	sockfd = -1;

sock_fail:

	return (int32_t)-ret;
}
