#if !defined (UNIXSOCKETCLIENTHELPER_H__)
#define UNIXSOCKETCLIENTHELPER_H__ (1)

#include <unix/SockParams.h>


/*
 * NAME: UNIXSocketClientHelper - class UNIXSocketClientHelper
 *
 * DESC
 *   - The socket client helper class
 */
class UNIXSocketClientHelper {

protected: UNIXSocketClientHelper();

public: static int startConnectToServer(const UNIXSockConnParams * params);

};

#endif /* UNIXSOCKETCLIENTHELPER_H__ */
