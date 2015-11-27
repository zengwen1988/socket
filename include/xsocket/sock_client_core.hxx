/* ==========================================================================
 *
 * NAME include/xsocket/sock_client_core.hxx
 *   Client socket (only) core here (header)
 *
 * DESC
 *   - This project all (socket related only) on STL and NO C++ 11
 *   - Funcs xsocket::core ...
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

#if !defined (XSOCKET_SOCK_SOCKCLIENTCORE_HXX__)
#define XSOCKET_SOCK_SOCKCLIENTCORE_HXX__ (1)

#include <posix/thread/simple_thread.hxx> /* pthreadx::SimpleThread */

#include <xsocket/basic_sock_type.hxx> /* type .. */

/* NOT INLCUDES IN THIS FILE */
namespace xsocket { class SockStartConnParams; }
namespace xsocket { class SockClientRecviveParams; }

/*extension of xsocket::core::internal */
namespace xsocket { namespace core { namespace internal {

/* class xsocket::core::internal::ConnectToServerBySockfd */
class ConnectToServerBySockfd: public pthreadx::SimpleThread {
public:
ConnectToServerBySockfd (void) { this->_ver = 0x00000000; }
protected:
	/* override */
	virtual void * run(void *);

public:
	inline void set_ver (uint32_t v) { this->_ver = v; }
	inline uint32_t ver (void) const { return this->_ver; }

private:
	uint32_t _ver;
};

/* class xsocket::core::internal::ClientRecevier */
class ClientRecevier: public pthreadx::SimpleThread {
protected:
	/* override */
	virtual void * run(void *);
};

} } } /* namespace xsocket .. */


/* extension of xsocket::core */
namespace xsocket { namespace core {

/* class xsocket::ClientHelper */
class ClientHelper {
/*
 * NAME class func xsocket::core::ClientHelper::startConnectBySockfd
 *   - To start to connect to target sockfd (server)
 *   - BETTER(public here) used by xsocket::SockClientHelper only
 */
public:
	static int startConnectBySockfd(int sockfd,
		xsocket::SockStartConnParams * params);
	/* TODO: add stop */
	static int startConnectBySockfdV2(xsocket::SockStartConnParams * params);
	static int startReceiveFromPeer(
		xsocket::SockClientRecviveParams * params);
};

} } /* namespace xsocket .. */

#endif /* XSOCKET_SOCK_SOCKCLIENTCORE_HXX__ */
