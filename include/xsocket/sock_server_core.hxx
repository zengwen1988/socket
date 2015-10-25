/* ==========================================================================
 *
 * NAME include/xsocket/sock_server_core.hxx
 *  Server socket (only) core here (header)
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

#if !defined (XSOCKET_SOCK_SOCK_SERVER_CORE_HXX__)
#define XSOCKET_SOCK_SOCK_SERVER_CORE_HXX__ (1)

#if !defined(WIN32)
#	include <netinet/in.h> /* sockaddr_in */
#else
#	pragma comment(lib, "ws2_32.lib")
// #	include <WinSock2.h> /* sockaddr_in */
#endif

#include <posix/thread/simple_thread.hxx> /* pthreadx::SimpleThread */

/*extension of xsocket::core::internal */
namespace xsocket { namespace core { namespace internal {

#if 0
/* class xsocket::core::internal::ClientRecevier */
class ClientRecevier: public pthreadx::SimpleThread {
protected:
	/* override */
	virtual void * run (void *);
};
#endif

} } } /* namespace xsocket .. */


/* extension of xsocket::core */
namespace xsocket { namespace core {

/* func xsocket::WaitConnect */
int WaitConnect(int sockfd, sockaddr_in * out);

} } /* namespace xsocket .. */

#endif /* XSOCKET_SOCK_SOCK_SERVER_CORE_HXX__ */
