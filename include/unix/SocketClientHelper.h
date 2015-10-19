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

public: static int startConnectToServer(
	const UNIXSockStartConnParams * params);

public: static int startConnectByDomain(const UNIXSockStartConnParamsD * ps);
/*
public: static int startReceiveFromServer(int sockfd,
	UNIXOnSocket * onSocket,
	uint32_t serverIP,
	uint16_t serverPort);
*/

};

#endif /* UNIXSOCKETCLIENTHELPER_H__ */
