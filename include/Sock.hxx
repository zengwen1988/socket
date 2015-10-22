#if !defined(SOCK_HXX__)
#define SOCK_HXX__ (1)

#if !defined(_WIN32)
#	include <stdint.h>
#else
#	include "stdint.h"
#endif

#if !defined(_WIN32)
#	include <netinet/in.h> /* sockaddr_in */
#else
#	pragma comment(lib, "ws2_32.lib")
#	include <WinSock2.h> /* sockaddr_in */
#endif


class XSock
{

/*
 * NAME
 *   XSock - protected XSock
 *
 * DESC
 *   For sub-class only
 */
protected: XSock(void);

public: ~XSock();

protected: int sockfd;
protected: sockaddr_in localAddress;

};


#endif /* XSOCK_HXX__ */