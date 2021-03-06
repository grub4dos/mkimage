/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2002,2003,2005,2006,2007,2008,2009,2010  Free Software Foundation, Inc.
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

#ifndef GRUB_UTIL_MISC_HEADER
#define GRUB_UTIL_MISC_HEADER	1

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <setjmp.h>
#include <unistd.h>

#include <config.h>
#include <grub/types.h>
#include <grub/symbol.h>
#include <grub/emu/misc.h>

char *grub_util_get_path (const char *dir, const char *file);
size_t grub_util_get_image_size (const char *path);
char *grub_util_read_image (const char *path);
void grub_util_load_image (const char *path, char *buf);
void grub_util_write_image (const char *img, size_t size, FILE *out,
			    const char *name);
void grub_util_write_image_at (const void *img, size_t size, off_t offset,
			       FILE *out, const char *name);

char *grub_canonicalize_file_name (const char *path);

void grub_util_host_init (int *argc, char ***argv);

#endif /* ! GRUB_UTIL_MISC_HEADER */
