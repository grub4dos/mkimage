/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 1999,2000,2001,2002,2003,2004,2006,2007,2008,2009,2010,2011,2012,2013  Free Software Foundation, Inc.
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

#include <config.h>
#include <config-util.h>

#include <grub/emu/config.h>
#include <grub/util/install.h>
#include <grub/util/misc.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>

#include <grub/util/windows.h>
#include <grub/charset.h>

#include <windows.h>
#include <winioctl.h>
#include <wincrypt.h>

static int allow_fd_syncs = 1;

#ifdef __MINGW32__

FILE *
grub_util_fopen (const char *path, const char *mode)
{
  LPTSTR tpath;
  FILE *ret;
  tpath = grub_util_get_windows_path (path);
#if SIZEOF_TCHAR == 1
  ret = fopen (tpath, tmode);
#else
  LPTSTR tmode;
  tmode = grub_util_utf8_to_tchar (mode);
  ret = _wfopen (tpath, tmode);
  free (tmode);
#endif
  free (tpath);
  return ret;
}

int
grub_util_file_sync (FILE *f)
{
  HANDLE hnd;

  if (fflush (f) != 0)
    {
      grub_util_info ("fflush err %x", (int) GetLastError ());
      return -1;
    }
  if (!allow_fd_syncs)
    return 0;
  hnd = (HANDLE) _get_osfhandle (fileno (f));
  if (!FlushFileBuffers (hnd))
    {
      grub_util_info ("flush err %x", (int) GetLastError ());
      return -1;
    }
  return 0;
}

#else

void
grub_util_file_sync (FILE *f)
{
  fflush (f);
  if (!allow_fd_syncs)
    return;
  fsync (fileno (f));
}

FILE *
grub_util_fopen (const char *path, const char *mode)
{
  return fopen (path, mode);
}

#endif
