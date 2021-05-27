AUTOMAKE_OPTIONS = subdir-objects -Wno-portability

DEPDIR = .deps-util
SUBDIRS = grub-core/lib/gnulib .

SUBDIRS += po

include $(top_srcdir)/conf/Makefile.common
include $(top_srcdir)/conf/Makefile.extra-dist

AM_CFLAGS = $(HOST_CFLAGS)
AM_LDFLAGS = $(HOST_LDFLAGS)
AM_CPPFLAGS = $(HOST_CPPFLAGS) $(CPPFLAGS_DEFAULT)
AM_CCASFLAGS = $(HOST_CCASFLAGS) $(CCASFLAGS_DEFAULT)

ACLOCAL_AMFLAGS = -I m4

CFLAGS_PROGRAM += $(CFLAGS_GNULIB)
LDFLAGS_PROGRAM += $(LDFLAGS_GNULIB)
CPPFLAGS_PROGRAM += $(CPPFLAGS_GNULIB)
CCASFLAGS_PROGRAM += $(CCASFLAGS_GNULIB)

include $(srcdir)/Makefile.util.am

# For libgrub.a
libgrub.pp: config-util.h $(libgrubmods_a_SOURCES) $(libgrubkern_a_SOURCES)
	$(CPP) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(libgrubmods_a_CPPFLAGS) $(libgrubkern_a_CPPFLAGS) $(CPPFLAGS) \
	  -D'GRUB_MOD_INIT(x)=@MARKER@x@' $^ > $@ || (rm -f $@; exit 1)
CLEANFILES += libgrub.pp

libgrub_a_init.lst: libgrub.pp
	cat $< | grep '@MARKER@' | sed 's/@MARKER@\(.*\)@/\1/g' | sort -u > $@ || (rm -f $@; exit 1)
CLEANFILES += libgrub_a_init.lst

libgrub_a_init.c: libgrub_a_init.lst $(top_srcdir)/geninit.sh
	sh $(top_srcdir)/geninit.sh `cat $<` > $@ || (rm -f $@; exit 1)
CLEANFILES += libgrub_a_init.c

# Install config.h into platformdir
nodist_platform_HEADERS = config.h

changelog_start_date = 2015-01-23
gitlog_to_changelog = $(top_srcdir)/build-aux/gitlog-to-changelog

ChangeLog: FORCE
	if test -d $(top_srcdir)/.git; then \
		$(gitlog_to_changelog) --srcdir=$(top_srcdir) --since=$(changelog_start_date) > '$@.tmp'; \
		rm -f '$@'; mv '$@.tmp' '$@'; \
	else \
		touch $@; \
	fi

EXTRA_DIST += ChangeLog ChangeLog-2015
