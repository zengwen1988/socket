/* ==========================================================================
 *
 * NAME include/sock_server_helper.hxx
 *   Server socket helper
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

#if !defined(XSOCKET_SOCK_SERVER_HELPER_HXX__)
#define XSOCKET_SOCK_SERVER_HELPER_HXX__ (1)

#include <xsocket/on_server_socket.hxx>
#include <xsocket/on_session.hxx>

namespace xsocket {

/*
 * TODO: support more than one server
 */
class SockServerHelper {

public: static int startServer(const xsocket::NetProtocol& bindto,
	xsocket::OnServerSocket * server_callback,
	xsocket::OnSession * session_callback);

};

} /* namespace xsocket */

#endif /* !defined(XSOCKET_SOCK_SERVER_HELPER_HXX__) */
