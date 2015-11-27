#include <xsocket/sock_client_params.hxx>

#include <cstring> /* memcpy */

/*
 * NAME new xsocket::SockStartConnParams::SockStartConnParams
 *   - constructor
 *
 * NOTE-XXX
 *   - xsocket::OnClientSocket * ocs should NOT be NULL or start will fail
 *   - Pass the "ocs" in NOT need to delete any more
 */
xsocket::SockStartConnParams::SockStartConnParams (
	uint32_t timeout,
	const xsocket::NetProtocol& target,
	xsocket::OnClientSocket * ocs)
	: _timeout(timeout), _on_client_socket(ocs)
{
	memcpy(&(this->_target), &target, sizeof(xsocket::NetProtocol));
	this->_ip = NULL;
	this->_port = NULL;
}

/* For v0x00000001 only */
xsocket::SockStartConnParams::SockStartConnParams (
	const uint32_t timeout,
	xsocket::ip_t * ip,
	xsocket::port_t * port,
	xsocket::OnClientSocket * ocs)
	: _timeout(timeout), _on_client_socket(ocs)
{
	memset(&(this->_target), 0, sizeof(xsocket::NetProtocol));
	this->_ip = ip;
	this->_port = port;
}

/*
 * NAME new xsocket::SockConnParams::SockConnParams
 *   - constructor
 */
xsocket::internal::SockConnParams::SockConnParams (int sockfd, uint32_t timeout,
	const xsocket::NetProtocol& target,
		xsocket::OnClientSocket * on_client_sock)
	: _sockfd(sockfd), _timeout(timeout), _on_client_socket(on_client_sock)
{
	memcpy(&(this->_target), &target, sizeof(xsocket::NetProtocol));
}

xsocket::internal::SockConnParams::SockConnParams (
	const uint32_t timeout,
	xsocket::ip_t * ip,
	xsocket::port_t * port,
	xsocket::OnClientSocket * on_client_sock)
	: _sockfd(-1), _timeout(timeout), _on_client_socket(on_client_sock)
{
	memset(&(this->_target), 0, sizeof(xsocket::NetProtocol));
	this->_ip = ip;
	this->_port = port;
}

xsocket::SockClientRecviveParams::SockClientRecviveParams (int sockfd,
	xsocket::OnClientSocket * on_socket, const xsocket::NetProtocol& from)
{
	this->_sockfd = sockfd;
	this->_on_socket = on_socket;
	memcpy(&(this->_from), &from, sizeof(xsocket::NetProtocol));
}

#if 0
XSockStartConnParams::XSockStartConnParams (uint32_t ip,
	uint16_t port,
	uint16_t timeout,
	XOnClientSocket * onSocket) {

	this->ip = ip;
	this->port = port;
	this->timeout = timeout;
	this->onSocket = onSocket;

}


uint32_t XSockStartConnParams::getIP () const {
	return this->ip;
}


uint16_t XSockStartConnParams::getPort () const {
	return this->port;
}


uint16_t XSockStartConnParams::getTimeout () const {
	return this->timeout;
}


XOnClientSocket * XSockStartConnParams::getOnSocket () const {
	return  this->onSocket;
}


XSockConnParams::XSockConnParams () {}


uint32_t XSockConnParams::getIP () const {
	return this->ip;
}


uint16_t XSockConnParams::getPort () const {
	return this->port;
}


XOnClientSocket * XSockConnParams::getOnSocket () const {
	return  this->onSocket;
}


int XSockConnParams::getSockfd () const {
	return this->sockfd;
}


pthread_t XSockConnParams::getTid () const {
	return this->tid;
}


uint16_t XSockConnParams::getTimeout () const {
	return this->timeout;
}




XSockConnParamsDomain::XSockConnParamsDomain () {}

uint16_t XSockConnParamsDomain::getPort () const {
	return this->port;
}


XOnClientSocket * XSockConnParamsDomain::getOnSocket () const {
	return  this->onSocket;
}


pthread_t XSockConnParamsDomain::getTid () const {
	return this->tid;
}


uint16_t XSockConnParamsDomain::getTimeout () const {
	return this->timeout;
}


/*
 * ========================================================================
 * class XSockServerCliSessionParams
 * ========================================================================
 */
XSockServerCliSessionParams::XSockServerCliSessionParams (void)
{
	this->serverCallback = NULL;
	this->serverfd = -1;
	this->clifd = -1;
}


XSockServerCliSessionParams::XSockServerCliSessionParams (
	XSockServerCliSessionParams &ref)
{
	this->serverCallback = ref.serverCallback;
}


const XOnServerSocket *
XSockServerCliSessionParams::getServerCallback (void) const
{
	return this->serverCallback;
}


void XSockServerCliSessionParams::setServerCallback (
	const XOnServerSocket * serverCallback)
{
	this->serverCallback = serverCallback;
}
#endif
