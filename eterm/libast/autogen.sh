#!/bin/sh
# Run this to generate all the initial makefiles, etc.
# $Id$

DIE=0

echo "Generating configuration files for libast, please wait...."

(autoconf --version) < /dev/null > /dev/null 2>&1 || {
	echo
        echo "You must have autoconf installed to compile libast."
        echo "Download the appropriate package for your distribution,"
        echo "or get the source tarball at ftp://ftp.gnu.org/pub/gnu/"
        DIE=1
}

(libtool --version) < /dev/null > /dev/null 2>&1 || {
        echo
        echo "You must have libtool installed to compile libast."
        echo "Download the appropriate package for your distribution,"
        echo "or get the source tarball at ftp://ftp.gnu.org/pub/gnu/"
        DIE=1
}

(automake --version) < /dev/null > /dev/null 2>&1 || {
        echo
        echo "You must have automake installed to compile libast."
        echo "Download the appropriate package for your distribution,"
        echo "or get the source tarball at ftp://ftp.gnu.org/pub/gnu/"
        DIE=1
}

if test "$DIE" -eq 1; then
        exit 1
fi

(set -x && libtoolize -c -f)
(set -x && aclocal -I . $ACLOCAL_FLAGS)
(set -x && autoheader)
(set -x && automake -a -c)
(set -x && autoconf)

./configure "$@"
