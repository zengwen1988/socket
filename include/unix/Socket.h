#if !defined (UNIXSOCKET_H__)
#define UNIXSOCKET_H__ (1)

#include <arpa/inet.h>
#include <unix/sock.h>
#include <unix/SockParams.h>

class UNIXSocket {

public: static int startConnectByProtocol (uint32_t ip, uint16_t port)
{
	return get_sockfd_by_ipn(htonl(ip), htons(port));
}

public: static int startConnBySockfd(int sockfd,
	const UNIXSockStartConnParams * ps);

protected: static void * connectBySockfd(UNIXSockConnParams * params);

protected: static int startReceiveFromPeer(UNIXSockReceiveParams * params);

protected: static void * receiveRoutine(UNIXSockReceiveParams * params);

};

#endif /* UNIXSOCKET_H__ */
