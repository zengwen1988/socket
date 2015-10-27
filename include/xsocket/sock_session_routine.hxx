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

namespace xsocket { namespace core { namespace internal { class SockServerAcceptRoutine; } } }

namespace xsocket { namespace core { namespace internal {

class SockSessionRoutine: public pthreadx::SimpleThread
{

/*
 * 1 new by SockServerAcceptRoutine
 * 2 delete by SockServerAcceptRoutine when start session fail
 *   and delete by OnSession::didFinish when start session success
 *   and willFinish called
 */
friend class xsocket::core::internal::SockServerAcceptRoutine;

protected:
	/* create by xsocket::OnSession */
	SockSessionRoutine(int sockfd);

protected:
	/* delete by xsocket::OnSession */
	~SockSessionRoutine();

protected:
	virtual void * run(void * session_callback);

private:
	static size_t instance_num;
};

} } }

#endif /* XSOCKET_SESSION_ROUTINE_HXX_ */
