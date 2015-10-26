#include <xsocket/sock_server_helper.hxx>

#include <stdio.h>
#include <posix/func/usleep.h>
#include <posix/func/bzero.h>
#include <timestamp.h>
#include <c_logfile.h>

#if defined(WIN32)
#	include <posix/func.hxx>
#endif

class MyXOnServerSocket: public xsocket::OnServerSocket {
/* override this */
public: virtual int onConnected(int, int, const xsocket::NetProtocol&)
{
	return 0;
}

/* override this */
public: virtual int willFinish(xsocket::SockWillFinish) {
	return 0;
}

/* override this */
public: virtual int onReceived(xsocket::SockRecved) {
	return 0;
}

/* override this */
public: virtual bool shouldTeminate(int) {
	return false;
}
};

int main (void)
{

	char lof[1024];
	bzero(lof, 1024);

	timestampname_d(lof, "gsserverr.", ".log");
	clogf_update(lof);

	clogf_append_v2(__func__, __FILE__, __LINE__, 0);

	MyXOnServerSocket ss;

	xsocket::NetProtocol bindto;
	strcpy(bindto.ip, "0.0.0.0");
	bindto.port = 12345;
	bindto.is_ipv6 = false;
	int ret = xsocket::SockServerHelper::startServer(bindto, &ss);

	clogf_append_v2("XSockServerHelper::startServer", __FILE__, __LINE__,
		ret);

	usleep(1000 * 1e6);

	return 0;

}
