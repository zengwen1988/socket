/* ===========================================================================
 *
 * NAME
 *   func/usleep.h
 *
 * DESC
 *   Function usleep
 *
 * CREATED
 *   2015-01-11 00:52:33
 *
 * SUPPORT / TESTED COMPILER / IDE
 *    - ...gcc / g++
 *    - VC6
 *
 * V
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


#if !defined(POSIX_FUNCUSLEEP_H__)
#define POSIX_FUNCUSLEEP_H__ (1)


#if defined(MINGW32) || defined(_WIN32)
# if defined(USE_UWINDOWS_H) /* upper */
#   include <Windows.h>
# else
#   include <windows.h>
# endif /* defined(USE_UWINDOWS_H) */
# define usleep(__1_msw_us) do { \
	Sleep((unsigned long)(__1_msw_us / 1e3)); \
} while (0)
#else
# include <unistd.h>
#endif /* defined(MINGW32) || defined(_WIN32) */


#endif /* POSIX_FUNCUSLEEP_H__ */
