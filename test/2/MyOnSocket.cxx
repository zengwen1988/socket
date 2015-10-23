#include <SimpleTimer.h>
#include "GSSockHelper.h"

#include <c_logfile.h> /* log */

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

	char dmsg[256];

	snprintf(dmsg, 256, "code: %d sockfd: %d peer-ip: %s:%u", sc.code,
		sc.sockfd, sc.info.ip, sc.info.port);
	clogf_append_v2(dmsg, __FILE__, __LINE__, 0);

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

	char dmsg[256];

	snprintf(dmsg, 256,
		"code: %d sockfd: %d", fi.code, fi.sockfd);
	clogf_append_v2(dmsg, __FILE__, __LINE__, 0);

	SimpleTimerHelper::startTimer(10 * 1e3, __reconnect, NULL);

	return 0;

}


/* override */
int MyOnSocket::onReceived (int sockfd, const uint8_t *, size_t sz)
{

	char dmsg[256];

	snprintf(dmsg, 256,
		"recv: %zu from sockfd: %d", sz, sockfd);
	clogf_append_v2(dmsg, __FILE__, __LINE__, 0);

	return 0;

}

/* override */
bool MyOnSocket::shouldTeminateRecv (int)
{
	return false;
}
