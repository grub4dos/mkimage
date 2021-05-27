AUTOMAKE_OPTIONS = subdir-objects -Wno-portability

DEPDIR=.deps-core

include $(top_srcdir)/conf/Makefile.common

CC=$(TARGET_CC)
CPP=$(TARGET_CC)
CCAS=$(TARGET_CC)
RANLIB=$(TARGET_RANLIB)
STRIP=$(TARGET_STRIP)

AM_CFLAGS = $(TARGET_CFLAGS)
AM_LDFLAGS = $(TARGET_LDFLAGS)
AM_CPPFLAGS = $(TARGET_CPPFLAGS) $(CPPFLAGS_DEFAULT)
AM_CCASFLAGS = $(TARGET_CCASFLAGS) $(CCASFLAGS_DEFAULT)

CFLAGS_PROGRAM += $(CFLAGS_PLATFORM)
LDFLAGS_PROGRAM += $(LDFLAGS_PLATFORM)
CPPFLAGS_PROGRAM += $(CPPFLAGS_PLATFORM)
CCASFLAGS_PROGRAM += $(CCASFLAGS_PLATFORM)

CFLAGS_LIBRARY += $(CFLAGS_PLATFORM) -fno-builtin
CPPFLAGS_LIBRARY += $(CPPFLAGS_PLATFORM)
CCASFLAGS_LIBRARY += $(CCASFLAGS_PLATFORM)

KERNEL_HEADER_FILES += $(top_srcdir)/include/grub/cache.h
KERNEL_HEADER_FILES += $(top_srcdir)/include/grub/command.h
KERNEL_HEADER_FILES += $(top_srcdir)/include/grub/device.h
KERNEL_HEADER_FILES += $(top_srcdir)/include/grub/disk.h
KERNEL_HEADER_FILES += $(top_srcdir)/include/grub/env.h
KERNEL_HEADER_FILES += $(top_srcdir)/include/grub/env_private.h
KERNEL_HEADER_FILES += $(top_srcdir)/include/grub/err.h
KERNEL_HEADER_FILES += $(top_srcdir)/include/grub/file.h
KERNEL_HEADER_FILES += $(top_srcdir)/include/grub/fs.h
KERNEL_HEADER_FILES += $(top_srcdir)/include/grub/i18n.h
KERNEL_HEADER_FILES += $(top_srcdir)/include/grub/kernel.h
KERNEL_HEADER_FILES += $(top_srcdir)/include/grub/list.h
KERNEL_HEADER_FILES += $(top_srcdir)/include/grub/misc.h

KERNEL_HEADER_FILES += $(top_srcdir)/include/grub/mm.h
KERNEL_HEADER_FILES += $(top_srcdir)/include/grub/partition.h
KERNEL_HEADER_FILES += $(top_srcdir)/include/grub/time.h
KERNEL_HEADER_FILES += $(top_srcdir)/include/grub/net.h
KERNEL_HEADER_FILES += $(top_srcdir)/include/grub/memory.h

if COND_HAVE_ASM_USCORE
ASM_PREFIX=_
else
ASM_PREFIX=
endif
