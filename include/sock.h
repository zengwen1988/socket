/*
 * UNIX
 * OSX
 * iOS
 * WIN32
 */

#if !defined(SOCK_H__)
#define SOCK_H__ (1)

#if !defined(_WIN32)
#include <sys/socket.h>
#else
#include <Winsock2.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

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

extern int __cdecl shutdown_socket(int sockfd, SHUTDOWN_HOW_t how);
extern int __cdecl close_socket(int sockfd);

#if defined(__cplusplus)
}
#endif

#endif /* !defined(SOCK_H__) */
