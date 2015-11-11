#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <posix/func/usleep.h>
#include <posix/func/bzero.h>
#include <timestamp.h>

#include <string>
#include <list>
#include <map>

#include <posix/thread/simple_thread.hxx>
#include <x_logfile.hxx>

#if defined(WIN32)
#	include <posix/func.hxx>
#endif

#include <xsocket/basic_sock_type.hxx>
#include <xsocket/sock_core.hxx>
#include <xsocket/on_server_socket.hxx>
#include <xsocket/on_session.hxx>
#include <xsocket/sock_server_helper.hxx>

namespace ncsr {

class GetSendDataRoutine: public pthreadx::SimpleThread {
public:
	GetSendDataRoutine (void) {
		this->path = "";
		this->_byebye = true;
	}
	virtual ~GetSendDataRoutine() {}
	inline void set_protocol (const xsocket::NetProtocol& prot) {
		char cs[64];
		snprintf(cs, 63, "%s:%u", prot.ip, prot.port);
		cs[63] = '\0';
		this->prot_str = std::string(cs);
	}

public:
	GetSendDataRoutine (void * fd) : SimpleThread(fd) {
		int session_fd = static_cast<int>(reinterpret_cast<long int>(fd));

		if (session_fd > 0) {
			this->_byebye = false;

			char buf[512];
			snprintf(buf, 511, "mkdir -p /tmp/ncsr%05u/", PORT % 0xffff);
			buf[511] = '\0';
			int ret = system(buf);
			ret = ret;

			snprintf(buf, 511, "/tmp/ncsr%05u/%d", PORT % 0xffff, session_fd);
			buf[511] = '\0';
			this->path = std::string(buf);
		} else {
			this->_byebye = true;
		}
	}
	inline void byebye (void) { this->_byebye = true; }

protected:
	virtual void * run(void * sfd);

private:
	int get (const std::string& from, uint8_t * out, int max) {
		int fd, ret;
		off_t file_sz;

		if (NULL == out) {
			return -EINVAL;
		}

		if (max <= 0) {
			return 0;
		}

		const char * ff = from.c_str();

		fd = open(ff, O_RDONLY);

		if (fd < 0) {
			return -errno;
		}


		file_sz = lseek(fd, 0, SEEK_END);
		if ((off_t)-1 == file_sz) {
			ret = -1;
			goto _ret;
		}

		if (file_sz <= 0) {
			ret = -1;
			goto _ret;
		}

		if (max > file_sz) {
			max = file_sz;
		}

		file_sz = lseek(fd, 0, SEEK_SET);
		if ((off_t)-1 == file_sz) {
			goto _ret;
		}

		ret = read(fd, out, max);

	_ret:
		close(fd);
		(void)unlink(ff);
		return ret;
	}

private:
	bool _byebye;
	std::string path;
	std::string prot_str;
};

class MyXOnServerSocket: public xsocket::OnServerSocket {
/* override this */
public: virtual int onConnected(int /* svr_fd */, int cli_fd,
	const xsocket::NetProtocol& prot)
{
	char msg[128];
	snprintf(msg, 127, "ip: %s. port: %u. fd: %d.", prot.ip, prot.port,
		cli_fd);
	msg[127] = '\0';
	xlog::AppendV2(msg, __FILE__, __LINE__, 0, XLOG_LEVEL_0);

	if (cli_fd > 0) {
		GetSendDataRoutine * s = new GetSendDataRoutine(
			reinterpret_cast<void *>(static_cast<long int>(cli_fd)));
		s->set_protocol(prot);
		this->clis.insert(std::pair<int, GetSendDataRoutine *>(cli_fd, s));
		s->start();
	}
	return 0;
}

/* override this */
public: virtual int didFinish(xsocket::SockDidFinish cli) {
	int fd = cli.fd;
	if (fd > 0) {
		std::map<int, GetSendDataRoutine *>::iterator it
			= this->clis.find(fd);
		if (this->clis.end() != it) {
			this->clis.erase(it);

			if (NULL != it->second) {
				delete it->second;
				it->second = NULL;
			}
		}
	}

	return 0;
}

/* override this */
public: virtual bool shouldTeminate(int fd) {
	if (fd < 0) {
		return true;
	} else {
		return false;
	}
}

private:
	/* FIXME release */
	std::map<int, GetSendDataRoutine *> clis;
};

class MyXOnSession: public xsocket::OnSession {
/* override */
public:
	/* */
	virtual int didFinish (const xsocket::SockDidFinish&) {
		xlog::AppendV2(__func__, __FILE__, __LINE__, 0, XLOG_LEVEL_I);
		return 0;
	}
	virtual int onReceived (const xsocket::SockRecved& red) {
		xlog::AppendV2(__func__, __FILE__, __LINE__, red.count, XLOG_LEVEL_I);

		bool is_ascii = true;
		for (int i = 0; i < red.count; ++i) {
			if (0 == isascii(red.data[i])) {
				is_ascii = false;
				break;
			}
		}

		if (is_ascii) {
			xlog::AppendV2((char *)red.data, __FILE__, __LINE__, 0,
				XLOG_LEVEL_I);
		}

		/* xsocket::core::SendData(red.fd, red.data, 0, red.count); */
		return 0;
	}
	virtual bool shouldTeminate (int sockfd) {
		xlog::AppendV2(__func__, __FILE__, __LINE__, 0, XLOG_LEVEL_I);
		return sockfd > 0 ? false : true;
	}
};

}

