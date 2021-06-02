/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2013  Free Software Foundation, Inc.
 *
 *  GRUB is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  GRUB is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GRUB.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GRUB_WINDOWS_UTIL_HEADER
#define GRUB_WINDOWS_UTIL_HEADER	1

#include <windows.h>

#if defined (__MINGW32__) && !defined (__MINGW64__)

/* 32 bit on Mingw-w64 already redefines them if _FILE_OFFSET_BITS=64 */
#ifndef _W64
#define fseeko fseeko64
#define ftello ftello64
#endif

#endif

char *
grub_util_tchar_to_utf8 (LPCTSTR in);

LPTSTR
grub_util_utf8_to_tchar (const char *in);

#endif
