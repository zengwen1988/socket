#if !defined(SOCKSERVER_HXX__)
#define SOCKSERVER_HXX__ (1)

#include <Sock.hxx> /* XSock */
#include <OnServerSocket.hxx> /* XOnServerSocket */
#include <SockServerAcceptRoutine.hxx> /* XOnServerSocket */
#include <sock.h>


class XSockServer: public XSock
{

public: XSockServer(int sockfd, sockaddr_in localAddress,
	const XOnServerSocket * serverCallback);

public: const XOnServerSocket * getServerCallback (void) const {
	return this->serverCallback;
}

private: const XOnServerSocket * serverCallback;

};

#endif /* SOCKSERVER_HXX__ */
