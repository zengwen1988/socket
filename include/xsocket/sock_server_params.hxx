#if !defined (XSOCKET_SOCK_PARAMS_HXX__)
#define XSOCKET_SOCK_PARAMS_HXX__ (1)

#if 0
#include <stdint.h> /* int32_t .. */
#include <pthread.h> /* pthread_t */
#include <string.h>
#endif

#include <xsocket/sock_client_core.hxx>
#include <xsocket/on_client_socket.hxx>

/* NO INCLUDES IN THIS FILE */
namespace xsocket { namespace core { namespace internal { class ConnectToServerBySockfd; } } }
namespace xsocket { namespace core { namespace internal { class ConnectToServerBySockfd; } } }
namespace xsocket { namespace core { namespace internal { class ClientRecevier; } } }
namespace xsocket { namespace core { class ClientHelper; } }

/* THIS FILE */
namespace xsocket { class SockStartConnParams; }
namespace xsocket { class SockClientRecviveParams; }
namespace xsocket { namespace internal { class SockConnParams; } }

namespace xsocket {

/*
 * NAME class xsocket::SockStartConnParams
 *   - BETTER(public here) used by xsocket::core::ClientHelper only
 */
class SockStartConnParams {
public:
	SockStartConnParams(
		uint32_t timeout,
		const xsocket::NetProtocol& host,
		xsocket::OnClientSocket * ocs);

public:
	inline uint32_t timeout (void) const {
		return this->_timeout;
	}
	inline const xsocket::NetProtocol& target (void) const {
		return this->_target;
	}
	inline xsocket::OnClientSocket * on_client_socket (void) const {
		return this->_on_client_socket;
	}

private:
	uint32_t _timeout;/* second */
	xsocket::NetProtocol _target;
	xsocket::OnClientSocket * _on_client_socket;
}; /* class xsocket::SockStartConnParams */

} /* namespace xsocket */


/*
 * NAME class xsocket::internal::SockConnParams
 *  - For:
 *  xsocket::internal::SockClientCore(new and delete) and
 *  xsocket::core::internal::ClientConnectBySockfd(delete only)
 */
namespace xsocket { namespace internal {

class SockConnParams {
friend class xsocket::core::ClientHelper;
friend class xsocket::core::internal::ConnectToServerBySockfd;

protected:
	SockConnParams(int sockfd, uint32_t timeout,
		const xsocket::NetProtocol& target,
		xsocket::OnClientSocket * on_client_sock);

protected:
	inline uint32_t sockfd (void) { return this->_sockfd; }
	inline uint32_t timeout (void) { return this->_timeout; }
	inline xsocket::OnClientSocket * on_client_socket (void) {
		return this->_on_client_socket;
	}
	inline const xsocket::NetProtocol * target(void) {
		return &(this->_target);
	}
	inline xsocket::core::internal::ConnectToServerBySockfd&
		routine (void) { return this->_routine; }

private:
	int _sockfd;
	uint32_t _timeout;
	xsocket::OnClientSocket * _on_client_socket;
	xsocket::NetProtocol _target;
	xsocket::core::internal::ConnectToServerBySockfd _routine;
}; /* class xsocket::internal::SockConnParams */

 } /* namespace xsocket::internal */
} /* namespace xsocket */


/*
 * NAME class xsocket::SockClientRecviveParams
 *  - For xsocket::core::internal::ClientConnectBySockfd (new and delete only)
 */
namespace xsocket {

class SockClientRecviveParams {
friend class xsocket::core::internal::ConnectToServerBySockfd;
friend class xsocket::core::internal::ClientRecevier;
friend class xsocket::core::ClientHelper;

protected:
	SockClientRecviveParams(int sockfd, xsocket::OnClientSocket * on_socket,
		const xsocket::NetProtocol& from);
	inline uint32_t sockfd (void) { return this->_sockfd; }
	inline xsocket::OnClientSocket * on_socket (void) {
		return this->_on_socket;
	}
	inline const xsocket::NetProtocol * from (void) {
		return &(this->_from);
	}
	inline xsocket::core::internal::ClientRecevier&
		routine (void) { return this->_routine; }

private:
	int _sockfd;
	xsocket::OnClientSocket * _on_socket;
	xsocket::NetProtocol _from;
	xsocket::core::internal::ClientRecevier _routine;
};/* class xsocket::SockClientRecviveParams */

} /* namespace xsocket */

#if 0
/*
 * ==========================================================================
 * NAME
 *   XSockConnParamsDomain - class socket connect params when use domain
 *
 * DESC
 *   - For inner (XSocketClient and client helper) use only
 * ==========================================================================
 */
class XSockConnParamsDomain {

friend class  XSockClientUtil;

protected: XSockConnParamsDomain();

public: const char * getDomain() const;
public: uint16_t getPort() const;
public: uint16_t getTimeout() const;
public: XOnClientSocket * getOnSocket() const;
public: pthread_t getTid() const;

protected: int sockfd;
protected: pthread_t tid;

protected: uint16_t port;/* host order */
protected: uint16_t timeout;/* second */

protected: XOnClientSocket * onSocket;
protected: char domain[1024];

};


/*
 * ==========================================================================
 * NAME
 *   XSockServerCliSessionParams - class for XSockServerCliSessionRoutine
 * ==========================================================================
 */
class XSockServerCliSessionRoutine;
class XSockServerAcceptRoutine;
class XOnServerSocket;

/* XXX: this class NOT delete passed in serverCallback */
class XSockServerCliSessionParams {

/* can call new / delete */
friend class XSockServerCliSessionRoutine;
friend class XSockServerAcceptRoutine;

protected: XSockServerCliSessionParams(void);
protected: XSockServerCliSessionParams(XSockServerCliSessionParams &ref);
protected: ~XSockServerCliSessionParams();

public: const XOnServerSocket * getServerCallback (void) const;
protected: void setServerCallback(const XOnServerSocket * serverCallback);

private: const XOnServerSocket * serverCallback;
private: int serverfd;
private: int clifd;

};
#endif


#endif /* XSOCKET_SOCK_PARAMS_HXX__ */
