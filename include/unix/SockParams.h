#if !defined (UNIXSOCKPARAMS_H__)
#define UNIXSOCKPARAMS_H__ (1)

#include <stdint.h> /* int32_t .. */
#include <pthread.h> /* pthread_t */


/*
 * NAME unix_sock_on_conn_params_t - yes! struct params safe copy
 */
typedef struct {
	/*
	 * code: 0: connect success and all ok
	 * > 0: connect success but start receive thread fail
	 * < 0: connect fail
	 */
	int code;
	int sockfd;
	uint32_t pairIP;
	uint16_t pairPort;
} unix_sock_on_conn_params_t;


/*
 * NAME unix_sock_on_conn_params_t - yes! struct params safe copy
 */
typedef struct {
	uint8_t * data;
	int count;
	int sockfd;
	uint32_t pairIP;
	uint16_t pairPort;
} unix_sock_on_recv_params_t;


typedef int(on_connect_t)(unix_sock_on_conn_params_t);


class UNIXOnSocket;

/*
 * NAME UNIXSockConnParams - class for start connect to server
 *
 * PARAMS:
 *   - TODO
 */
class UNIXSockConnParams {

public: UNIXSockConnParams(uint32_t ip,
	uint16_t port,
	uint16_t timeout,
	UNIXOnSocket * onSocket);

public: uint32_t getIP() const;
public: uint16_t getPort() const;
public: uint16_t getTimeout() const;
public: UNIXOnSocket * getOnSocket() const;

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


class ConnectRoutineParams {

friend class UNIXSocketClientHelper;

public: int getSockfd () const {
	return this->sockfd;
}


public: uint16_t getTimeout () const {
	return this->timeout;
}


public: UNIXOnSocket * getOnSocket () const {
	return this->onSocket;
}


public: uint32_t getServerIP () const {
	return this->serverIP;
}


public: uint16_t getServerPort () const {
	return this->serverPort;
}


public: pthread_t getTid () const {
	return this->tid;
}


protected: int sockfd;
protected: uint16_t timeout;/* seconds */
protected: UNIXOnSocket * onSocket;

protected: uint32_t serverIP;/* host order */
protected: uint16_t serverPort;/* host order */

protected: pthread_t tid;

};


class RecveiveParams {

	friend class UNIXSocketClientHelper;

	public: int getSockfd () const {
		return this->sockfd;
	}


	public: UNIXOnSocket * getOnSocket () const {
		return this->onSocket;
	}


	public: uint32_t getServerIP () const {
		return this->serverIP;
	}


	public: uint16_t getServerPort () const {
		return this->serverPort;
	}


	public: pthread_t getTid () const {
		return this->tid;
	}


	protected: int sockfd;
	protected: UNIXOnSocket * onSocket;

	protected: uint32_t serverIP;/* host order */
	protected: uint16_t serverPort;/* host order */

	protected: pthread_t tid;

};


#endif /* UNIXSOCKPARAMS_H__ */
