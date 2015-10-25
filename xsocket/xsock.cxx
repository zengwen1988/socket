#include <xsocket/sock.hxx>
#include <xsocket/basic_sock_type.hxx>
#include <xsocket/sock_core.hxx>

#include <cstring> /* memset */

xsocket::Sock::Sock (void)
{
	this->_sockfd = -1;
	memset(&(this->_local_address), 0, sizeof(sockaddr_in));
}


xsocket::Sock::~Sock () {
	this->relaseSocket();
}


void xsocket::Sock::relaseSocket (void)
{
	if (-1 != this->sockfd()) {
		xsocket::core::ShutdownSocket(this->sockfd(),
			xsocket::ShutdownHow::RDWR);
		xsocket::core::CloseSocket(this->sockfd());
		this->set_sockfd(-1);
	}
}
