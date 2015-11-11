#if !defined (XSOCKET_ON_SERVER_SOCKET_HXX__)
#define XSOCKET_ON_SERVER_SOCKET_HXX__ (1)

#include <cstring>

#include <xsocket/basic_sock_type.hxx> /* basic type */

/* class xsocket::core::internal::SockServerAcceptRoutine */
namespace xsocket { namespace core { namespace internal {
	class SockServerAcceptRoutine; } } }

/* class xsocket::SockServerHelper */
namespace xsocket { class SockServerHelper; }

/* class xsocket::OnSession */
namespace xsocket { class OnSession; }

namespace xsocket {

/*
 * NAME class xsocket::OnServerSocket
 *   server callback
 */
class OnServerSocket {

friend class xsocket::core::internal::SockServerAcceptRoutine;
friend class xsocket::SockServerHelper;/* set_sockfd */

public:
	OnServerSocket(void);

/*
 * 1 will be delete by callback (SockServerAcceptRoutine)
 * when accept started success (when finish)
 * 2 sub-class
 */
protected:
	virtual ~OnServerSocket();

/*
 * NAME
 *   onConnected - to override this callback
 *
 * NOTE-XXX
 *   - Should not block (this callback not in a thread)
 *   - TODO: callback in a thread
 */
/* override */
public:
	virtual int onConnected(int svr_fd, int cli_fd,
		const xsocket::NetProtocol& cli_prot) = 0;
	virtual int didFinish(SockDidFinish) = 0;
	virtual bool shouldTeminate(int sockfd) = 0;

public:
	inline int sockfd (void) { return this->_sockfd; }
	inline xsocket::OnSession * on_session (void) { return this->_on_session; }

protected:
	inline void set_sockfd (int fd) { this->_sockfd = fd; }
	inline void set_on_session (xsocket::OnSession * os) {
		this->_on_session = os; }
	inline xsocket::NetProtocol server_prot (void) {
		return this->_server_prot;
	}
	inline void set_server_prot (const xsocket::NetProtocol& server_prot) {
		memcpy(&(this->_server_prot), &(server_prot),
			sizeof(xsocket::NetProtocol));
	}

/* NOTE BETTER let xsocket::SockServerHelper start accept */
protected:
	int startAccept(void);

private:
	bool acceptPrepare(void);

/*
 * bind accept_rounine to here:
 * 1 new when OnServerSocket new
 * 2 delete when OnServerSocket delete
 */
private:
	xsocket::core::internal::SockServerAcceptRoutine * _accept_routine;
	int _sockfd;
	xsocket::OnSession * _on_session;

private:
	xsocket::NetProtocol _server_prot;
	static int instance_num;
};

} /* namespace xsocket */

#endif /* XSOCKET_ON_SERVER_SOCKET_HXX__ */
