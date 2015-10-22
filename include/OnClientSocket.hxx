#if !defined (ONCLIENTSOCKET_HXX__)
#define ONCLIENTSOCKET_HXX__ (1)

#include <sock.h> /* type .. */

class XOnClientSocket {

public: virtual ~XOnClientSocket() {}

/* override this */
public: virtual int onConnect(sock_on_conn_t) = 0;

/* override this */
public: virtual int willFinish(sock_will_finish_t) = 0;

/* override this */
public: virtual int onReceived(int, const uint8_t *, size_t) = 0;

/* override this */
public: virtual bool shouldTeminateRecv(int sockfd) = 0;

};

#endif /* ONCLIENTSOCKET_HXX__ */
