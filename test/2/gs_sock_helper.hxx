#if !defined(GSSOCKHELPER_HXX__)
#define GSSOCKHELPER_HXX__ (1)

#include "my_on_socket.hxx"


/*
 * NAME GSSockHelper - Gun Safe socket helper
 */
class GSSockHelper
{

public:
	static int connectToServer(void);
	static int disconnect(void);

public:
	static xsocket::status::Type sock_status (void) {
		return GSSockHelper::_sock_status;
	}
	static void set_sock_status(xsocket::status::Type status) {
		GSSockHelper::_sock_status = status;
	}

public:
	static MyOnSocket * on_sock () { return GSSockHelper::os; }

// private: static XSockStartConnParamsD * ps;
private:
	static MyOnSocket * os;
	static xsocket::status::Type _sock_status;

};

#endif /* !defined(GSSOCKHELPER_HXX__) */
