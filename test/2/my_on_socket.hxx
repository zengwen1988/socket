#if !defined (MYONSOCKET_HXX__)
#define MYONSOCKET_HXX__ (1)

#include <xsocket/on_client_socket.hxx>

class MyOnSocket: public xsocket::OnClientSocket
{

public: MyOnSocket();

/* override */
public:
	virtual int onConnect(const xsocket::SockOnConnnect& sc);
	virtual int willFinish(const xsocket::SockWillFinish&);
	virtual int onReceived(const xsocket::SockRecved);
	virtual bool shouldTeminateRecv(int);

public:
	int sockfd (void) const { return this->_sockfd; }
	void set_sockfd (int sockfd) { this->_sockfd = sockfd; }

private:
	int _sockfd;

};

#endif /* MYONSOCKET_HXX__ */
