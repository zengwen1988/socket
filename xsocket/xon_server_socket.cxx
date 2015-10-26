/* ===========================================================================
 *
 * NAME
 *   xsocket/xon_server_socket.cxx
 *   Base server callback class
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

#include <xsocket/on_server_socket.hxx>

#if !defined(NO_C_LOGFILE)
#	include <c_logfile.h>
#endif

#include <xsocket/sock_server_accept_routine.hxx>

xsocket::OnServerSocket::OnServerSocket (void)
{
	++OnServerSocket::instance_num;

#if !defined(NO_C_LOGFILE) && defined(ENABLE_SOCK_DEBUG)
	char dmsg[128];
	snprintf(dmsg, 127, "func: %s: in: %zu", __func__,
		OnServerSocket::instance_num);
	clogf_append_v2(dmsg, __FILE__, __LINE__, 0);
#endif
	this->_accept_routine
		= new xsocket::core::internal::SockServerAcceptRoutine();
	this->acceptPrepare();
}

xsocket::OnServerSocket::~OnServerSocket ()
{
#if !defined(NO_C_LOGFILE) && defined(ENABLE_SOCK_DEBUG)
	char dmsg[128];
	snprintf(dmsg, 127, "func: %s: in-will: %zu", __func__,
		OnServerSocket::instance_num - 1);
	dmsg[127] = '\0';
	clogf_append_v2(dmsg, __FILE__, __LINE__, 0);
#endif

	if (NULL != this->_accept_routine) {
		delete this->_accept_routine;
		this->_accept_routine = NULL;
	}

	--OnServerSocket::instance_num;
}

bool xsocket::OnServerSocket::acceptPrepare (void)
{
#if !defined(NO_C_LOGFILE) && defined(ENABLE_SOCK_DEBUG)
	/* trace */ char dmsg[128];
	snprintf(dmsg, 127, "func: %s", __func__); dmsg[127] = '\0';
	clogf_append_v2(dmsg, __FILE__, __LINE__, 0);
#endif

	if (NULL == this->_accept_routine) {
		/* F */
		return false;
	} else {
		this->_accept_routine->set_arg(this);
		return true;
	}
}

int xsocket::OnServerSocket::startAccept (void)
{
#if !defined(NO_C_LOGFILE) && defined(ENABLE_SOCK_DEBUG)
	/* trace */ char dmsg[128];
	snprintf(dmsg, 127, "func: %s", __func__); dmsg[127] = '\0';
	clogf_append_v2(dmsg, __FILE__, __LINE__, 0);
#endif

	if (NULL == this->_accept_routine) {
		return -xsocket::error::NO_SERVER;
	} else {
		return this->_accept_routine->start();
	}
}

size_t xsocket::OnServerSocket::instance_num = 0;
