/* ==========================================================================
 *
 * NAME include/xsocket/sock_core.hxx
 *   All socket core here (header)
 *
 * DESC
 *   - This project all (socket related only) on STL and NO C++ 11
 *   - Funcs xsocket::core ...
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
 *   SOCKET
 *   is free software: you can redistribute it
 *   and/or modify it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation, either version 3 of the
 *   License, or (at your option) any later version.
 *   SOCKET
 *   is distributed in the hope that it will be
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

#if !defined (XSOCKET_SOCK_CORE_HXX__)
#define XSOCKET_SOCK_CORE_HXX__ (1)

#if !defined(WIN32)
#	include <stdint.h> /* uint8_t .. */
#else
#	include <posix/type/ssize.h>
#	include <posix/mingw/stdint.h> /* uint8_t .. */
#endif
#include <stddef.h> /* size_t */

#include <xsocket/basic_sock_type.hxx>

/* namespace xsocket::core */
namespace xsocket { namespace core {

/*
 * NAME func xsocket::core::GetSockfdByHost
 *   - Better used by xsocket::SockClientHelper
 *   or xsocket::SockServerHelper only
 */
int GetSockfdByHost(const xsocket::NetProtocol& target);

ssize_t SendData(int sockfd, const uint8_t * data, int start, size_t _count);

int SetSocketBlock(int sockfd, bool block);

int ShutdownSocket(int sockfd, xsocket::ShutdownHow::Type how);

int CloseSocket(int sockfd);

int RecvFromSockfd(int sockfd, uint8_t * buf, int32_t start, size_t max,
	uint32_t wr_us, uint32_t r_us);

int InitSocketEnvironment(void);

void DeinitSocketEnvironment(void);

#if 0
static int startConnectByName(const char * name, uint16_t port,
	uint32_t timeout, /* second */
		const xsocket::OnClientSocket& on_socket/* client socket callback */);

	static void * connectByDomain(xsocket::SockConnParamsDomain *domain);
	static void * connectBySockfd(xsocket::SockConnParams *params);
	static int startReceiveFromPeer(xsocket::SockReceiveParams *params);
	static void * receiveRoutine(xsocket::SockReceiveParams *params);
#endif

 } /* namespace xsocket::core */
} /* namespace xsocket */

#endif /* XSOCKET_SOCK_CORE_HXX__ */
