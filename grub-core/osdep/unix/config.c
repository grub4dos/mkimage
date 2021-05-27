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

#include <grub/emu/hostdisk.h>
#include <grub/emu/exec.h>
#include <grub/emu/config.h>
#include <grub/util/install.h>
#include <grub/util/misc.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>

const char *
grub_util_get_pkgdatadir (void)
{
  const char *ret = getenv ("PWD");
  if (ret)
    return ret;
  return ".";
}

const char *
grub_util_get_pkglibdir (void)
{
  const char *ret = getenv ("PWD");
  if (ret)
    return ret;
  return ".";
}

const char *
grub_util_get_localedir (void)
{
  return "./locale";
}
