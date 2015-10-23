#include <Sock.hxx>
#include <SockServerHelper.hxx>

#include <stdio.h>
#include <posix/func/usleep.h>
#include <posix/func/bzero.h>
#include <timestamp.h>
#include <c_logfile.h>

class MyXOnServerSocket: public XOnServerSocket {
/* override this */
public: virtual int onConnected(int, int, net_protocol_t) {
	return 0;
}

/* override this */
public: virtual int willFinish(sock_will_finish_t) {
	return 0;
}

/* override this */
public: virtual int onReceived(sock_recved_t) {
	return 0;
}

/* override this */
public: virtual bool shouldTeminate(int sockfd) {
	return false;
}
};

int main (void)
{

	MyXOnServerSocket ss;

	char lof[1024];
	bzero(lof, 1024);

	timestampname_d(lof, "gsserverr.", ".log");
	clogf_update(lof);

	int ret = XSockServerHelper::startServer("0.0.0.0", 12345, &ss);

	clogf_append_v2("XSockServerHelper::startServer", __FILE__, __LINE__,
		ret);

	usleep(1000 * 1e6);

	return 0;

}
