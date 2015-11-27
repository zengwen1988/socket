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

#if !defined(NO_X_LOGFILE)
#	include <x_logfile.hxx>
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

	xlog::AppendV2(__func__, __FILE__, __LINE__, 0);

	/* check */
	if (NULL == on_socket) {
		ret = -xsocket::error::SOCKARG_INVAL;
		xlog::AppendV2("no callback", __FILE__, __LINE__, ret, XLOG_LEVEL_E);
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
		xlog::AppendV2("start connect 2 fail", __FILE__, __LINE__, ret);

		goto conn2fail;
	} else {
		/* success */
		xlog::Append("start connect success");
		return 0;
	}

conn2fail:
	xsocket::core::CloseSocket(sockfd);
	sockfd = -1;

conn_fail:
	return ret;
}

int xsocket::SockClientHelper::startConnectByHostV2 (
	xsocket::ip_t * ip,
	xsocket::port_t * port,
	uint32_t timeout, /* second */
	xsocket::OnClientSocket * on_socket/* client socket callback */)
{
	int ret;
	xsocket::SockStartConnParams * params = NULL;

	xlog::AppendV2(__func__, __FILE__, __LINE__, 0, XLOG_LEVEL_T);
	/* check */
	if (NULL == on_socket) {
		ret = -xsocket::error::SOCKARG_INVAL;
		xlog::AppendV2("no callback", __FILE__, __LINE__, ret, XLOG_LEVEL_E);
		return ret;
	}

	params = new SockStartConnParams(timeout, ip, port, on_socket);
	ret = xsocket::core::ClientHelper::startConnectBySockfdV2(params);
	if (0 != ret) {
		xlog::AppendV2("start connect 2 fail", __FILE__, __LINE__, ret);
		goto end;
	} else {
		/* success */
		xlog::AppendV2("start connect success", __FILE__, __LINE__, 0,
			 XLOG_LEVEL_I);
		return 0;
	}

end:
	return ret;
}
