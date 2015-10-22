#include <Sock.hxx>
#include <SockServerHelper.hxx>

#include <stdio.h>
#include <posix/func/usleep.h>

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

	int ret = XSockServerHelper::startServer("0.0.0.0", 12345, &ss);

	printf("ret: %d", ret);

	usleep(1000 * 1e6);

	return 0;

}
