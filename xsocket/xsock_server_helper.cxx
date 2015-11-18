#include <xsocket/sock_server_helper.hxx>

#if !defined(WIN32)
#	include <sys/socket.h>
#	include <netinet/in.h>
#	include <arpa/inet.h>
#else
#endif

#include <x_logfile.hxx>

#include <xsocket/sock_core.hxx>

#include <xsocket/sock_server_accept_routine.hxx>
#include <xsocket/on_server_socket.hxx>


/*
 * if fail please release callbacks
 * if success DO-NOT release callbacks
 */
int xsocket::SockServerHelper::startServer (
	const xsocket::NetProtocol& bindto,
	xsocket::OnServerSocket * server_callback,
	xsocket::OnSession * session_callback)
{
	int ret;

#if defined(XSOCKET_LOGLEVEL)
	/* trace */ char dmsg[128];
	snprintf(dmsg, 127, "func: %s", __func__); dmsg[127] = '\0';
	xlog::AppendV2(dmsg, __FILE__, __LINE__, 0, XLOG_LEVEL_V);
#endif

	if (NULL == server_callback) {
		ret = -xsocket::error::NO_SERVER_CB;
#if 	!defined(NO_X_LOGFILE)
		xlog::AppendV2("null server callback", __FILE__, __LINE__, ret);
#endif
		return ret;
	}

	if (NULL == session_callback) {
		ret = -xsocket::error::NO_SERVER_CB;
#if 	!defined(NO_X_LOGFILE)
		xlog::AppendV2("null session callback", __FILE__, __LINE__, ret);
#endif
		return ret;
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
		ret = -xsocket::error::SOCK_INIT_FAIL;
#if 	!defined(NO_X_LOGFILE)
		xlog::AppendV2("xsocket::core::InitSocketEnvironment fail",
			__FILE__, __LINE__, ret);
#endif
		return -ret;
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET/*PF_INET*/;
	addr.sin_addr.s_addr = inet_addr(bindto.ip);
	addr.sin_port = htons(bindto.port);

	/* SOCK_STREAM -- here is TCP FIXME use all params */
	int sockfd = socket(AF_INET /* PF_INET */, SOCK_STREAM, 0);

	if (sockfd < 0) {
		ret = -errno;
#if 	!defined(NO_X_LOGFILE)
		xlog::AppendV2("socket fail", __FILE__, __LINE__, ret);
#endif
		return ret;/* create socket fail */
	}

	ret = bind(sockfd, (const sockaddr *)&addr, sizeof(sockaddr_in));

	if (ret < 0) {
		ret = -errno;
#if 	!defined(NO_X_LOGFILE)
		xlog::AppendV2("bind fail", __FILE__, __LINE__, ret);
#endif
		goto bind_fail;
	}

	ret = listen(sockfd, 10);

	if (ret < 0) {
		ret = -errno;
#if 	!defined(NO_X_LOGFILE)
		xlog::AppendV2("lsiten fail", __FILE__, __LINE__, ret);
#endif
		goto listen_fail;
	}

	/* set fd before start */
	server_callback->set_sockfd(sockfd);
	server_callback->set_on_session(session_callback);

	/* start accept thread */
	ret = server_callback->startAccept();

	if (0 != ret) {
		goto start_accept_fail;
	}

	return  0;/* success */

start_accept_fail:
listen_fail:
bind_fail:
	xsocket::core::ShutdownSocket(sockfd, xsocket::ShutdownHow::RDWR);
	xsocket::core::CloseSocket(sockfd);
	return ret;

}
