#include <sock.h>

#include <SockServerAcceptRoutine.hxx>
#include <SockServer.hxx>
#include <c_logfile.h>

#include <stdio.h>

void * XSockServerAcceptRoutine::run (void * server) {

	XSockServer * ss = (XSockServer *)server;

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

	while (! (teminate = serverCallback->shouldTeminate(serverfd))) {

		memset(&addr, 0, sizeof(struct sockaddr_in));
		ret = wait_connect(serverfd, &addr);

		if (ret >= 0) {

			/* client connect to me */
			memset(&clientProt, 0, sizeof(net_protocol_t));
			p = inet_ntoa(addr.sin_addr);
			strncpy(clientProt.ip, p, SOCK_MAX_IP_LEN + 1);
			clientProt.port = ntohs(addr.sin_port);
			printf("+%d: server[%d]. got connection: [%d] %s:%u\n", __LINE__,
				serverfd, ret, clientProt.ip, clientProt.port);

			serverCallback->onConnected(serverfd, ret, clientProt);

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
