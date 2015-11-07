/* ==========================================================================
 *
 * NAME include/sock_session_routine.hxx
 *   class xsocket::core::internal::SockSessionRoutine
 *
 * DESC
 *   - This project all (socket related only) on STL and NO C++ 11
 *   - For server, when client connect to server
 *   server create a session
 *   and when disconnect or client be close
 *   or server closed session routine will be auto delete
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

#if !defined(XSOCKET_SESSION_ROUTINE_HXX_)
#define XSOCKET_SESSION_ROUTINE_HXX_ (1)

#include <posix/thread/simple_thread.hxx> /* pthreadx::SimpleThread */
#include <xsocket/basic_sock_type.hxx>

#define XSOCKET_WAIT_READABLE_TIMEOUT (15) /* second */
#define XSOCKET_READ_TIMEOUT (15) /* second */

namespace xsocket { class OnSession; }

namespace xsocket { namespace core { namespace internal {

class SockSessionRoutineGC;

class SockSessionRoutine: public pthreadx::SimpleThread {

/*
 * 1 new by SockServerAcceptRoutine
 * 2 delete by SockServerAcceptRoutine when start session fail
 *   and delete by OnSession::didFinish when start session success
 *   and willFinish called
 */
friend class xsocket::OnSession;

protected:
	/* create by xsocket::OnSession */
	SockSessionRoutine(int cli_fd, const xsocket::NetProtocol& client,
		int svr_fd, const xsocket::NetProtocol& server,
		xsocket::OnSession * on_session);

friend class xsocket::core::internal::SockSessionRoutineGC;
protected:
	/* delete by xsocket::OnSession */
	virtual ~SockSessionRoutine();

protected:
	virtual void * run(void * /* nil */);

	xsocket::core::internal::SockSessionRoutineGC * gc;

private:
	int cli_fd;
	int svr_fd;
	xsocket::NetProtocol client;
	xsocket::NetProtocol server;
	/* will not delete when session routine exit */
	xsocket::OnSession * on_session;

	static int instance_num;
};

class SockSessionRoutineGC {
friend class xsocket::core::internal::SockSessionRoutine;
protected:
	SockSessionRoutineGC (void) {}
	inline void gc (xsocket::core::internal::SockSessionRoutine * g) {
		if (NULL != g) {
			delete g;
		}
	}
};

} } }

#endif /* XSOCKET_SESSION_ROUTINE_HXX_ */
