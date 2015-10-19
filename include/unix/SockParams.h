#if !defined (UNIXSOCKPARAMS_H__)
#define UNIXSOCKPARAMS_H__ (1)

#include <stdint.h> /* int32_t .. */
#include <pthread.h> /* pthread_t */
#include <string.h>


class UNIXOnSocket;

/*
 * NAME UNIXSockStartConnParams - class for start connect to server
 */
class UNIXSockStartConnParams {

public: UNIXSockStartConnParams(uint32_t ip,
	uint16_t port,
	uint16_t timeout,
	UNIXOnSocket * onSocket);

public: uint32_t getIP() const;
public: uint16_t getPort() const;
public: uint16_t getTimeout() const;
public: UNIXOnSocket * getOnSocket() const;
public: pthread_t getTid() const;

private: uint32_t ip;/* host order */
private: uint16_t port;/* host order */
private: uint16_t timeout;/* second */


/*
 * NAME UNIXOnSocket - connected / connect timeout/ disconn / ...
 *
 * NOTE
 *   - Must manual set onSocket before call start connect or will fail
 */
private: UNIXOnSocket * onSocket;

};


/*
 * NAME UNIXSockStartConnParamsD - class for start connect to server (domain)
 */
class UNIXSockStartConnParamsD {

public: UNIXSockStartConnParamsD(const char * domain,
	uint16_t port,
	uint16_t timeout,
	UNIXOnSocket * onSocket)
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
public: UNIXOnSocket * getOnSocket() const { return this->onSocket; }
public: const char * getDomain() const { return this->domain; }

private: uint16_t port;/* host order */
private: uint16_t timeout;/* second */
private: UNIXOnSocket * onSocket;
private: char domain[1024];

};


class UNIXSockConnParams {

friend class UNIXSocket;

protected: UNIXSockConnParams();

public: int getSockfd() const;
public: uint32_t getIP() const;
public: uint16_t getPort() const;
public: uint16_t getTimeout() const;
public: UNIXOnSocket * getOnSocket() const;
public: pthread_t getTid() const;

protected: int sockfd;
protected: pthread_t tid;

protected: uint32_t ip;/* host order */
protected: uint16_t port;/* host order */
protected: uint16_t timeout;/* second */

protected: UNIXOnSocket * onSocket;

};


class UNIXSockConnParamsDomain {

friend class UNIXSocket;

protected: UNIXSockConnParamsDomain();

public: const char * getDomain() const;
public: uint16_t getPort() const;
public: uint16_t getTimeout() const;
public: UNIXOnSocket * getOnSocket() const;
public: pthread_t getTid() const;

protected: int sockfd;
protected: pthread_t tid;

protected: uint16_t port;/* host order */
protected: uint16_t timeout;/* second */

protected: UNIXOnSocket * onSocket;
protected: char domain[1024];

};


class UNIXSockReceiveParams {

friend class UNIXSocket;

public: int getSockfd () const {
	return this->sockfd;
}


public: UNIXOnSocket * getOnSocket () const {
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
protected: UNIXOnSocket * onSocket;

protected: uint32_t peerIP;/* host order */
protected: uint16_t peerPort;/* host order */

protected: pthread_t tid;

};


#endif /* UNIXSOCKPARAMS_H__ */
