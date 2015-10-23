#if !defined (XSOCKPARAMS_HXX__)
#define XSOCKPARAMS_HXX__ (1)

#if !defined(__cplusplus)
#	error "Please use complier for cxx"
#endif

#include <stdint.h> /* int32_t .. */
#include <pthread.h> /* pthread_t */
#include <string.h>


/*
 * ==========================================================================
 * NAME
 *   XSockStartConnParams - class for start connect to server
 *
 * DESC
 *   For client
 * ==========================================================================
 */
class XOnClientSocket;

class XSockStartConnParams {

public: XSockStartConnParams(uint32_t ip,
	uint16_t port,
	uint16_t timeout,
	XOnClientSocket * onSocket);

public: uint32_t getIP() const;
public: uint16_t getPort() const;
public: uint16_t getTimeout() const;
public: XOnClientSocket * getOnSocket() const;
public: pthread_t getTid() const;

private: uint32_t ip;/* host order */
private: uint16_t port;/* host order */
private: uint16_t timeout;/* second */


/*
 * NAME
 *   XOnClientSocket - connected / connect timeout/ disconn / ...
 *
 * NOTE
 *   - Must manual set onSocket before call start connect or will fail
 */
private: XOnClientSocket * onSocket;

};


/*
 * ==========================================================================
 * NAME
 *   XSockStartConnParamsD - class for start connect to server (domain)
 *
 * DESC
 *   For client
 * ==========================================================================
 */
class XSockStartConnParamsD {

public: XSockStartConnParamsD (const char * domain,
	uint16_t port,
	uint16_t timeout,
	XOnClientSocket * onSocket)
{

	bzero(this->domain, 1024);

	if (NULL != domain) {
		strncpy(this->domain, domain, 1024);
		this->domain[1023] = '\0';
	}

	this->port = port;
	this->timeout = timeout;
	this->onSocket = onSocket;

}

public: uint16_t getPort() const { return this->port; }
public: uint16_t getTimeout() const { return this->timeout; }
public: XOnClientSocket * getOnSocket() const { return this->onSocket; }
public: const char * getDomain() const { return this->domain; }

private: uint16_t port;/* host order */
private: uint16_t timeout;/* second */
private: XOnClientSocket * onSocket;
private: char domain[1024];

};


/*
 * ==========================================================================
 * NAME
 *   XSockConnParams - class socket connect params
 *
 * DESC
 *   - For inner (XSocketClient and client helper) use only
 * ==========================================================================
 */
class XSockClientUtil;

class XSockConnParams {

friend class XSockClientUtil;

protected: XSockConnParams();

public: int getSockfd() const;
public: uint32_t getIP() const;
public: uint16_t getPort() const;
public: uint16_t getTimeout() const;
public: XOnClientSocket * getOnSocket() const;
public: pthread_t getTid() const;

protected: int sockfd;
protected: pthread_t tid;

protected: uint32_t ip;/* host order */
protected: uint16_t port;/* host order */
protected: uint16_t timeout;/* second */

protected: XOnClientSocket * onSocket;

};


/*
 * ==========================================================================
 * NAME
 *   XSockConnParamsDomain - class socket connect params when use domain
 *
 * DESC
 *   - For inner (XSocketClient and client helper) use only
 * ==========================================================================
 */
class XSockConnParamsDomain {

friend class  XSockClientUtil;

protected: XSockConnParamsDomain();

public: const char * getDomain() const;
public: uint16_t getPort() const;
public: uint16_t getTimeout() const;
public: XOnClientSocket * getOnSocket() const;
public: pthread_t getTid() const;

protected: int sockfd;
protected: pthread_t tid;

protected: uint16_t port;/* host order */
protected: uint16_t timeout;/* second */

protected: XOnClientSocket * onSocket;
protected: char domain[1024];

};


class XSockReceiveParams {

friend class XSockClientUtil;

public: int getSockfd () const {
	return this->sockfd;
}


public: XOnClientSocket * getOnSocket () const {
	return this->onSocket;
}


public: uint32_t getPeerIP () const {
	return this->peerIP;
}


public: uint16_t getPeerPort () const {
	return this->peerPort;
}


public: pthread_t getTid () const {
	return this->tid;
}


protected: int sockfd;
protected: XOnClientSocket * onSocket;

protected: uint32_t peerIP;/* host order */
protected: uint16_t peerPort;/* host order */

protected: pthread_t tid;

};


/*
 * ==========================================================================
 * NAME
 *   XSockServerCliSessionParams - class for XSockServerCliSessionRoutine
 * ==========================================================================
 */
class XSockServerCliSessionRoutine;
class XSockServerAcceptRoutine;
class XOnServerSocket;

/* XXX: this class NOT delete passed in serverCallback */
class XSockServerCliSessionParams {

/* can call new / delete */
friend class XSockServerCliSessionRoutine;
friend class XSockServerAcceptRoutine;

protected: XSockServerCliSessionParams(void);
protected: XSockServerCliSessionParams(XSockServerCliSessionParams &ref);
protected: ~XSockServerCliSessionParams();

public: const XOnServerSocket * getServerCallback (void) const;
protected: void setServerCallback(const XOnServerSocket * serverCallback);

private: const XOnServerSocket * serverCallback;
private: int serverfd;
private: int clifd;

};

#endif /* XSOCKPARAMS_HXX__ */
