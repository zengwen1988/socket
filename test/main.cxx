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
	log2stdout("code: %d sockfd: %d peer-ip: 0x%x:%u", sc.code,
		sc.sockfd, sc.info.peer_ip, sc.info.peer_port);

	return 0;
}


/* override */
virtual int willFinish (sock_will_finish_t fi)
{
	log2stdout("code: %d sockfd: %d", fi.code, fi.sockfd);

	return 0;
}

/* override */
virtual int onReceived (int sockfd, const uint8_t *, size_t sz)
{
	log2stdout("recv: %zu from sockfd: %d", sz, sockfd);

	return 0;
}

/* override */
virtual bool shouldTeminateRecv(int)
{
	return false;
}


};


int main (int, char * [])
{

	MyOnSocket * os = new MyOnSocket();

	/*
	UNIXSockStartConnParams * ps = new UNIXSockStartConnParams(
		ntohl(inet_addr("183.237.232.202")), 60005, 15, os);
	*/
	/* UNIXSocketClientHelper::startConnectToServer(ps); */
	UNIXSockStartConnParamsD * ps = new UNIXSockStartConnParamsD(
		"183.237.232.202", 60005, 15, os);
	UNIXSocketClientHelper::startConnectByDomain(ps);

	while (true) {
		usleep(5 * 1e6);
	}

	return 0;
}

