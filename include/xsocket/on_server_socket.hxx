#if !defined (XSOCKET_ON_SERVER_SOCKET_HXX__)
#define XSOCKET_ON_SERVER_SOCKET_HXX__ (1)

#include <xsocket/basic_sock_type.hxx>

namespace xsocket {

class OnServerSocket {

public: virtual ~OnServerSocket() {}

/*
 * NAME
 *   onConnected - to override this callback
 *
 * NOTE-XXX
 *   - Should not block (this callback not in a thread)
 *   - TODO: callback in a thread
 */
public: virtual int onConnected(int, int, const xsocket::NetProtocol&) = 0;

/* override this */
public: virtual int willFinish(SockWillFinish) = 0;

/* override this */
public: virtual int onReceived(SockRecved) = 0;

/* override this */
public: virtual bool shouldTeminate(int sockfd) = 0;

};

} /* namespace xsocket */

#endif /* XSOCKET_ON_SERVER_SOCKET_HXX__ */
