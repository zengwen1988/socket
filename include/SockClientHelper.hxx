#if !defined (SOCKCLIENTHELPER_HXX__)
#define SOCKCLIENTHELPER_HXX__ (1)

#include <SockParams.hxx>


/*
 * NAME: XSockClientHelper - class UNIXSocketClientHelper
 *
 * DESC
 *   - The socket client helper class
 */
class XSockClientHelper {

protected: XSockClientHelper();

public: static int startConnectToServer(
	const XSockStartConnParams * params);

public: static int startConnectByDomain(const XSockStartConnParamsD * ps);
/*
public: static int startReceiveFromServer(int sockfd,
	UNIXOnSocket * onSocket,
	uint32_t serverIP,
	uint16_t serverPort);
*/

};

#endif /* SOCKCLIENTHELPER_HXX__ */
