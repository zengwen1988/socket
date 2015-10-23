#include <sock.h>

#include <SockParams.hxx>
#include <SockServerAcceptRoutine.hxx>
#include <SockServerCliSessionRoutine.hxx>
#include <SockServer.hxx>

#include <c_logfile.h>
#include <posix/func/snprintf.h>

#include <stdio.h>

void * XSockServerAcceptRoutine::run (void * server) {

	XSockServer * ss = (XSockServer *)server;
	char dmsg[256];

	if (NULL == ss) {
		printf("No server");
		return (void *)-1;
	}

	XOnServerSocket * serverCallback
		= (XOnServerSocket *)(ss->getServerCallback());
	bool teminate;
	int serverfd = ss->getSockfd();
	int ret;
	int ec = 0;
	struct sockaddr_in addr;
	char * p;
	net_protocol_t clientProt;

	XSockServerCliSessionParams cliSessParams;

	while (! (teminate = serverCallback->shouldTeminate(serverfd))) {

		memset(&addr, 0, sizeof(struct sockaddr_in));
		ret = wait_connect(serverfd, &addr);

		if (ret >= 0) {

			/* client connect to me */
			memset(&clientProt, 0, sizeof(net_protocol_t));
			p = inet_ntoa(addr.sin_addr);
			strncpy(clientProt.ip, p, SOCK_MAX_IP_LEN + 1);
			clientProt.port = ntohs(addr.sin_port);

			snprintf(dmsg, 255,
				"server[%d]. got connection: [%d] %s:%u",
				serverfd, ret, clientProt.ip, clientProt.port);
			dmsg[255] = 0;
			clogf_append_v2(dmsg, __FILE__, __LINE__, 0);

			serverCallback->onConnected(serverfd, ret, clientProt);

			/*
			 * TODO start recv thread
			 */
			XSockServerCliSessionRoutine * clisess
				= new XSockServerCliSessionRoutine();


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

	shutdown_socket(serverfd, SDH_SHUT_RDWR);
	close_socket(serverfd);

	sock_will_finish_t fi;
	fi.code = ec;
	fi.sockfd = serverfd;/* server will finish */
	/* TODO: fill info */

	serverCallback->willFinish(fi);

	delete this;

	return (void *)ec;

}
