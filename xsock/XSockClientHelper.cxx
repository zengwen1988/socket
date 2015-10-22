#include <SockClientHelper.hxx>

#include <OnClientSocket.hxx>
#include <SockClientUtil.hxx>

#include <unistd.h> /* close */
#include <pthread.h> /* pthread_create */

#include <c_log.h>


int XSockClientHelper::startConnectToServer (
	const XSockStartConnParams * params)
{

	int ret;
	int sockfd;
	uint32_t ip;
	uint16_t port;

#if defined(X_SOCK_DEBUG)
	show_trace();
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

	sockfd = XSockClientUtil::startConnectByIP(ip, port);

	if (sockfd < 0) {
		ret = -sockfd;
		log2stream(stderr, "startConnectByProtocol fail");
		goto conn_fail;
	}

	ret = XSockClientUtil::startConnBySockfd(sockfd, params);

	if (0 != ret) {
		log2stream(stderr, "start connect 2 fail");

		goto conn2fail;
	} else {
		/* success */
		set_sock_block(sockfd, 1);

		log2stream(stdout, "start connect success");
		return 0;
	}

conn2fail:
	set_sock_block(sockfd, 1);
	close(sockfd);
	sockfd = -1;

conn_fail:

	return -ret;

}


/*
 * RETURN
 *   - success: 0
 *   - fail: -errno
 */
int XSockClientHelper::startConnectByDomain (
	const XSockStartConnParamsD * ps)
{
	return XSockClientUtil::startConnectByDomain(ps);
}
