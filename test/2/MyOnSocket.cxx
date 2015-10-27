#include <xsimple_timer.hxx>
#include "gs_sock_helper.hxx"

#include <x_logfile.hxx> /* log */

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
	xlog::AppendV2(dmsg, __FILE__, __LINE__, 0);

	if (sc.code >= 0) {
		this->set_sockfd(sc.sockfd);
	} else {
		xtimer::SimpleTimerHelper::startTimer(30 * 1e3, __reconnect, NULL);
	}

	return 0;

}



/* override */
int MyOnSocket::willFinish(const xsocket::SockWillFinish& fi)
{

	char dmsg[256];

	snprintf(dmsg, 255,
		"code: %d sockfd: %d", fi.code, fi.sockfd);
	dmsg[255] = '\0';
	xlog::AppendV2(dmsg, __FILE__, __LINE__, 0);

	xtimer::SimpleTimerHelper::startTimer(10 * 1e3, __reconnect, NULL);

	return 0;

}


/* override */
int MyOnSocket::onReceived(const xsocket::SockRecved rcv)
{

	char dmsg[256];

	snprintf(dmsg, 255,
		"recv: %zu from sockfd: %d", rcv.count, rcv.fd_receiver);
	dmsg[255] = '\0';
	xlog::AppendV2(dmsg, __FILE__, __LINE__, 0);

	return 0;

}

/* override */
bool MyOnSocket::shouldTeminateRecv (int)
{
	return false;
}
