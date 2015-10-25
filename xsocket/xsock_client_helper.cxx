/* ==========================================================================
 *
 * NAME xsocket/sock_client_helper.cc
 *   Class xsocket::SockClientHelper
 *
 * DESC
 *   - This project all (socket related only) on STL and NO C++ 11
 *   - class xsocket::SockClientHelper
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

#include <xsocket/sock_client_helper.hxx>

#include <unistd.h> /* close */
#include <pthread.h> /* pthread_create */
#include <errno.h>

#if !defined(NO_C_LOGFILE)
#	include <c_logfile.h>
#endif

#include <xsocket/sock_core.hxx>
#include <xsocket/sock_client_core.hxx>
#include <xsocket/sock_client_params.hxx>

int xsocket::SockClientHelper::startConnectByHost (
	const xsocket::NetProtocol& host,
	uint32_t timeout, /* second */
	xsocket::OnClientSocket * on_socket/* client socket callback */)
{
	int ret;
	int sockfd;
	xsocket::SockStartConnParams * params = NULL;

#if !defined(NO_C_LOGFILE) && defined(ENABLE_SOCK_DEBUG)
	clogf_append_v2("xsocket::SockClientHelper::startConnectByHost",
		__FILE__, __LINE__, 0);
#endif

	/* check */
	if (NULL == on_socket) {
		ret = -xsocket::error::SOCKARG_INVAL;
#if 	!defined(NO_C_LOGFILE)
		clogf_append_v2("no callback", __FILE__, __LINE__, ret);
#endif
		return ret;
	}

	/* FIXME vaid ip first */

	sockfd = xsocket::core::GetSockfdByHost(host);

	if (sockfd < 0) {
		ret = sockfd;
		goto conn_fail;
	} else {
		ret = 0;
	}

	params = new SockStartConnParams(timeout, host, on_socket);
	ret = xsocket::core::ClientHelper::startConnectBySockfd(sockfd, params);

	if (0 != ret) {
		clogf_append_v2("start connect 2 fail", __FILE__, __LINE__, ret);

		goto conn2fail;
	} else {
		/* success */
		clogf_append("start connect success");
		return 0;
	}

conn2fail:
	xsocket::core::CloseSocket(sockfd);
	sockfd = -1;

conn_fail:
	return ret;
}
