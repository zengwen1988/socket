/*
 * NAME
 *   sock.c - socket operations
 *
 * DESC
 *   - funcs all in c
 *   - support: linux / osx / ios
 *
 * CC
 *   ...gcc / g++
 *
 * CREATED
 *   2015-10-19 1 292 02:04:45 -0400 by yuiwong
 *
 * V
 *   - 1.0.0.0 - 20151019
 *   first version
 *   - NEW TODO
 *
 * LICENSE
 *   LGPLv3
 *	 This file is part of socket.
 *	 socket is free software:
 *	 you can redistribute it and/or modify it under the terms of the GNU
 *	 General Public License as published by the Free Software Foundation,
 *	 either version 3 of the License, or (at your option) any later version.
 *   socket is distributed in the hope that it will be
 *	 useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	 GNU General Public License for more details.
 *	 You should have received a copy of the GNU General Public License
 *	 along with socket.
 *	 If not, see <http://www.gnu.org/licenses/>.
 */


#include <sock.h>

#if !defined(_WIN32)
#	include <unistd.h> /* close */
#	include <sys/socket.h> /* SOL_SOCKET .. */
#else
#	include <windows.h>
#endif
#include <fcntl.h> /* F_GETFL .. */

#include <c_log.h> /* debug log */

#if defined(__cplusplus)
extern "C" {
#endif

/*
 * NAME set_sock_block - set socket block or not
 *
 * RETURN
 *   - success: 0
 *   - fail: -errno
 */
#if !defined(_WIN32)
int set_sock_block(int sockfd, uint8_t block)
{

	int ret;
	int flags;

	flags = fcntl(sockfd, F_GETFL, 0);
	if (flags < 0) {
		ret = -errno;

		log2stream(stderr, "fcntl fail");

		return ret;
	}

	if (0 == block) {
		ret = fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
	} else {
		flags &= ~ O_NONBLOCK;
		ret = fcntl(sockfd, F_SETFL, flags);
	}

	if (ret < 0) {
		ret = -errno;

		log2stream(stderr, "fcntl fail");

		return ret;
	} else {
		return 0;
	}

}
#endif


/*
 * Shut down all or part of the connection open on socket FD.
 *
 * HOW determines what to shut down:
 *   SHUT_RD   = No more receptions;
 *   SHUT_WR   = No more transmissions;
 *   SHUT_RDWR = No more receptions or transmissions.
 *
 * Returns 0 on success, -1 for errors.
 */
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


int init_socket_environment (void)
{

#if defined(_WIN32)
	WSADATA  d;

	/* init windows socket */
	return WSAStartup(MAKEWORD(2,2), &d);
#endif

	return 0;

}


void deinit_socket_environment (void)
{

#if defined(_WIN32)
	WSACleanup();
#endif

}

#if defined(__cplusplus)
}
#endif
