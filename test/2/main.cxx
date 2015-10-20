#include <unix/OnSocket.h>
#include <unix/SocketClientHelper.h>

#include <unistd.h>
#include <arpa/inet.h>

#include <c_log.h>
#include <random.h>

#include "MyOnSocket.h"
#include "GSSockHelper.h"


int main (int, char * [])
{

	static uint32_t buf[1024];
	int i;
	int fd;
	int ret;
	uint32_t rd;


	ret = GSSockHelper::connectToServer();

	show_trace();

	while (SOCK_STATUS_CONNECTING
		== GSSockHelper::getSockStatus()) {
		usleep(1 * 1e6);
	}

	fd = GSSockHelper::getSock()->getSockfd();

	show_trace();
	while (true) {
		show_trace();
		for (i = 0; i < 1024; ++i) {
			rd = random_next(NULL);
			buf[i] = rd;
		}
		ret = send_data(fd, (const uint8_t *)buf, 0, 4096);
		log2stdout("send_data: ret: %d", ret);
		if (ret < 0) {
			break;
		}
		usleep(5 * 1e6);
	}

	return 0;
}
