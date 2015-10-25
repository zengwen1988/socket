#include <SimpleTimer.h>
#include "gs_sock_helper.hxx"

#include <c_logfile.h> /* log */

#include "my_on_socket.hxx"


MyOnSocket::MyOnSocket ()
{
	this->_sockfd = -1;
}


static int __reconnect (uint64_t, int, void *)
{
	GSSockHelper::connectToServer();
	return 0;
}

/* override */
int MyOnSocket::onConnect(const xsocket::SockOnConnnect& sc)
{

	char dmsg[256];

	snprintf(dmsg, 256, "code: %d sockfd: %d peer-ip: %s:%u", sc.code,
		sc.sockfd, sc.info.ip, sc.info.port);
	clogf_append_v2(dmsg, __FILE__, __LINE__, 0);

	if (sc.code >= 0) {
		this->set_sockfd(sc.sockfd);
	} else {
		SimpleTimerHelper::startTimer(30 * 1e3, __reconnect, NULL);
	}

	return 0;

}



/* override */
int MyOnSocket::willFinish(const xsocket::SockWillFinish& fi)
{

	char dmsg[256];

	snprintf(dmsg, 256,
		"code: %d sockfd: %d", fi.code, fi.sockfd);
	clogf_append_v2(dmsg, __FILE__, __LINE__, 0);

	SimpleTimerHelper::startTimer(10 * 1e3, __reconnect, NULL);

	return 0;

}


/* override */
int MyOnSocket::onReceived(const xsocket::SockRecved rcv)
{

	char dmsg[256];

	snprintf(dmsg, 256,
		"recv: %zu from sockfd: %d", rcv.count, rcv.fd_receiver);
	clogf_append_v2(dmsg, __FILE__, __LINE__, 0);

	return 0;

}

/* override */
bool MyOnSocket::shouldTeminateRecv (int)
{
	return false;
}
