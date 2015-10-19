/*
 * NAME
 *   unix_sock_util.c - unix socket utilities
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
 *   . new set_sock_block
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


#include <unix/sock.h> /* header */

#include <fcntl.h> /* F_GETFL .. */

#include <c_log.h> /* log2stream */


#if defined(__cplusplus)
extern "C" {
#endif

/*
 * =========================================================================
 * utilities part
 * =========================================================================
 */

/*
 * NAME set_sock_block - set socket block or not
 *
 * RETURN
 *   - success: 0
 *   - fail: -errno
 */
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


/*
 * =========================================================================
 * operations part
 * =========================================================================
 */

/*
 * NAME start_connect_by_protocol - start to connect to ip:port
 *
 * PARAMS
 *   - ip: host order IPv4
 *   - port: host order
 *
 * RETURN
 *   - success: sockfd >= 0
 *   - fail: -errno
 */
extern int start_connect_by_protocol(uint32_t ip, uint16_t port);

#if defined(__cplusplus)
}
#endif
