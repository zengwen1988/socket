/* ==========================================================================
 *
 * NAME include/basic_sock_type.hxx
 *   Basic socket type
 *
 * DESC
 *   - This project all (socket related only) on STL and NO C++ 11
 *   - class xsocket::Sock
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
 *   This file is part of XSOCKET.
 *   XSOCKET
 *   is free software: you can redistribute it
 *   and/or modify it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation, either version 3 of the
 *   License, or (at your option) any later version.
 *   XSOCKET
 *   is distributed in the hope that it will be
 *   useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with XSOCKET.
 *   If not, see <http://www.gnu.org/licenses/>.
 *
 * ==========================================================================
 */

#if !defined(XSOCKET_BASIC_SOCK_TYPE_HXX__)
#define XSOCKET_BASIC_SOCK_TYPE_HXX__ (1)

#include <errno.h>

#if !defined(WIN32)
#	include <stdint.h>
#else
#	include <posix/mingw/stdint.h>
#endif

#if !defined(WIN32)
#	include <netinet/in.h> /* sockaddr_in */
#else
#	pragma comment(lib, "ws2_32.lib")
// #	include <WinSock2.h> /* sockaddr_in */
#endif

#define XSOCKET_MAXIPLEN (28)

namespace xsocket {

struct NetProtocol {
	/* IP and PORT (data) only */
	/* host order */
	char ip[XSOCKET_MAXIPLEN + 1 + 3];
	/* host order */
	uint16_t port;
	bool is_ipv6;
};

} /* namespace xsocket */

namespace xsocket {

struct SockOnConnnect {
	/* For client' "onConnect" callback */
	int code;/* onConnect code */
	int sockfd;/* This sockfd connect to below "info" */
	xsocket::NetProtocol info;
};

struct SockDidFinish {
	/* For client and server */
	int code;/* finish code */
	int fd;/* who with this sockfd will finish */
	int fd_peer;
	xsocket::NetProtocol info;/* who with this info will finish */
	xsocket::NetProtocol peer;
};

struct SockRecved {
	/* For client and server */
	uint8_t * data;
	int count;
	/* self fd */
	int fd;
	int fd_peer;
	int sockfd;
	/* self */
	xsocket::NetProtocol info;
	xsocket::NetProtocol peer;
};

} /* namespace xsocket */

namespace xsocket {

namespace error { enum Type {
	SUCCESS = 0,
	SOCKFD_INVAL = 10000, /* invalid socket fd */
	NO_CLIENT_CB, /* no client callback */
	SOCKARG_INVAL, /* socket argument invalid */
	START_CONN_THREAD_FAIL,
	/* server */
	NO_SERVER,
	NO_SERVER_CB, /* no server callback */
	/* WIN32 only */
	SOCK_INIT_FAIL,
}; }

namespace status { enum Type {
	UNKNOWN = 0,
	CONNECTING = 1,
	CONNECTED,
	DISCONNECTED,
}; }


/* SHUT_RD, SHUT_WR, SHUT_RDWR have the value 0, 1, 2 */
namespace ShutdownHow { enum Type {
#if !defined(WIN32)
	RD = 0,
	WR = 1,
	RDWR = 2,
#else
	/*
	RD = SD_RECEIVE,
	WR = SD_SEND,
	RDWR = SD_BOTH,
	*/
	RD = 0x01,
	WR = 0x02,
	RDWR = 0x03,
#endif
}; }

} /* namespace xsocket */


#if defined(WIN32)
#if 0
struct in_addr {
	union {
		struct { unsigned char s_b1,s_b2,s_b3,s_b4; } S_un_b;
		struct { unsigned short s_w1,s_w2; } S_un_w;
		unsigned long S_addr;
	} S_un;
#define s_addr  S_un.S_addr
/* can be used for most tcp & ip code */
#define s_host  S_un.S_un_b.s_b2
/* host on imp */
#define s_net   S_un.S_un_b.s_b1
/* network */
#define s_imp   S_un.S_un_w.s_w2
/* imp */
#define s_impno S_un.S_un_b.s_b4
/* imp # */
#define s_lh    S_un.S_un_b.s_b3
/* logical host */
};

/*
 * WinSock2.h
 * Socket address, internet style.
 */
struct sockaddr_in {
	short sin_family;
	unsigned short sin_port;
	struct in_addr sin_addr;
	char sin_zero[8];
};
#endif
// #include <winsock.h>
#endif

#endif /* XSOCKET_BASIC_SOCK_TYPE_HXX__ */
