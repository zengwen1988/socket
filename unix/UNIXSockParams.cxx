#include <unix/SockParams.h>

#include <c_log.h>

UNIXSockConnParams::UNIXSockConnParams (uint32_t ip,
	uint16_t port,
	uint16_t timeout,
	int(*onConnect)(unix_sock_on_conn_params_t))
{

	this->ip = ip;
	this->port = port;
	this->timeout = timeout;
	this->onConnect = onConnect;

}
