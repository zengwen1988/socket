/*
 * NAME unix/sock.h - unix socket header
 *
 * DESC
 *   - funcs all in c
 *   - support: linux / osx / ios
 *
 * CC ...gcc / g++
 *
 * CREATED: 2015-10-19 1 292 02:04:45 -0400 by yuiwong
 *
 * V
 *   - 1.0.0.0 - TODO
 *   . new set_sock_block
 */


#if !defined (UNIXSOCK_H__)
#define UNIXSOCK_H__ (1)

#include <pthread.h> /* pthread_t */
#include <sys/types.h> /* ssize_t */

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h> /* uint8_t .. */

typedef struct _sock_connect_info_t {
	uint32_t peer_ip;
	uint16_t peer_port;
} sock_info_t;

typedef struct _sock_on_conn_t {
	int code;
	int sockfd;
	sock_info_t info;
} sock_on_conn_t;

typedef struct _sock_will_finish_t {
	int code;
	int sockfd;
	sock_info_t info;
} sock_will_finish_t;

typedef struct _sock_recved_t {
	uint8_t * data;
	int count;
	int sockfd;
	sock_info_t info;
} sock_recved_t;

/*
 * connect fail
 * connect success
 */
typedef int(on_connect_t)(sock_on_conn_t);
typedef int(will_finish_t)(sock_will_finish_t);
typedef int(on_received_t)(sock_recved_t);
typedef uint8_t(should_teminate_recv_t)(int);

typedef struct _on_sock_t {
	on_connect_t * on_connect;
	will_finish_t * will_finish;
	on_received_t * on_received;
	should_teminate_recv_t * should_teminate_recv;
} on_sock_t;

typedef struct _sock_start_conn_t {
	uint16_t timeout;/* second */
	on_sock_t on_sock;
	sock_info_t info;
} sock_start_conn_t;

struct _sock_conn_t {
	int sockfd;
	uint16_t timeout;/* second */
	pthread_t tid;/* self */
	on_sock_t on_sock;
	sock_info_t info;
};

struct _sock_recv_t {
	int sockfd;
	pthread_t tid;/* self */
	will_finish_t * will_finish;
	on_received_t * on_received;
	should_teminate_recv_t * should_teminate_recv;
	sock_info_t info;
};

/* socket util */
extern int set_sock_block(int sockfd, uint8_t block);

/* socket operation */
extern int get_sockfd_by_ip(uint32_t ip/* host prder*/,
	uint16_t port/*host order*/);
extern int get_sockfd_by_ipn(uint32_t ipn/* net order */,
	uint16_t portn/* net order */);
extern int get_sockfd_by_ipstr(const char * ip/* host order */, uint16_t port);
extern int start_conn_by_sock_fd(int sockfd,
	const sock_start_conn_t * ps);
extern ssize_t recv_from_sockfd (int32_t sockfd, uint8_t * buf, int32_t start,
size_t max, uint32_t wr_us, uint32_t r_us);

#if defined(__cplusplus)
}
#endif

#endif /* UNIXSOCK_H__ */
