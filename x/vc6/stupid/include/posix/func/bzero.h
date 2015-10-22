/* ===========================================================================
 *
 * NAME
 *   func/bzero.h
 *
 * DESC
 *   Function bzero
 *
 * CREATED
 *   2015-10-22
 *
 * SUPPORT / TESTED COMPILER / IDE
 *    - ...gcc / g++
 *    - VC6
 *
 * V
 *   TODO
 *
 * LICENSE
 *   LGPLv3
 *   This file is is free software: you can redistribute it
 *   and/or modify it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation, either version 3 of the
 *   License, or (at your option) any later version.
 *   This file
 *   is distributed in the hope that it will be
 *   useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *   You should have received a copy of the GNU General Public License along
 *   with this file.
 *   If not, see <http://www.gnu.org/licenses/>.
 *
 * ===========================================================================
 */


#if !defined(POSIX_FUNCBZERO_H__)
#define POSIX_FUNCBZERO_H__ (1)


#if defined(MINGW32) || defined(_WIN32)
# include <stdlib.h>
# define  bzero(__1_addr, __2_sz) (memset((__1_addr), 0, (__2_sz)))
#else
# include <string.h>
#endif /* defined(MINGW32) || defined(_WIN32) */


#endif /* POSIX_FUNCBZERO_H__ */
