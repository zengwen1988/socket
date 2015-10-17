#if !defined (UNIXSOCKPARAMS_H__)
#define UNIXSOCKPARAMS_H__ (1)

#include <stdint.h> /* int32_t .. */


/*
 * NAME unix_sock_on_conn_params_t - yes! struct params safe copy
 */
typedef struct {
	int code;
	int fd;
	uint32_t pairIP;
	uint16_t pairPort;
} unix_sock_on_conn_params_t;


class UNIXSockConnParams {

public: UNIXSockConnParams(uint32_t ip,
	uint16_t port,
	uint16_t timeout,
	int(*onConnect)(unix_sock_on_conn_params_t));

public: uint32_t ip;/* host order */
public: uint16_t port;/* host order */
public: uint16_t timeout;/* second */

public: int(*onConnect)(unix_sock_on_conn_params_t);

};



#endif /* UNIXSOCKPARAMS_H__ */
