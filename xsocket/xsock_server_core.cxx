/* ==========================================================================
 *
 * NAME xsocket/xsock_client_core.cxx
 *   Server socket (only) core here
 *
 * DESC
 *   - This project all (socket related only) on STL and NO C++ 11
 *
 * CREATED
 *   2015-10 by Yui Wong
 *   E-mail: yuiwong@126.com
 *
 * VERSIONS
 *   - 1.0.0.1 - 2015-10-24 PM +0800
 *   First stable verion
 *
 * LICENSE
 *   GNU LESSER GENERAL PUBLIC LICENSE Version 3
 *
 *   This file is part of SOCKET.
 *   SOCKET is free software: you can redistribute it
 *   and/or modify it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation, either version 3 of the
 *   License, or (at your option) any later version.
 *   SOCKET is distributed in the hope that it will be
 *   useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with SOCKET.
 *   If not, see <http://www.gnu.org/licenses/>.
 *
 * ==========================================================================
 */

#include <xsocket/sock_server_core.hxx>

#include <errno.h> /* errno */
#include <cstring>

#if !defined(NO_C_LOGFILE)
#	include <c_logfile.h>
#endif


/*
 * NAME
 *   xsocket::core::WaitConnect - wait client connect to server
 *
 * DESC
 *   Will block => e.x. call this in a thread
 *
 * RETURN
 *   success: >= 0 clientfd should also check WSAGetLastError
 *   fail: -errno or -WSA errno
 */
int xsocket::core::WaitConnect (int serverfd, struct sockaddr_in * addr)
{

#if	defined(WIN32)
	int le;
#endif
	int fd_conned;
	struct sockaddr_in _addr;
	int len;

	memset(&_addr, 0, sizeof(struct sockaddr_in));
	len = sizeof(struct sockaddr_in);/* XXX-NOTE: len NOT 0 */

#if	defined(WIN32)
	WSASetLastError(0);
#endif

	/*  accept */
	fd_conned = accept(serverfd,
#if		!defined(WIN32)
		(struct sockaddr *)&_addr,
		(socklen_t *)&len);
#else
		(struct sockaddr *)&_addr,
		&len);
#endif

	if (fd_conned >= 0) {

#if		defined(WIN32)
		if (0 == fd_conned) {
			le = WSAGetLastError();
			if (0 != le) {
				return -le;
			}
		}
#endif

		if (NULL != addr) {
			memcpy(addr, &_addr, sizeof(struct sockaddr));
		}

		return fd_conned;
	} else {
#if		!defined(WIN32)
		return -errno;
#else
		return -1 * WSAGetLastError();
#endif
	}

}
