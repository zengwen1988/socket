#if !defined (MYONSOCKET_H__)
#define MYONSOCKET_H__ (1)

#include <OnClientSocket.hxx>

class MyOnSocket: public XOnClientSocket
{

public: MyOnSocket();

/* override */
public: virtual int onConnect(sock_on_conn_t sc);


/* override */
public: virtual int willFinish(sock_will_finish_t fi);

/* override */
public: virtual int onReceived(int sockfd, const uint8_t *, size_t sz);

/* override */
public: virtual bool shouldTeminateRecv(int);

public: int getSockfd () const { return this->sockfd; }

private: int sockfd;

};

#endif /* MYONSOCKET_H__ */
