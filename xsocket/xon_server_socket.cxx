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

#include <x_logfile.hxx>

#include <xsocket/sock_server_accept_routine.hxx>
#include <xsocket/on_session.hxx>

/* ONE RUN FOR EACH SERVER */
xsocket::OnServerSocket::OnServerSocket (void)
{
	++instance_num;

#if defined(XSOCKET_LOGLEVEL) && (XSOCKET_LOGLEVEL >= 0x20)
	xlog::AppendV2(__func__, __FILE__, __LINE__, instance_num, XLOG_LEVEL_T);
#endif

	this->_on_session = NULL;
	this->_accept_routine
		= new xsocket::core::internal::SockServerAcceptRoutine();
	this->acceptPrepare();
}

/* ONE RUN FOR EACH SERVER */
xsocket::OnServerSocket::~OnServerSocket ()
{
	--instance_num;

#if defined(XSOCKET_LOGLEVEL) && (XSOCKET_LOGLEVEL >= 0x20)
	xlog::AppendV2(__func__, __FILE__, __LINE__, instance_num, XLOG_LEVEL_T);
#endif

	if (NULL != this->_on_session) {
#if		defined(XSOCKET_LOGLEVEL)
		xlog::AppendV2("i'll delete _on_session", __FILE__, __LINE__,
			0, XLOG_LEVEL_W);
#endif
		delete this->_on_session;
		this->_on_session = NULL;
	}

	if (NULL != this->_accept_routine) {
#if		defined(XSOCKET_LOGLEVEL)
		xlog::AppendV2("i'll delete _accept_routine", __FILE__, __LINE__,
			0, XLOG_LEVEL_W);
#endif
		delete this->_accept_routine;
		this->_accept_routine = NULL;
	}
}

bool xsocket::OnServerSocket::acceptPrepare (void)
{
#if !defined(NO_X_LOGFILE) && defined(ENABLE_SOCK_DEBUG)
	/* trace */ char dmsg[128];
	snprintf(dmsg, 127, "func: %s", __func__); dmsg[127] = '\0';
	xlog::AppendV2(dmsg, __FILE__, __LINE__, 0);
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
	xlog::AppendV2(__func__, __FILE__, __LINE__, 0, XLOG_LEVEL_T);

	if (NULL == this->_accept_routine) {
		return -xsocket::error::NO_SERVER;
	} else {
		return this->_accept_routine->start();
	}
}

int xsocket::OnServerSocket::instance_num = 0;
