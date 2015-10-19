/*
 * NAME
 *   UNIXOnSocket.cxx - socket callback
 *
 * DESC
 *   - funcs all in c++
 *   - support: linux
 *
 * CC
 *   ...gcc / g++
 *
 * CREATED
 *   2015-10-19 1 292 02:04:45 -0400 by yuiwong
 *
 * V
 *   - 1.0.0.0 - 20151019
 *   first version
 *
 * LICENSE
 *   LGPLv3
 *	 This file is part of socket.
 *	 socket is free software:
 *	 you can redistribute it and/or modify it under the terms of the GNU
 *	 General Public License as published by the Free Software Foundation,
 *	 either version 3 of the License, or (at your option) any later version.
 *   socket is distributed in the hope that it will be
 *	 useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	 GNU General Public License for more details.
 *	 You should have received a copy of the GNU General Public License
 *	 along with socket.
 *	 If not, see <http://www.gnu.org/licenses/>.
 */


#include <unix/OnSocket.h>

/* override this */
int UNIXOnSocket::onConnect (sock_on_conn_t) { return 0; }
/* override this */
int UNIXOnSocket::willFinish(sock_will_finish_t) { return 0; }
/* override this */
int UNIXOnSocket::onReceived (int, const uint8_t *, size_t)
{ return 0; }
/* override this */
bool UNIXOnSocket::shouldTeminateRecv(int /* sockfd */) { return false; }
