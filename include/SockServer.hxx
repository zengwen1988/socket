#if !defined(SOCKSERVER_HXX__)
#define SOCKSERVER_HXX__ (1)

#include <Sock.hxx> /* XSock */
#include <OnServerSocket.hxx> /* XSock */
#include <sock.h>
#include <posix/thread/XSimpleThread.hxx> /* PosixXSimpleThread */

#include <stdio.h>

class XSockServerhelper;
class XSockServerAcceptRountine;

class XSockServer: public XSock
{

friend class XSockServerhelper;

public: XSockServer (int sockfd, sockaddr_in localAddress,
	const XOnServerSocket * serverCallback);

protected: XSockServerAcceptRountine * acceptRoutine;

protected: const XOnServerSocket * serverCallback;

};


class XSockServerAcceptRountine: public PosixXSimpleThread
{

public: XSockServerAcceptRountine (void * arg) 
	: PosixXSimpleThread (arg) {
}

protected: virtual void * run (void * server) {
	printf("HAHAHA!!!");
	return (void *)222;
}

};

#endif /* SOCKSERVER_HXX__ */
