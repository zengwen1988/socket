#include <unistd.h>
#include <arpa/inet.h>

#include <x_logfile.hxx>
#include <random.h>

#include <xsocket/basic_sock_type.hxx>
#include <xsocket/sock_core.hxx>

#include "my_on_socket.hxx"
#include "gs_sock_helper.hxx"


int main (int, char * [])
{

	static uint32_t buf[1024];
	int i;
	int fd;
	int ret;
	uint32_t rd;

	xlog::Update("cli2.log");

	ret = GSSockHelper::connectToServer();

	while (xsocket::status::CONNECTING
		== GSSockHelper::sock_status()) {
		usleep(1 * 1e6);
	}

	if (xsocket::status::CONNECTED == GSSockHelper::sock_status()) {
		fd = GSSockHelper::on_sock()->sockfd();

		xlog::AppendV2("main", __FILE__, __LINE__, 0);

		while (true) {
			for (i = 0; i < 1024; ++i) {
				rd = random_next(NULL);
				buf[i] = rd;
			}

			ret = xsocket::core::SendData(fd, (const uint8_t *)buf, 0, 4096);
			xlog::AppendV2("send_data", __FILE__, __LINE__, ret);

			if (ret < 0) {
				xlog::AppendV2("send_data fail", __FILE__, __LINE__, ret);
				break;
			}
			usleep(5 * 1e6);
		}
	} else {
		xlog::AppendV2("connect fail", __FILE__, __LINE__, -1);
	}

	return 0;
}
