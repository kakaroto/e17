#!/bin/sh
# Run this to generate all the initial makefiles, etc.

PKG_NAME="enlightenment"

srcdir=`dirname $0`
DIE=0

(autoconf --version) < /dev/null > /dev/null 2>&1 || {
        echo
        echo "You must have autoconf installed to compile Enlightenment."
        echo "Download the appropriate package for your distribution,"
        echo "or get the source tarball at ftp://ftp.gnu.org/pub/gnu/autoconf"
        DIE=1
}

(automake --version) < /dev/null > /dev/null 2>&1 || {
        echo
        echo "You must have automake installed to compile Enlightenment."
        echo "Get it at ftp://ftp.gnu.org/pub/gnu/automake"
        DIE=1
}

(gettext --version) < /dev/null > /dev/null 2>&1 || {
        echo
        echo "You must have gettext installed to compile Enlightenment."
        echo "Get ftp://ftp.gnu.org/pub/gnu/gettext"
        DIE=1
}

if test "$DIE" -eq 1; then
        exit 1
fi

(test -d src && test -d dox) || {
        echo "You must run this script in the top-level Enlightenment directory"
        exit 1
}

if test -z "$*"; then
        echo "I am going to run ./configure with no arguments - if you wish "
        echo "to pass any to it, please specify them on the $0 command line."
fi

echo "Generating configuration files for Enlightenment, please wait...."

#echo " gettextize -f"
#gettextize -f
echo "  setup-gettext.sh"
grep 'AM_GNU_GETTEXT_VERSION' configure.in >/dev/null 2>&1 || sh ./setup-gettext.sh --install
sh ./setup-gettext.sh
echo "  aclocal $ACLOCAL_FLAGS"
aclocal $ACLOCAL_FLAGS
echo "  autoheader"
autoheader
echo "  automake --add-missing"
automake --add-missing 
echo "  autoconf"
autoconf


#conf_flags="--enable-maintainer-mode --enable-compile-warnings"

if test x$NOCONFIGURE = x; then
  echo Running $srcdir/configure $conf_flags "$@" ...
  $srcdir/configure $conf_flags "$@" \
  && echo Now type \`make\' to compile $PKG_NAME
else
  echo Skipping configure process.
fi
