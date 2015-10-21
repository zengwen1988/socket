#if !defined(GSSOCKHELPER_H__)
#define GSSOCKHELPER_H__ (1)

#include <unix/SockParams.h>

#include "MyOnSocket.h"


/*
 * NAME GSSockHelper - Gun Safe socket helper
 */
class GSSockHelper
{

public: static int connectToServer(void);
public: static int disconnect(void);

public: static socket_status_t getSockStatus (void)
{
	return GSSockHelper::sock_status;
}

public: static MyOnSocket * getSock () { return GSSockHelper::os; }

private: static UNIXSockStartConnParamsD * ps;
private: static MyOnSocket * os;
private: static socket_status_t sock_status;

};

#endif /* !defined(GSSOCKHELPER_H__) */
