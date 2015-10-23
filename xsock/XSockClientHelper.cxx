#include <SockClientHelper.hxx>

#include <OnClientSocket.hxx>
#include <SockClientUtil.hxx>

#include <unistd.h> /* close */
#include <pthread.h> /* pthread_create */
#include <errno.h>

#include <c_logfile.h>


int XSockClientHelper::startConnectToServer (
	const XSockStartConnParams * params)
{

	int ret;
	int sockfd;
	uint32_t ip;
	uint16_t port;

#if defined(ENABLE_SOCK_DEBUG)
	clogf_append_v2("XSockClientHelper::startConnectToServer", __FILE__,
		__LINE__, 0);
#endif

	/* check */
	if ((NULL == params)
		|| (NULL == params->getOnSocket())) {

		clogf_append_v2("null or invalid params", __FILE__, __LINE__, -EINVAL);
		return -EINVAL;

	}

	ip = params->getIP();
	port = params->getPort();

	sockfd = XSockClientUtil::startConnectByIP(ip, port);

	if (sockfd < 0) {
		ret = -sockfd;
		clogf_append_v2("startConnectByProtocol fail", __FILE__, __LINE__,
			ret);
		goto conn_fail;
	}

	ret = XSockClientUtil::startConnBySockfd(sockfd, params);

	if (0 != ret) {
		clogf_append_v2("start connect 2 fail", __FILE__, __LINE__, ret);

		goto conn2fail;
	} else {
		/* success */
		set_sock_block(sockfd, 1);

		clogf_append("start connect success");
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
