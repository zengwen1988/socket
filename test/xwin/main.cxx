#include <stdio.h>
#include <posix/func/usleep.h>
#include <posix/func/bzero.h>
#include <timestamp.h>

#include <x_logfile.hxx>

#if defined(WIN32)
#	include <posix/func.hxx>
#endif

#include <xsocket/basic_sock_type.hxx>
#include <xsocket/sock_core.hxx>
#include <xsocket/on_server_socket.hxx>
#include <xsocket/on_session.hxx>
#include <xsocket/sock_server_helper.hxx>

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
