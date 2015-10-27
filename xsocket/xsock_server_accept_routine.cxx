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

/* inet_ntoa */
#if !defined(WIN32)
#	include <sys/socket.h>
#	include <netinet/in.h>
#	include <arpa/inet.h>
#else
/* #	include <WinSock2.h> */
#endif

#include <posix/func/snprintf.h>

#if !defined(NO_X_LOGFILE)
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

			snprintf(dmsg, 255,
				"server[%d]. got connection: [%d] %s:%u",
				serverfd, ret, clientProt.ip, clientProt.port);
			dmsg[255] = 0;
			xlog::AppendV2(dmsg, __FILE__, __LINE__, 0);

			server_callback->onConnected(serverfd, ret, clientProt);

			/*
			 * TODO start recv thread
			 */
			session_callback->startSession();
			/*
			XSockServerCliSessionRoutine * clisess
				= new XSockServerCliSessionRoutine();
			*/


		} else {
			fprintf(stderr, "+%d: server[%d]. willFinish: %d\n",
				__LINE__, serverfd, ret);
			ec = ret;
		}

		/* usleep(5 * 1e6); */
	}

	if (teminate) {
		ec = 1;/* by teminate */
	}

	xsocket::core::ShutdownSocket(serverfd, xsocket::ShutdownHow::RDWR);
	xsocket::core::CloseSocket(serverfd);

	xsocket::SockWillFinish fi;
	fi.code = ec;
	fi.sockfd = serverfd;/* server will finish */
	/* TODO: fill info */

	server_callback->willFinish(fi);

	/*
	 * YES when delete server_callback
	 * -- self the routine will also be delete nice
	 */
	delete server_callback;
	server_callback = NULL;

	return (void *)ec;

} /* xsocket::core::internal::SockServerAcceptRoutine::run */


xsocket::core::internal::SockServerAcceptRoutine::SockServerAcceptRoutine
(void)
{
	++SockServerAcceptRoutine::instance_num;

#if !defined(NO_X_LOGFILE) && defined(ENABLE_SOCK_DEBUG)
	char dmsg[128];
	snprintf(dmsg, 127,
		"xsocket::core::internal::SockServerAcceptRoutine::SockServerAcceptRoutine: "
		"in: %zu", SockServerAcceptRoutine::instance_num);
	dmsg[127] = '\0';
	xlog::AppendV2(dmsg, __FILE__, __LINE__, 0);
#endif
}

xsocket::core::internal::SockServerAcceptRoutine::~SockServerAcceptRoutine ()
{
#if !defined(NO_X_LOGFILE) && defined(ENABLE_SOCK_DEBUG)
	int ret;
	long int retval;
	ret = this->nowFinish((void **)&retval);
#else
	(void)this->nowFinish();
#endif

#if !defined(NO_X_LOGFILE) && defined(ENABLE_SOCK_DEBUG)
	char dmsg[128];
	snprintf(dmsg, 127,
		"xsocket::core::internal::SockServerAcceptRoutine::~SockServerAcceptRoutine: "
		"in-will: %zu, ret: %d retval: %ld",
		SockServerAcceptRoutine::instance_num - 1, ret, retval);
	dmsg[127] = '\0';
	xlog::AppendV2(dmsg, __FILE__, __LINE__, 0);
#endif

	--SockServerAcceptRoutine::instance_num;
}

size_t xsocket::core::internal::SockServerAcceptRoutine::instance_num = 0;
