#include <SockServerHelper.hxx>
#include <OnServerSocket.hxx>

#include <stdio.h>
#include <errno.h>


static bool __sockEQU (int sockfd1, int sockfd2)
{
	return sockfd1 == sockfd2;
}


int XSockServerHelper::startServer (const char * bindIP, uint16_t bindPort,
	const XOnServerSocket * serverCallback)
{

	int ret;
	XSockServer * sv = NULL;
	XSockServerAcceptRountine * acc = NULL;

	if (NULL == bindIP) {
		return -1;/* invalid IP */
	}

#if	0
	WORD versionRequested;
    WSADATA dt;

    versionRequested = MAKEWORD(1,1);
	/* load winsocket dll */
    ret = WSAStartup(versionRequested, &dt);
#endif
	ret = init_socket_environment();

	if (0 != ret) {
		return -2;/* load winsocket dll fail */
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET/*PF_INET*/;
	addr.sin_addr.s_addr = inet_addr(bindIP);
	addr.sin_port = htons(bindPort);

	/* SOCK_STREAM -- here is TCP FIXME use all params */
	int sockfd = socket(AF_INET /* PF_INET */, SOCK_STREAM, 0);

	if (sockfd < 0) {
		fprintf(stderr, "socket: %d %d %d\n", sockfd, errno, ret);
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

	/* start accept thread */
	sv = new XSockServer(sockfd, addr, serverCallback);

	acc = new XSockServerAcceptRountine(sv);

	ret = acc->start();

	if (0 != ret) {
		ret = -6;
		goto start_accept_fail;
	}

	if (NULL == XSockServerHelper::servers) {
		XSockServerHelper::servers
			= SimpleKVMaker<int, XSockServer *>::make(__sockEQU);
	}

	kvo_t<int, XSockServer *> red
		= XSockServerHelper::servers->remove(sockfd);

	if (red.set) {
		delete red.value;
		red.value = NULL;
	}

	/* return sockfd; */
	return  0;

start_accept_fail:
	if (NULL != acc) {
		delete acc;
		acc = NULL;
	}

	if (NULL != sv) {
		delete sv;
		sv = NULL;
	}

listen_fail:

bind_fail:
	shutdown_socket(sockfd, SDH_SHUT_RDWR);
	close_socket(sockfd);
	return ret;

}


SimpleKV<int, XSockServer *> * XSockServerHelper::servers = NULL;
