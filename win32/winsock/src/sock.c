#include <sock.h>

#if defined(__cplusplus)
extern "C" {
#endif

int shutdown_socket (int sockfd, SHUTDOWN_HOW_t how)
{
	return shutdown(sockfd, (int)how);
}


int close_socket (int sockfd)
{
#if !defined(_WIN32)
	return close(sockfd);
#else
	return closesocket(sockfd);
#endif
}

#if defined(__cplusplus)
}
#endif