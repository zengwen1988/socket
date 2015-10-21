#if !defined(XSOCKSERVERHELPER_HXX__)
#define XSOCKSERVERHELPER_HXX__ (1)

#include <xwin/XSockServer.hxx>

#include <SimpleKV.hxx>


class XSockServerHelper
{


public: static int startServer(const char * bindIP, uint16_t bindPort);

public: static int closeSocket(int sockfd);

private: static SimpleKV<int, XSockServer *> * servers;


};

#endif /* !defined(XSOCKSERVERHELPER_HXX__) */