int main (void)
{

	char * lof = new char[1024];
	bzero(lof, 1024);

	char prefix[16];
	snprintf(prefix, 11, "ncsr%05u.", PORT & 0xffff);
	prefix[10] = '\0';
	timestampname_d(lof, prefix, ".log");
	xlog::Update(lof);
	delete lof;
	lof = NULL;

	xlog::ToggleLogLevel();
	xlog::ToggleLogLevel();
	xlog::ToggleLogLevel();
	xlog::AppendV2(__func__, __FILE__, __LINE__, 0);

	ncsr::MyXOnServerSocket * ss = new ncsr::MyXOnServerSocket();
	ncsr::MyXOnSession * ssi = new ncsr::MyXOnSession();

	xsocket::NetProtocol bindto;
	strcpy(bindto.ip, "0.0.0.0");
	bindto.port = PORT;
	bindto.is_ipv6 = false;
	int ret = xsocket::SockServerHelper::startServer(bindto, ss, ssi);

	xlog::AppendV2("XSockServerHelper::startServer", __FILE__, __LINE__,
		ret);
	if (ret < 0) {
		delete ss;
		ss = NULL;
		delete ssi;
		ssi = NULL;
		goto end;
	}

	while (true) {
		xlog::AppendV2("alive", __FILE__, __LINE__, 0);
		usleep(120 * 1e6);
	}

end:
	return 0;

}

void * ncsr::GetSendDataRoutine::run (void *sfd)
{
	static const uint8_t bye[8] = {
		0xff, 0xff, 'b', 'y',
		'e', 0xff, 0xfe, 0xab
	};

	uint8_t * buf = new uint8_t[16 * 1024];
	int fd = (int)(long int)(sfd);
	int tosend;
	int retval = 0;
	uint32_t sf;
	std::string sfmsg;
	int i;
	bool byebye = true;
	bool eq;
	snprintf((char *)buf, 512, "send %s to: [%d] %s", this->path.c_str(), fd,
		this->prot_str.c_str());
	buf[512] = '\0';
	sfmsg = std::string((char *)buf);

	if (fd < 0) {
		xlog::AppendV2("bye", __FILE__, __LINE__, -1, XLOG_LEVEL_I);
		goto end;
	}

	sf = 0;
	do {
		byebye = this->_byebye;
		if (byebye) {
			xlog::AppendV2("bye", __FILE__, __LINE__, 1, XLOG_LEVEL_I);
			goto end;
		}

		tosend = this->get(this->path, buf, 16 * 1024);

		if (tosend > 0) {
			retval = write(fd, buf, tosend);
			xlog::AppendV2("wrote", __FILE__, __LINE__, retval,
				XLOG_LEVEL_I);

			if (8 == tosend) {
				eq = false;
				for (i = 0; i < 8; ++i) {
					if (buf[i] != bye[i]) {
						break;
					}
				}
				if (8 == i) {
					eq = true;
				}

				if (eq) {
					xlog::AppendV2("bye", __FILE__, __LINE__, 0, XLOG_LEVEL_I);
					goto end;
				}
			}

			if (retval != tosend) {
				xlog::AppendV2("bye", __FILE__, __LINE__, -2, XLOG_LEVEL_I);
				goto end;
			}
		}

		if (0 == (sf % 60)) {
			xlog::AppendV2(sfmsg.c_str(), __FILE__, __LINE__, 0,
			XLOG_LEVEL_0);
		}
		++sf;
		usleep(1000 * 1e3);
	} while (! byebye);

end:
	delete buf;
	buf = NULL;
	unlink(this->path.c_str());
	return NULL;
}
