#!/bin/sh
# Run this to generate all the initial makefiles, etc.

echo "Boostrapping StarDict dictionary..."

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

PKG_NAME="stardict"
REQUIRED_AUTOMAKE_VERSION=1.9

(test -f $srcdir/configure.ac \
  && test -f $srcdir/ChangeLog \
  && test -d $srcdir/src) || {
    echo -n "**Error**: Directory "\`$srcdir\'" does not look like the"
    echo " top-level stardict directory"
    exit 1
}


which mate-autogen || {
    echo "You need to install mate-common package"
    exit 1
}
USE_GNOME2_MACROS=1 NOCONFIGURE=yes . mate-autogen "$@"
