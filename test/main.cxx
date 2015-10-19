#include <unix/OnSocket.h>
#include <unix/SocketClientHelper.h>

#include <unistd.h>
#include <arpa/inet.h>

#include <c_log.h>



class MyOnSocket: public UNIXOnSocket
{

/* override */
virtual int onConnect (sock_on_conn_t sc)
{
	log2stream(stdout, "code: %d", sc.code);

	return 0;
}


/* override */
virtual int willFinish (sock_will_finish_t fi)
{
	log2stream(stdout, "code: %d", fi.code);

	return 0;
}

/* override */
virtual int onReceived (const uint8_t *, size_t)
{
	return 0;
}

/* override */
virtual bool shouldTeminateRecv(int sockfd)
{
	return false;
}


};


int main (int argc, char * argv[])
{

	MyOnSocket * os = new MyOnSocket();

	/*
	UNIXSockStartConnParams * ps = new UNIXSockStartConnParams(
		ntohl(inet_addr("183.237.232.202")), 60005, 15, os);
	*/
	UNIXSockStartConnParams * ps = new UNIXSockStartConnParams(
		0, 60005, 15, os);
	UNIXSocketClientHelper::startConnectByDomain("183.237.232.202", 60005,
		ps);
	/* UNIXSocketClientHelper::startConnectToServer(ps); */

	while (true) {
		usleep(5 * 1e6);
	}

	return 0;
}

