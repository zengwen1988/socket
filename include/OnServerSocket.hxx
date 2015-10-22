#if !defined (ONSERVERSOCKET_HXX__)
#define ONSERVERSOCKET_HXX__ (1)

#include <sock.h> /* type .. */

class XOnServerSocket {

public: virtual ~XOnServerSocket() {}

/*
 * NAME
 *   onConnected - to override this callback
 *
 * NOTE-XXX
 *   - Should not block (this callback not in a thread)
 *   - TODO: callback in a thread
 */
public: virtual int onConnected(int, int, net_protocol_t) = 0;

/* override this */
public: virtual int willFinish(sock_will_finish_t) = 0;

/* override this */
public: virtual int onReceived(sock_recved_t) = 0;

/* override this */
public: virtual bool shouldTeminate(int sockfd) = 0;

};

#endif /* ONSERVERSOCKET_HXX__ */
