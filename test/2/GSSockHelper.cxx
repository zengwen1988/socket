#include <cstring>

#include <xsocket/sock_core.hxx>

#include <c_logfile.h>

#include <xsocket/sock_client_helper.hxx>

#include "gs_sock_helper.hxx"


int GSSockHelper::connectToServer (void)
{

	clogf_append_v2("GSSockHelper::connectToServer", __FILE__, __LINE__, 0);

	GSSockHelper::disconnect();

	GSSockHelper::set_sock_status(xsocket::status::CONNECTING);

	GSSockHelper::os = new MyOnSocket();
	/*
	UNIXSockStartConnParams * ps = new UNIXSockStartConnParams(
		ntohl(inet_addr("183.237.232.202")), 60005, 15, os);
	*/
	/* UNIXSocketClientHelper::startConnectToServer(ps); */
	// "192.168.0.113", 5577, 15, os);
	// "183.237.232.202", 60005, 15, os);
	// "192.168.0.112", 5577, 15, os);
	// new XSockStartConnParamsD("192.168.0.112", 5577,
	// new XSockStartConnParamsD("183.237.232.202", 60005,
	/*
	GSSockHelper::ps =
		new XSockStartConnParamsD("192.168.0.119", 12345,
		15, GSSockHelper::os);
	*/

	xsocket::NetProtocol host;
	// strcpy(host.ip, "127.0.0.1");
	strcpy(host.ip, "192.168.0.119");
	host.is_ipv6 = false;
	host.port = 12345;
	int ret = xsocket::SockClientHelper::startConnectByHost(host, 15, os);
	if (0 != ret) {
		clogf_append_v2("startConnectByHost fail", __FILE__, __LINE__, ret);
		GSSockHelper::set_sock_status(xsocket::status::UNKNOWN);
	}

	return ret;

}


int GSSockHelper::disconnect ()
{

	int ret = 0;

	clogf_append_v2("GSSockHelper::disconnect", __FILE__, __LINE__, 0);

	GSSockHelper::set_sock_status(xsocket::status::DISCONNECTING);

	if (NULL != GSSockHelper::os) {
		int fd = GSSockHelper::os->sockfd();
		if (fd >= 0) {
			xsocket::core::ShutdownSocket(fd, xsocket::ShutdownHow::RDWR);
			ret = xsocket::core::CloseSocket(fd);
		}

		delete GSSockHelper::os;
		GSSockHelper::os = NULL;
	}

	return ret;

}


MyOnSocket * GSSockHelper::os = NULL;
xsocket::status::Type GSSockHelper::_sock_status = xsocket::status::UNKNOWN;
