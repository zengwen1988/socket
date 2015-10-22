/*
 * NAME unix/sock.h - unix socket header
 *
 * DESC
 *   - funcs all in c
 *   - support:
 *   UNIX
 *   OSX
 *   iOS
 *   win32
 *
 * SUPPORT / TESTED COMPLIER
 *   ...gcc / g++ / VC6
 *
 * CREATED: 2015-10-19 1 292 02:04:45 -0400 by yuiwong
 *
 * V
 *   - 1.0.0.0 - TODO
 *   . new set_sock_block
 */
/*
 */

#if !defined(SOCK_H__)
#define SOCK_H__ (1)

#include <stdint.h> /* uint8_t .. */

#if !defined(_WIN32)
#	include <pthread.h> /* pthread_t */
#	include <sys/types.h> /* ssize_t */
#else
#	include <WinSock2.h>
#	include <posix\type\ssize.h> /* ssize_t */
#endif

#if defined(_WIN32)
#	define __CALL_METHOD __cdecl /* yes here also __cdecl */
#else
#	define __CALL_METHOD /* empty */
#endif


#if defined(__cplusplus)
extern "C" {
#endif

#define SOCK_MAX_IP_LEN (28)

typedef struct _net_protocol_t {
	char ip[SOCK_MAX_IP_LEN + 1 + 3];
	uint16_t port;
	uint8_t isv6;
} net_protocol_t;


/*
 * SHUT_RD, SHUT_WR, SHUT_RDWR have the value 0, 1, 2
 */
typedef enum _SHUTDOWN_HOW_t {
#if !defined(_WIN32)
	SDH_SHUT_RD = 0,
	SDH_SHUT_WR = 1,
	SDH_SHUT_RDWR = 2,
#else
	SDH_SHUT_RD = SD_RECEIVE,
	SDH_SHUT_WR = SD_SEND,
	SDH_SHUT_RDWR = SD_BOTH,
#endif
} SHUTDOWN_HOW_t;

typedef enum _socket_status_t {
	SOCK_STATUS_UNKNOWN = 0,
	SOCK_STATUS_CONNECTING,
	SOCK_STATUS_CONNECTED,
	SOCK_STATUS_DISCONNECTING,
	SOCK_STATUS_DISCONNECTED,
} socket_status_t;

typedef struct _sock_on_conn_t {
	int code;
	int sockfd;
	net_protocol_t info;
} sock_on_conn_t;

typedef struct _sock_will_finish_t {
	int code;
	int sockfd;
	net_protocol_t info;
} sock_will_finish_t;

typedef struct _sock_recved_t {
	uint8_t * data;
	int count;
	int fd_receiver;
	/* from sockfd */
	int sockfd;
	/* from info */
	net_protocol_t info;
} sock_recved_t;


/*
 * NAME
 *   on_connected_to_t - func type - on connected to remote
 *
 * OUT_PARAMS
 *   - int: self sockfd for the connection
 *   - net_protocol_t: remote
 */
typedef int(on_connected_to_t)(int, net_protocol_t);

/*
 * NAME
 *   on_connected_t - func type - on remote connected to me
 *
 * OUT_PARAMS
 *   - int: self (me) sockfd (i am server)
 *   - int: sockfd: remote is client
 *   - net_protocol_t: remote ip and port
 */
typedef int(on_connected_t)(int, int, net_protocol_t);

/*
 * NAME
 *   will_finish_t - func type - 'who'(sock_will_finish_t) will finish
 */
typedef int(will_finish_t)(sock_will_finish_t);

/*
 * NAME
 *   on_received_t - func type - when received
 */
typedef int(on_received_t)(sock_recved_t);


/*
 * NAME
 *   should_teminate_t - func type ARG is serverfd or clientfd
 */
typedef uint8_t(should_teminate_t)(int);


typedef struct _on_server_sock_t {
	on_connected_t * on_connected;/* remote (client) connect to me */
	/*
	 * if sockfd is for server the server will finish
	 * if sockfd is for client the connected client will finish(discon..)
	 */
	will_finish_t * will_finish;
	on_received_t * on_received;
	should_teminate_t * should_teminate;
} on_server_sock_t;

/*
 * connect fail
 * connect success
 */
typedef int(on_connect_t)(sock_on_conn_t);
typedef uint8_t(should_teminate_recv_t)(int);

typedef struct _on_client_sock_t {
	on_connect_t * on_connect;
	will_finish_t * will_finish;
	on_received_t * on_received;
	should_teminate_recv_t * should_teminate_recv;
} on_client_sock_t;


typedef struct _sock_start_conn_t {
	uint16_t timeout;/* second */
	on_client_sock_t on_sock;
	net_protocol_t info;
} sock_start_conn_t;

struct _sock_conn_t {
	int sockfd;
	uint16_t timeout;/* second */
#if	!defined(_WIN32)
	pthread_t tid;/* self */
#else
	void * tid;/* self */
#endif
	on_client_sock_t on_sock;
	net_protocol_t info;
};

struct _sock_recv_t {
	int sockfd;
#if	!defined(_WIN32)
	pthread_t tid;/* self */
#else
	void * tid;/* self */
#endif
	will_finish_t * will_finish;
	on_received_t * on_received;
	should_teminate_recv_t * should_teminate_recv;
	net_protocol_t info;
};

/* socket util */
extern int __CALL_METHOD shutdown_socket(int sockfd, SHUTDOWN_HOW_t how);
extern int __CALL_METHOD close_socket(int sockfd);
extern int __CALL_METHOD init_socket_environment(void);
extern void __CALL_METHOD deinit_socket_environment(void);

extern int set_sock_block(int sockfd, uint8_t block);

/* socket operation */
extern int get_sockfd_by_ip(uint32_t ip/* host prder*/,
	uint16_t port/*host order*/);
extern int get_sockfd_by_ipn(uint32_t ipn/* net order */,
	uint16_t portn/* net order */);
extern int get_sockfd_by_ipstr(const char * ip/* host order */,
	uint16_t port);
extern int start_conn_by_sock_fd(int sockfd,
	const sock_start_conn_t * ps);
extern ssize_t recv_from_sockfd (int32_t sockfd, uint8_t * buf, int32_t start,
size_t max, uint32_t wr_us, uint32_t r_us);

extern ssize_t send_data(int sockfd, const uint8_t * data,  int start,
	size_t nbyte);

#if defined(__cplusplus)
}
#endif

#endif /* !defined(SOCK_H__) */
