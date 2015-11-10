#include <stdint.h>
#include <stdio.h>
#include <posix/func/usleep.h>
#include <posix/func/bzero.h>
#include <timestamp.h>

#include <string>

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
	GetSendDataRoutine (void * fd) : SimpleThread(fd) {
		this->_byebye = false;

		timestampname();
		char path[512];
		snprintf(path, 511, "/tmp/ncsr/%d");
	}
	void byebye (void) { this->_byebye = true; }

protected:
	virtual void * run (void * sfd) {
		int fd = static_cast<int>(reinterpret_cast<long int>(sfd));
		bool byebye = true;
		do {
			byebye = this->_byebye;
			if (byebye) {
				goto bye;
			}

			usleep(10 * 1e3);
		} while (! byebye);

	bye:
			return NULL;
	}

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
};

class MyXOnServerSocket: public xsocket::OnServerSocket {
/* override this */
public: virtual int onConnected(int, int, const xsocket::NetProtocol&)
{
	return 0;
}

/* override this */
public: virtual int didFinish(xsocket::SockDidFinish) {
	return 0;
}

/* override this */
public: virtual bool shouldTeminate(int) {
	static int bb = 0;

	if (++bb > 30) {
		return true;
	} else {
		return false;
	}
}

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
		xsocket::core::SendData(red.fd, red.data, 0, red.count);
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

	char lof[1024];
	bzero(lof, 1024);

	timestampname_d(lof, "gsserverr.", ".log");
	xlog::Update(lof);

	xlog::ToggleLogLevel();
	xlog::ToggleLogLevel();
	xlog::ToggleLogLevel();
	xlog::AppendV2(__func__, __FILE__, __LINE__, 0);

	MyXOnServerSocket * ss = new MyXOnServerSocket();
	MyXOnSession * ssi = new MyXOnSession();

	xsocket::NetProtocol bindto;
	strcpy(bindto.ip, "0.0.0.0");
	bindto.port = 12345;
	bindto.is_ipv6 = false;
	int ret = xsocket::SockServerHelper::startServer(bindto, ss, ssi);

	xlog::AppendV2("XSockServerHelper::startServer", __FILE__, __LINE__,
		ret);
	if (ret < 0) {
		delete ss;
		ss = NULL;
		delete ssi;
		ssi = NULL;
	}

	usleep(1000 * 1e6);

	return 0;

}
