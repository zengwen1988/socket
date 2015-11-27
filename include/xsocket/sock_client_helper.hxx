/* ==========================================================================
 *
 * NAME include/xsocket/sock_client_helper.hxx
 *   Class xsocket::SockClientHelper header
 *
 * DESC
 *   - This project all (socket related only) on STL and NO C++ 11
 *   - class xsocket::SockClientHelper
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

#if !defined (XSOCKET_SOCK_CLIENTHELPER_HXX__)
#define XSOCKET_SOCK_CLIENTHELPER_HXX__ (1)

#include <xsocket/basic_sock_type.hxx> /* type .. */
#include <xsocket/on_client_socket.hxx>

namespace xsocket {

/*
 * NAME: class SockClientHelper
 *   - The client socket helper class
 */
class SockClientHelper {

protected: SockClientHelper (void) {} /* no instance here */

public:
	static int startConnectByHost(
		const xsocket::NetProtocol& target,
		uint32_t timeout, /* second */
		xsocket::OnClientSocket * on_socket/* client socket callback */);
	static int startConnectByHostV2(
		xsocket::ip_t * ip,
		xsocket::port_t * port,
		uint32_t timeout, /* second */
		xsocket::OnClientSocket * on_socket/* client socket callback */);

	static int startConnectByName(const char * name, uint16_t port,
		uint32_t timeout, /* second */
		xsocket::OnClientSocket * on_socket/* client socket callback */);

};

} /* namespace xsocket */

#endif /* XSOCKET_SOCK_CLIENTHELPER_HXX__ */
