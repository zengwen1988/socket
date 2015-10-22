#include <Sock.hxx>

#include <sock.h> /* shutdown_socket .. */
#include <string.h> /* memset */


XSock::XSock (void)
{
	this->sockfd = -1;
	memset(&(this->localAddress), 0, sizeof(sockaddr_in));
}


XSock::~XSock () {
	this->relaseSocket();
}


void XSock::relaseSocket (void)
{
	if (-1 != this->sockfd) {
		shutdown_socket(this->sockfd, SDH_SHUT_RDWR);
		close_socket(this->sockfd);
		this->sockfd = -1;
	}
}

int XSock::getSockfd (void) const { return this->sockfd; }
sockaddr_in XSock::getLocalAddress (void) const { return this->localAddress; }
