/* ==========================================================================
 *
 * NAME xsocket/on_session.cxx
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

#include <xsocket/on_session.hxx>

#if defined(XSOCKET_LOGLEVEL)
#	include <x_logfile.hxx>
#endif

#include <xsocket/sock_core.hxx>
#include <xsocket/sock_session_routine.hxx>

/* ONE RUN FOR EACH SERVER */
xsocket::OnSession::OnSession (void)
{
#if defined(XSOCKET_LOGLEVEL) && (XSOCKET_LOGLEVEL >= 0x20)
	xlog::AppendV2(__func__, __FILE__, __LINE__, 0, XLOG_LEVEL_T);
#endif

	this->clis.clear();
}

/* ONE RUN FOR EACH SERVER */
xsocket::OnSession::~OnSession ()
{
#if defined(XSOCKET_LOGLEVEL) && (XSOCKET_LOGLEVEL >= 0x20)
	xlog::AppendV2(__func__, __FILE__, __LINE__, 0, XLOG_LEVEL_T);
#endif

	this->clis.clear();
}

int xsocket::OnSession::startSession (int cli_fd,
	const NetProtocol& client,
	int svr_fd,
	const NetProtocol& server)
{
#if defined(XSOCKET_LOGLEVEL) && (XSOCKET_LOGLEVEL >= 0x20)
	xlog::AppendV2(__func__, __FILE__, __LINE__, 0, XLOG_LEVEL_T);
#endif

	/* will be auto release start success */
	xsocket::core::internal::SockSessionRoutine * ass
		= new xsocket::core::internal::SockSessionRoutine(cli_fd, client,
		svr_fd, server, this);

	int ret = ass->start();
	if (ret < 0) {
		delete ass;
		ass = NULL;
	} else {
		this->clis.push_back(cli_fd);
		this->_should_exit = false;
	}
	/* else success: will auto release */

	return ret;
}

void xsocket::OnSession::closeAllClis (void)
{
#if defined(XSOCKET_LOGLEVEL) && (XSOCKET_LOGLEVEL >= 0x20)
	xlog::AppendV2(__func__, __FILE__, __LINE__, 0, XLOG_LEVEL_T);
#endif

	while (this->clis.size() > 0) {
		int fd = this->clis.front();
		xsocket::core::ShutdownSocket(fd, xsocket::ShutdownHow::RDWR);
		xsocket::core::CloseSocket(fd);
		this->clis.pop_front();
	}
}
