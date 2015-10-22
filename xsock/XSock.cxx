#include <Sock.hxx>

#include <sock.h> /* shutdown_socket .. */
#include <string.h> /* memset */


XSock::XSock (void)
{
	this->sockfd = -1;
	memset(&(this->localAddress), 0, sizeof(sockaddr_in));
}


XSock::~XSock () {
	shutdown_socket(this->sockfd, SDH_SHUT_RDWR);
	close_socket(this->sockfd);
}
