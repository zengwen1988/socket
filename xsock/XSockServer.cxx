#include <SockServer.hxx>

XSockServer::XSockServer (int sockfd, sockaddr_in localAddress,
	const XOnServerSocket * serverCallback)
{

	XSock::sockfd = sockfd;
	memcpy(&(this->XSock::localAddress), &localAddress, sizeof(sockaddr_in));
	this->serverCallback = serverCallback;

}
