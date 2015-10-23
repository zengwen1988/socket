#include <unistd.h>
#include <arpa/inet.h>

#include <c_logfile.h>
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

	clogf_update("cli2.log");

	ret = GSSockHelper::connectToServer();

	while (SOCK_STATUS_CONNECTING
		== GSSockHelper::getSockStatus()) {
		usleep(1 * 1e6);
	}

	fd = GSSockHelper::getSock()->getSockfd();

	clogf_append_v2("main", __FILE__, __LINE__, 0);

	while (true) {
		for (i = 0; i < 1024; ++i) {
			rd = random_next(NULL);
			buf[i] = rd;
		}

		ret = send_data(fd, (const uint8_t *)buf, 0, 4096);
		clogf_append_v2("send_data", __FILE__, __LINE__, ret);

		if (ret < 0) {
			break;
		}
		usleep(5 * 1e6);
	}

	return 0;
}
