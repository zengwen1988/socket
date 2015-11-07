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
#include <xsocket/sock_session_routine.hxx>

#if defined(XSOCKET_LOGLEVEL)
#	include <x_logfile.hxx>
#endif

xsocket::OnSession::OnSession (void)
{
	this->session_routine = NULL;
}

xsocket::OnSession::~OnSession ()
{
	this->session_routine = NULL;
}

int xsocket::OnSession::startSession (int cli_fd, const NetProtocol& client,
	const NetProtocol& server)
{
	if (NULL != this->session_routine) {
		return 0;
	}

	this->session_routine
		= new xsocket::core::internal::SockSessionRoutine(cli_fd, client,
		server);

	int ret = this->session_routine->start();
	if (ret < 0) {
		delete this->session_routine;
		this->session_routine = NULL;
	}
	/* else success: will auto release */

	return ret;
}
