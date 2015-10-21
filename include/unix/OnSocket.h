#if !defined (UNIXONSOCKET_H__)
#define UNIXONSOCKET_H__ (1)

#include <unix/sock.h>
#include <unix/SockParams.h>


class UNIXOnSocket {

public: virtual ~UNIXOnSocket() {}

/* override this */
public: virtual int onConnect(sock_on_conn_t);

/* override this */
public: virtual int willFinish(sock_will_finish_t);

/* override this */
public: virtual int onReceived(int, const uint8_t *, size_t);

/* override this */
public: virtual bool shouldTeminateRecv(int sockfd);
};

#endif /* UNIXONSOCKET_H__ */
