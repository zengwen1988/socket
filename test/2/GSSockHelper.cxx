#include <unix/sock.h>
#include <c_log.h>

#include <unix/SocketClientHelper.h>

#include "GSSockHelper.h"


int GSSockHelper::connectToServer (void)
{

	show_trace();

	GSSockHelper::disconnect();

	GSSockHelper::sock_status = SOCK_STATUS_CONNECTING;

	GSSockHelper::os = new MyOnSocket();
	/*
	UNIXSockStartConnParams * ps = new UNIXSockStartConnParams(
		ntohl(inet_addr("183.237.232.202")), 60005, 15, os);
	*/
	/* UNIXSocketClientHelper::startConnectToServer(ps); */
	// "192.168.0.113", 5577, 15, os);
	// "183.237.232.202", 60005, 15, os);
	// "192.168.0.112", 5577, 15, os);
	GSSockHelper::ps
		= new UNIXSockStartConnParamsD("192.168.0.112", 5577,
		15, GSSockHelper::os);

	int ret = UNIXSocketClientHelper::startConnectByDomain(
		GSSockHelper::ps);
	if (0 != ret) {
		GSSockHelper::sock_status = SOCK_STATUS_UNKNOWN;
	}

	return ret;

}


int GSSockHelper::disconnect ()
{

	int ret = 0;

	show_trace();

	GSSockHelper::sock_status = SOCK_STATUS_DISCONNECTING;

	if (NULL != GSSockHelper::os) {
		int fd = GSSockHelper::os->getSockfd();
		if (fd >= 0) {
			ret = close_socket(fd);
		}

		delete GSSockHelper::os;
		GSSockHelper::os = NULL;
	}

	if (NULL != GSSockHelper::ps) {
		delete GSSockHelper::ps;
		GSSockHelper::ps = NULL;
	}

	return ret;

}


UNIXSockStartConnParamsD * GSSockHelper::ps = NULL;
MyOnSocket * GSSockHelper::os = NULL;
socket_status_t GSSockHelper::sock_status = SOCK_STATUS_UNKNOWN;
