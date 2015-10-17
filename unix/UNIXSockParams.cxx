#include <unix/SockParams.h>

#include <c_log.h>



UNIXSockConnParams::UNIXSockConnParams (uint32_t ip,
	uint16_t port,
	uint16_t timeout,
	UNIXOnSocket * onSocket) {

	this->ip = ip;
	this->port = port;
	this->timeout = timeout;
	this->onSocket = onSocket;

}


uint32_t UNIXSockConnParams::getIP () const {
	return this->ip;
}


uint16_t UNIXSockConnParams::getPort () const {
	return this->port;
}


uint16_t UNIXSockConnParams::getTimeout () const {
	return this->timeout;
}


UNIXOnSocket * UNIXSockConnParams::getOnSocket () const {
	return  this->onSocket;
}
