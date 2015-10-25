#if !defined(XSOCKET_SOCK_SERVER_HELPER_HXX__)
#define XSOCKET_SOCK_SERVER_HELPER_HXX__ (1)

#include <xsocket/on_server_socket.hxx>

namespace xsocket {

/*
 * TODO: support more than one server
 */
class SockServerHelper {

public: static int startServer(const xsocket::NetProtocol& bindto,
	xsocket::OnServerSocket * server_callback);

};

} /* namespace xsocket */

#endif /* !defined(XSOCKET_SOCK_SERVER_HELPER_HXX__) */
