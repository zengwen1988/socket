#if !defined(XWINSOCKSERVER_HXX__)
#define XWINSOCKSERVER_HXX__ (1)

#include <xwin/xsock.hxx>
class XSockServerhelper;

class XSockServer: public XSock
{

public: XSockServer (int sockfd, sockaddr_in localAddress) {

	XSock::sockfd = sockfd;
	memcpy(&(this->XSock::localAddress), &localAddress, sizeof(sockaddr_in));

}

};

#endif /* XWINSOCKSERVER_HXX__ */