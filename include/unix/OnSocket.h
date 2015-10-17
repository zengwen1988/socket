#if !defined (UNIXONSOCKET_H__)
#define UNIXONSOCKET_H__ (1)


#include <unix/SockParams.h>


class UNIXOnSocket {

/* override this */
public: virtual int onConnect(unix_sock_on_conn_params_t);

/* override this */
public: virtual int onDisconnected(unix_sock_on_conn_params_t);

/* override this */
public: virtual int onReceived(const uint8_t * data, size_t sz);

/* override this */
public: virtual int onSent(const uint8_t * data, size_t sz);

};

#endif /* UNIXONSOCKET_H__ */
