#if !defined (SOCKCLIENTUTIL_HXX__)
#define SOCKCLIENTUTIL_HXX__ (1)

#include <sock.h>
#include <SockParams.hxx>


class XSockClientUtil {

public: static int startConnectByIP(uint32_t ip, uint16_t port);

public: static int startConnBySockfd(int sockfd,
	const XSockStartConnParams * ps);

public: static int startConnectByDomain(const XSockStartConnParamsD * ps);

protected: static void * connectByDomain(XSockConnParamsDomain * domain);

protected: static void * connectBySockfd(XSockConnParams * params);

protected: static int startReceiveFromPeer(XSockReceiveParams * params);

protected: static void * receiveRoutine(XSockReceiveParams * params);

};

#endif /* SOCKCLIENTUTIL_HXX__ */
