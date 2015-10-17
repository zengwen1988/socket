#include <unix/SocketClientUtil.h>

#include <fcntl.h> /* F_GETFL .. */

#include <c_log.h> /* log2stream */


int UNIXSocketClientUtil::setSocketBlock(int sockfd, bool block)
{

	int ret;
	int flags;

	flags = fcntl(sockfd, F_GETFL, 0);
	if (flags < 0) {
		ret = -errno;

		log2stream(stdout, "fcntl fail");

		return ret;
	}

	if (!block) {
		ret = fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
	} else {
		flags &= ~ O_NONBLOCK;
		ret = fcntl(sockfd, F_SETFL, flags);
	}

	if (ret < 0) {
		ret = -errno;

		log2stream(stdout, "fcntl fail");

		return ret;
	} else {
		return 0;
	}

}
