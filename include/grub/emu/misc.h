/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 2010  Free Software Foundation, Inc.
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

#ifndef GRUB_EMU_MISC_H
#define GRUB_EMU_MISC_H 1

#include <config.h>
#include <stdarg.h>

#include <stdio.h>

#include <grub/compiler.h>
#include <grub/symbol.h>
#include <grub/types.h>
#include <grub/misc.h>
#include <grub/util/misc.h>

extern int verbosity;
extern const char *program_name;

void grub_init_all (void);
void grub_fini_all (void);

#define GRUB_HOST_PRIuLONG_LONG "llu"
#define GRUB_HOST_PRIxLONG_LONG "llx"

void * EXPORT_FUNC(xcalloc) (grub_size_t nmemb, grub_size_t size) WARN_UNUSED_RESULT;
void * EXPORT_FUNC(xmalloc) (grub_size_t size) WARN_UNUSED_RESULT;
void * EXPORT_FUNC(xrealloc) (void *ptr, grub_size_t size) WARN_UNUSED_RESULT;
char * EXPORT_FUNC(xstrdup) (const char *str) WARN_UNUSED_RESULT;
char * EXPORT_FUNC(xasprintf) (const char *fmt, ...) __attribute__ ((format (GNU_PRINTF, 1, 2))) WARN_UNUSED_RESULT;

void EXPORT_FUNC(grub_util_warn) (const char *fmt, ...) __attribute__ ((format (GNU_PRINTF, 1, 2)));
void EXPORT_FUNC(grub_util_info) (const char *fmt, ...) __attribute__ ((format (GNU_PRINTF, 1, 2)));
void EXPORT_FUNC(grub_util_error) (const char *fmt, ...) __attribute__ ((format (GNU_PRINTF, 1, 2), noreturn));

FILE *
grub_util_fopen (const char *path, const char *mode);

int grub_util_file_sync (FILE *f);

#endif /* GRUB_EMU_MISC_H */
