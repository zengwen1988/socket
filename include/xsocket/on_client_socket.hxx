/* ==========================================================================
 *
 * NAME include/xsocket/on_client_socket.hxx
 *   abstract class xsocket::OnClientSocket
 *
 * DESC
 *   - This project all (socket related only) on STL and NO C++ 11
 *   - All client callbacks
 *
 * CREATED
 *   2015-10 by Yui Wong
 *   E-mail: yuiwong@126.com
 *
 * VERSIONS
 *   - 1.0.0.1 - 2015-10-24 PM +0800
 *   Fisrt stable verion
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

#if !defined (XSOCKET_ONCLIENTSOCKET_HXX__)
#define XSOCKET_ONCLIENTSOCKET_HXX__ (1)

#include <cstdio>

#include <xsocket/basic_sock_type.hxx>

/* NOT INCLUDES IN THIS FILE */
namespace xsocket { namespace core { namespace internal {
	class ConnectToServerBySockfd;
} } }

namespace xsocket {

class OnClientSocketGC;

/*
 * NAME class xsocket::OnClientSocket
 *   abstract class
 */
class OnClientSocket {

/* to delete */
friend class xsocket::core::internal::ConnectToServerBySockfd;

friend class xsocket::OnClientSocketGC;
protected:
	OnClientSocket(void);
	virtual ~OnClientSocket();

/* override */
public:
	virtual int onConnect(const xsocket::SockOnConnnect&) = 0;
	virtual int	didFinish(const xsocket::SockDidFinish&) = 0;
	virtual int onReceived(xsocket::SockRecved&) = 0;
	/* override true recv routine will exit */
	virtual bool shouldTeminateRecv(int sockfd) = 0;

	inline xsocket::OnClientSocketGC * gc (void) const { return this->_gc; }

private:
	xsocket::OnClientSocketGC * _gc;
};

class OnClientSocketGC {
friend class xsocket::OnClientSocket;
protected:
	OnClientSocketGC (void) { }
	~OnClientSocketGC () { }

public:
	void gc (xsocket::OnClientSocket * g) { if (NULL != g) { delete g;} }
};

} /* namespace xsocket */

#endif /* XSOCKET_ONCLIENTSOCKET_HXX__ */
