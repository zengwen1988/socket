#include <xsocket/sock_server.hxx>

xsocket::SockServer::SockServer (int sockfd, const sockaddr_in& local_address,
	xsocket::OnServerSocket * server_callback)
{
	this->set_sockfd(sockfd);
	this->set_local_address(local_address);
	this->_server_callback = server_callback;
}
