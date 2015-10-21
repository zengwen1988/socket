#if !defined(XWINSOCK_H__)
#define XWINSOCK_H__ (1)

#include <stdint.h>
#include <sock.h>

#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h> /* sockaddr_in */

#define XSOCK_MAX_IP_LEN (15)


class XSock
{

protected: XSock(void) {
	this->sockfd = -1;
}

public: ~XSock() {
	shutdown_socket(this->sockfd, SDH_SHUT_RDWR);
	close_socket(this->sockfd);
}

public: int SendData(const uint8_t * data, size_t count);
public: int CloseSocket(int how);
public: const char * getIP (void) const { return this->IP; }
public: uint16_t getPort (void) const { return this->port; }

protected: char IP [XSOCK_MAX_IP_LEN + 1];
protected: uint16_t port;

protected: int sockfd;
protected: sockaddr_in localAddress;

};


#endif /* XWINSOCK_H__ */