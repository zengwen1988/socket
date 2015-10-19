#include <unix/SockParams.h>

#include <c_log.h>



UNIXSockStartConnParams::UNIXSockStartConnParams (uint32_t ip,
	uint16_t port,
	uint16_t timeout,
	UNIXOnSocket * onSocket) {

	this->ip = ip;
	this->port = port;
	this->timeout = timeout;
	this->onSocket = onSocket;

}


uint32_t UNIXSockStartConnParams::getIP () const {
	return this->ip;
}


uint16_t UNIXSockStartConnParams::getPort () const {
	return this->port;
}


uint16_t UNIXSockStartConnParams::getTimeout () const {
	return this->timeout;
}


UNIXOnSocket * UNIXSockStartConnParams::getOnSocket () const {
	return  this->onSocket;
}


UNIXSockConnParams::UNIXSockConnParams () {}


uint32_t UNIXSockConnParams::getIP () const {
	return this->ip;
}


uint16_t UNIXSockConnParams::getPort () const {
	return this->port;
}


UNIXOnSocket * UNIXSockConnParams::getOnSocket () const {
	return  this->onSocket;
}


int UNIXSockConnParams::getSockfd () const {
	return this->sockfd;
}


pthread_t UNIXSockConnParams::getTid () const {
	return this->tid;
}


uint16_t UNIXSockConnParams::getTimeout () const {
	return this->timeout;
}
