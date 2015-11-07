/* ===========================================================================
 *
 * NAME
 *   xsocket/sock_server_accept_routine.hxx
 *   Socket server accept routine class (internal)
 *
 * DESC
 *   TODO
 *
 * CREATED
 *   2015-10-26 AM +0800 by yuiwong
 *   E-mail: yuiwong@126.com
 *
 * SUPPORTED / TESTED PLATFORM
 *   TODO
 *
 * SUPPORTED / TESTED COMPILER
 *   TODO
 *
 * VERSION
 *   1.0.0.0 - TODO
 *
 * LICENSE
 *   LGPLv3
 *	 THIS FILE (is a part of XSOCKET)
 *   is free software:
 *	 you can redistribute it and/or modify it under the terms of the GNU
 *	 General Public License as published by the Free Software Foundation,
 *	 either version 3 of the License, or (at your option) any later version.
 *   THIS FILE
 *   is distributed in the hope that it will be
 *	 useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	 GNU General Public License for more details.
 *	 You should have received a copy of the GNU General Public License
 *	 along with
 *   THIS FILE.
 *	 If not, see <http://www.gnu.org/licenses/>.
 *
 * ===========================================================================
 */

#if !defined(XSOCKET_SOCK_SERVER_ACCEPT_ROUTINE_HXX__)
#define XSOCKET_SOCK_SERVER_ACCEPT_ROUTINE_HXX__ (1)

#include <posix/thread/simple_thread.hxx> /* pthreadx::SimpleThread */

namespace xsocket { class OnServerSocket; }

namespace xsocket { namespace core { namespace internal {

/*
 * NAME class xsocket::core::internal::SockServerAcceptRoutine
 *   server accept routine
 */
class SockServerAcceptRoutine: public pthreadx::SimpleThread {

/*
 * - Only can be created by xsocket::OnServerSocket
 * - No arg
 * - Will be set by func of xsocket::OnServerSocket for SockServerHelper
 */
friend class xsocket::OnServerSocket;
protected:
	SockServerAcceptRoutine(void);
	virtual ~SockServerAcceptRoutine();

protected:
	/* arg: xsocket::OnServerSocket */
	virtual void * run(void * server_callback);

private:
	static size_t instance_num;
};

} } }


#endif /* XSOCKET_SOCK_SERVER_ACCEPT_ROUTINE_HXX__ */
