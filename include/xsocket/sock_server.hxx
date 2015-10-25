#if !defined(XSOCKET_SOCK_SERVER_HXX__)
#define XSOCKET_SOCK_SERVER_HXX__ (1)

#include <xsocket/sock.hxx> /* xsocket::Sock */
#include <xsocket/on_server_socket.hxx> /* xsocket::on_server_socket */
#include <xsocket/sock_server_accept_routine.hxx>

namespace xsocket {

class SockServer: public xsocket::Sock {

public: SockServer(int sockfd, const sockaddr_in& addr,
	xsocket::OnServerSocket * server_callback);

public: xsocket::OnServerSocket * server_callback (void) const {
	return this->_server_callback;
}

private: xsocket::OnServerSocket * _server_callback;

};

}

#endif /* XSOCKET_SOCK_SERVER_HXX__ */
