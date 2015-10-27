/* ==========================================================================
 *
 * NAME include/xsocket/on_session.hxx
 *   abstract class xsocket::OnSession
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

#if !defined (XSOCKET_ON_SESSION_HXX__)
#define XSOCKET_ON_SESSION_HXX__ (1)

#include <xsocket/basic_sock_type.hxx> /* basic type */

/* class xsocket::core::internal::SockSessionRoutine */
// namespace xsocket { namespace core { namespace internal {
// 	class SockSessionRoutine; } } }
/* class xsocket::core::internal::SockServerAcceptRoutine */
namespace xsocket { namespace core { namespace internal {
	class SockServerAcceptRoutine; } } }

namespace xsocket {

/*
 * NAME class xsocket::OnSession
 *   server callback when client connected
 *   one session for multi clients
 */
class OnSession {

friend class xsocket::core::internal::SockServerAcceptRoutine;
// friend class xsocket::core::internal::SockSessionRoutine;

public:
	OnSession(void);

/*
 * 1 will be delete by callback (SockSessionRoutine) when receive session
 * started success
 * 2 or will be delete by SockServerAcceptRoutine when start fail
 * 3 or sub-class
 */
protected:
	virtual ~OnSession();

protected:
	/* */
	int didFinish int didFinish(SockWillFinish) = 0;

/* override */
public:
	virtual int willFinish(SockWillFinish) = 0;
	virtual int onReceived(SockRecved) = 0;
	virtual bool shouldTeminate(int sockfd) = 0;

private:
	static size_t instance_num;

};

} /* namespace xsocket */

#endif /* XSOCKET_ON_SESSION_HXX__ */
