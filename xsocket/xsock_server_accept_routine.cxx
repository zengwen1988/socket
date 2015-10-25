#include <xsocket/sock_server_accept_routine.hxx>

/* inet_ntoa */
#if !defined(WIN32)
#	include <sys/socket.h>
#	include <netinet/in.h>
#	include <arpa/inet.h>
#else
// #	include <WinSock2.h>
#endif

#include <posix/func/snprintf.h>

#if !defined(NO_C_LOGFILE)
#	include <c_logfile.h>
#endif

#include <xsocket/sock_core.hxx>
#include <xsocket/sock_server_core.hxx>
#include <xsocket/sock_server.hxx>

// #include <xsocket/sock_serverCliSessionRoutine.hxx>

// #include <SockParams.hxx>

void * xsocket::core::internal::SockServerAcceptRoutine::run (void * server)
{

	xsocket::SockServer * ss = (xsocket::SockServer *)server;
	char dmsg[256];

	if (NULL == ss) {
		printf("No server");
		return (void *)-1;
	}

	xsocket::OnServerSocket * server_callback = ss->server_callback();
	bool teminate;
	int serverfd = ss->sockfd();
	int ret;
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
			clogf_append_v2(dmsg, __FILE__, __LINE__, 0);

			server_callback->onConnected(serverfd, ret, clientProt);

			/*
			 * TODO start recv thread
			 */
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

	delete ss;
	delete this;/* FIXME: bind to ss */

	return (void *)ec;
}
