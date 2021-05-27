#! /usr/bin/env bash

set -e

if [ ! -e grub-core/lib/gnulib/stdlib.in.h ]; then
  echo "Gnulib not yet bootstrapped; run ./bootstrap instead." >&2
  exit 1
fi

# Set ${PYTHON} to plain 'python' if not set already
: ${PYTHON:=python}

export LC_COLLATE=C
unset LC_ALL

find . -iname '*.[ch]' ! -ipath './build-aux/*' ! -ipath './gnulib/*' ! -ipath './grub-core/lib/gnulib/*' |sort > po/POTFILES.in
find util -iname '*.in' ! -name Makefile.in  |sort > po/POTFILES-shell.in

echo "Generating Automake input..."

UTIL_DEFS='Makefile.util.def'

${PYTHON} gentpl.py $UTIL_DEFS > Makefile.util.am

echo "Saving timestamps..."
echo timestamp > stamp-h.in

if [ -z "$FROM_BOOTSTRAP" ]; then
  # Unaided autoreconf is likely to install older versions of many files
  # than the ones provided by Gnulib, but in most cases this won't matter
  # very much.  This mode is provided so that you can run ./autogen.sh to
  # regenerate the GRUB build system in an unpacked release tarball (perhaps
  # after patching it), even on systems that don't have access to
  # gnulib.git.
  echo "Running autoreconf..."
  cp -a INSTALL INSTALL.grub
  autoreconf -vif
  mv INSTALL.grub INSTALL
fi

exit 0
