#include <SockParams.hxx>


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
