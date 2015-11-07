#include <xsocket/sock_session_routine.hxx>

#include <cstring>

xsocket::core::internal::SockSessionRoutine::SockSessionRoutine (int cli_fd,
	const xsocket::NetProtocol& client,
	const xsocket::NetProtocol& server, OnSession * on_session)
{
	this->cli_fd = cli_fd;
	memcpy(&(this->client), &client, sizeof(xsocket::NetProtocol));
	memcpy(&(this->server), &server, sizeof(xsocket::NetProtocol));
}
