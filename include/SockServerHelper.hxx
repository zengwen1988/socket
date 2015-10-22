#if !defined(SOCKSERVERHELPER_HXX__)
#define SOCKSERVERHELPER_HXX__ (1)

#include <SockServer.hxx>
#include <OnServerSocket.hxx>

#include <SimpleKV.hxx>


class XSockServerHelper
{


public: static int startServer(const char * bindIP, uint16_t bindPort,
	const XOnServerSocket * serverCallback);

private: static SimpleKV<int, XSockServer *> * servers;


};

#endif /* !defined(SOCKSERVERHELPER_HXX__) */
