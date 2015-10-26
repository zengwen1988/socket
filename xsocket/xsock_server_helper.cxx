#include <xsocket/sock_server_helper.hxx>

#include <xsocket/sock_core.hxx>

#include <xsocket/on_server_socket.hxx>
// #include <xsocket/sock_server.hxx>
#include <xsocket/sock_server_accept_routine.hxx>

#if !defined(WIN32)
#	include <sys/socket.h>
#	include <netinet/in.h>
#	include <arpa/inet.h>
#else
#endif

#if !defined(NO_C_LOGFILE)
#	include <c_logfile.h>
#endif

int xsocket::SockServerHelper::startServer (
	const xsocket::NetProtocol& bindto,
	xsocket::OnServerSocket * server_callback)
{

	int ret;
	/* xsocket::SockServer * sv = NULL; */

#if !defined(NO_C_LOGFILE) && defined(ENABLE_SOCK_DEBUG)
	/* trace */ char dmsg[128];
	snprintf(dmsg, 127, "func: %s", __func__); dmsg[127] = '\0';
	clogf_append_v2(dmsg, __FILE__, __LINE__, 0);
#endif

	if (NULL == server_callback) {
		return -1;/* invalid */
	}

#if	0
	WORD versionRequested;
    WSADATA dt;

    versionRequested = MAKEWORD(1,1);
	/* load winsocket dll */
    ret = WSAStartup(versionRequested, &dt);
#endif
	ret = xsocket::core::InitSocketEnvironment();

	if (0 != ret) {
		return -2;/* load winsocket dll fail */
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET/*PF_INET*/;
	addr.sin_addr.s_addr = inet_addr(bindto.ip);
	addr.sin_port = htons(bindto.port);

	/* SOCK_STREAM -- here is TCP FIXME use all params */
	int sockfd = socket(AF_INET /* PF_INET */, SOCK_STREAM, 0);

	if (sockfd < 0) {
		clogf_append_v2("socket fail", __FILE__, __LINE__, sockfd);
		return -3;/* create socket fail */
	}

	ret = bind(sockfd, (const sockaddr *)&addr, sizeof(sockaddr_in));

	if (ret < 0) {
		ret = -4;
		goto bind_fail;
	}

	ret = listen(sockfd, 10);

	if (ret < 0) {
		ret = -5;
		goto listen_fail;
	}

	/* set fd before start */
	server_callback->set_sockfd(sockfd);

	/* start accept thread */
	ret = server_callback->startAccept();

	if (0 != ret) {
		ret = -6;
		goto start_accept_fail;
	}

	/* return sockfd; */
	return  0;

start_accept_fail:
	if (NULL != server_callback) {
		delete server_callback;
		server_callback = NULL;
	}

listen_fail:

bind_fail:
	xsocket::core::ShutdownSocket(sockfd, xsocket::ShutdownHow::RDWR);
	xsocket::core::CloseSocket(sockfd);
	return ret;

}
