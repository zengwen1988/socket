/* ===========================================================================
 *
 * NAME
 *   xsocket/xsock_server_accept_routine.cxx
 *   Server accept routine class
 *
 * DESC
 *   TODO
 *
 * CREATED
 *   2015-10-26 AM +0800 by yuiwong
 *   E-mail: yuiwong@126.com
 *
 * SUPPORTED / TESTED PLATFORM
 *   TODO
 *
 * SUPPORTED / TESTED COMPILER
 *   TODO
 *
 * VERSION
 *   1.0.0.0 - TODO
 *
 * LICENSE
 *   LGPLv3
 *	 THIS FILE (is a part of XSOCKET)
 *   is free software:
 *	 you can redistribute it and/or modify it under the terms of the GNU
 *	 General Public License as published by the Free Software Foundation,
 *	 either version 3 of the License, or (at your option) any later version.
 *   THIS FILE
 *   is distributed in the hope that it will be
 *	 useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	 GNU General Public License for more details.
 *	 You should have received a copy of the GNU General Public License
 *	 along with
 *   THIS FILE.
 *	 If not, see <http://www.gnu.org/licenses/>.
 *
 * ===========================================================================
 */

#include <xsocket/sock_server_accept_routine.hxx>

#include <cstring>
#include <posix/func/usleep.h>

/* inet_ntoa */
#if !defined(WIN32)
#	include <sys/socket.h>
#	include <netinet/in.h>
#	include <arpa/inet.h>
#else
/* #	include <WinSock2.h> */
#endif

#if !defined(MINGW32)
#	include <posix/func/snprintf.h>
#endif

#if defined(XSOCKET_LOGLEVEL)
#	include <x_logfile.hxx>
#endif

#include <xsocket/sock_core.hxx>
#include <xsocket/sock_server_core.hxx>
#include <xsocket/on_server_socket.hxx>
#include <xsocket/on_session.hxx>

void * xsocket::core::internal::SockServerAcceptRoutine::run (void * sc)
{

	int ret;
	char dmsg[256];

#if !defined(NO_X_LOGFILE) && defined(ENABLE_SOCK_DEBUG)
	xlog::AppendV2("xsocket::core::internal::SockServerAcceptRoutine::run",
		__FILE__, __LINE__, 0);
#endif

	xsocket::OnServerSocket * server_callback = (xsocket::OnServerSocket *)sc;

	if (NULL == server_callback) {
		ret = -xsocket::error::NO_SERVER;
#if !defined(NO_X_LOGFILE) && defined(ENABLE_SOCK_DEBUG)
		xlog::AppendV2("ERROR: no server",
			__FILE__, __LINE__, ret);
#endif
		return (void *)ret;
	}

	xsocket::OnSession * session_callback = server_callback->on_session();
	/* no possible
	if (NULL == server_callback) {
	}
	*/

	bool teminate;
	int serverfd = server_callback->sockfd();
	int ec = 0;
	struct sockaddr_in addr;
	char * p;
	xsocket::NetProtocol serverProt = server_callback->server_prot();
	xsocket::NetProtocol clientProt;

	// XSockServerCliSessionParams cliSessParams;

	while (! (teminate = server_callback->shouldTeminate(serverfd))) {

		memset(&addr, 0, sizeof(struct sockaddr_in));
		ret = xsocket::core::WaitConnect(serverfd, &addr);

		if (ret >= 0) {

			/* client connect to me */
			memset(&clientProt, 0, sizeof(xsocket::NetProtocol));
			p = reinterpret_cast<char *>(inet_ntoa(addr.sin_addr));
			strncpy(clientProt.ip, p, XSOCKET_MAXIPLEN + 1);
			clientProt.port = ntohs(addr.sin_port);

#if			defined(XSOCKET_LOGLEVEL)
			snprintf(dmsg, 255,
				"serverfd[%d]. got connection: clientfd[%d] %s:%u",
				serverfd, ret, clientProt.ip, clientProt.port);
			dmsg[255] = 0;
			xlog::AppendV2(dmsg, __FILE__, __LINE__, 0, XLOG_LEVEL_I);
#endif

			server_callback->onConnected(serverfd, ret, clientProt);

			/*
			 * TODO start recv thread (for each client)
			 */
			session_callback->startSession(ret, clientProt, serverfd,
				serverProt);
		} else {
#if			defined(XSOCKET_LOGLEVEL)
			snprintf(dmsg, 255,
				"serverfd[%d]. will teminate: %s (%s:%u)",
				serverfd, strerror(-ret), clientProt.ip, clientProt.port);
			dmsg[255] = 0;
			xlog::AppendV2(dmsg, __FILE__, __LINE__, 0, XLOG_LEVEL_F);
#endif
			ec = ret;
			teminate = false;
			break;
		}

		/* usleep(5 * 1e6); */
	}

	if (teminate) {
		ec = 0;/* by teminate */
	}

	xsocket::core::ShutdownSocket(serverfd, xsocket::ShutdownHow::RDWR);
	xsocket::core::CloseSocket(serverfd);

	session_callback->notifyAllExit();
	usleep(100 * 1e3);
	session_callback->closeAllClis();
	usleep(100 * 1e3);

	xsocket::SockDidFinish fi;
	fi.code = ec;
	fi.fd = serverfd;/* server will finish */
	memcpy(&(fi.info), &(serverProt), sizeof(xsocket::NetProtocol));
	/* TODO: fill info */

	server_callback->didFinish(fi);

	/*
	 * YES when delete server_callback
	 * -- self the routine will also be delete nice
	 */
#if	defined(XSOCKET_LOGLEVEL)
	xlog::AppendV2("i'll exit and delete server_callback", __FILE__, __LINE__,
		0, XLOG_LEVEL_W);
#endif
	delete server_callback;
	server_callback = NULL;

	return (void *)ec;

} /* xsocket::core::internal::SockServerAcceptRoutine::run */


/* ONE RUN FOR EACH SERVER */
xsocket::core::internal::SockServerAcceptRoutine::SockServerAcceptRoutine
	(void)
{
#if	defined(XSOCKET_LOGLEVEL)
	xlog::AppendV2(__func__, __FILE__, __LINE__, 0, XLOG_LEVEL_T);
#endif
}

/* ONE RUN FOR EACH SERVER */
xsocket::core::internal::SockServerAcceptRoutine::~SockServerAcceptRoutine ()
{
#if	defined(XSOCKET_LOGLEVEL)
	xlog::AppendV2(__func__, __FILE__, __LINE__, 0, XLOG_LEVEL_T);
#endif
}
