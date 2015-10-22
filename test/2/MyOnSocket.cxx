#include <SimpleTimer.h>
#include "GSSockHelper.h"

#include <c_log.h> /* log */

#include "MyOnSocket.h"


MyOnSocket::MyOnSocket ()
{
	this->sockfd = -1;
}


static int __reconnect (uint64_t, int, void *)
{
	GSSockHelper::connectToServer();
	return 0;
}

/* override */
int MyOnSocket::onConnect (sock_on_conn_t sc)
{
	log2stdout("code: %d sockfd: %d peer-ip: %s:%u", sc.code,
	sc.sockfd, sc.info.ip, sc.info.port);

	if (sc.code >= 0) {
		this->sockfd = sc.sockfd;
	} else {
		SimpleTimerHelper::startTimer(30 * 1e3, __reconnect, NULL);
	}

	return 0;
}



/* override */
int MyOnSocket::willFinish (sock_will_finish_t fi)
{
	log2stdout("code: %d sockfd: %d", fi.code, fi.sockfd);

	SimpleTimerHelper::startTimer(10 * 1e3, __reconnect, NULL);

	return 0;
}


/* override */
int MyOnSocket::onReceived (int sockfd, const uint8_t *, size_t sz)
{
	log2stdout("recv: %zu from sockfd: %d", sz, sockfd);

	return 0;
}

/* override */
bool MyOnSocket::shouldTeminateRecv (int)
{
	return false;
}
