/* ==========================================================================
 *
 * NAME include/sock.hxx
 *   Base socket class
 *
 * DESC
 *   - This project all (socket related only) on STL and NO C++ 11
 *   - class xsocket::Sock
 *
 * SUPPORT PLATFORMS
 *   - Linux
 *   - MacOS
 *   - Windows
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

#if !defined(XSOCKET_SOCK_HXX__)
#define XSOCKET_SOCK_HXX__ (1)

#include <cstring> /* memcpy */

#if !defined(WIN32)
#	include <stdint.h>
#else
#	include <posix/mingw/stdint.h>
#endif

#if !defined(WIN32)
#	include <netinet/in.h> /* sockaddr_in */
#else
#	pragma comment(lib, "ws2_32.lib")
// #	include <WinSock2.h> /* sockaddr_in */
#	include <windows.h> /* sockaddr_in */
#endif

/* NOT INCLUDES IN THIS FILE */
/* class xsocket::core::internal::ConnectToServerBySockfd */
namespace xsocket { namespace core { namespace internal { class ConnectToServerBySockfd; } } }

namespace xsocket {

/* xsocket::Sock base socket class, for sub-class only */
class Sock {
/* to delete when fail */
friend class xsocket::core::internal::ConnectToServerBySockfd;

public:
	Sock(void);
protected:
	~Sock();

public:
	void relaseSocket(void);

public:
	inline int sockfd (void) const { return this->_sockfd; }
	inline sockaddr_in local_address (void) const {
		return this->_local_address;
	}

protected:
	inline void set_sockfd (int sockfd) {
		this->_sockfd = sockfd;
	}
	inline void set_local_address (const sockaddr_in& addr) {
		memcpy(&(this->_local_address), &addr, sizeof(sockaddr_in));
	}

private:
	int _sockfd;
	sockaddr_in _local_address;

};

} /* namespace xsocket */

#endif /* XSOCKET_SOCK_HXX__ */
