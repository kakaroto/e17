#! /bin/sh
# Run this to generate all the initial makefiles, etc.

# Stolen from the GNU Midnight Commander. Customized for giFTcurs. Stolen from
# giFTcurs. Customized for giFT. Stolen from giFT. Customized for Embrace.

# Make it possible to specify path in the environment
: ${AUTOCONF=autoconf}
: ${AUTOHEADER=autoheader}
: ${AUTOMAKE=automake}
: ${ACLOCAL=aclocal}
: ${LIBTOOLIZE=libtoolize}

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

(
cd $srcdir

# The autoconf cache (version after 2.52) is not reliable yet.
rm -rf autom4te.cache

rm -f aclocal.m4
ACLOCAL_INCLUDES="-I m4"

# Some old version of GNU build tools fail to set error codes.
# Check that they generate some of the files they should.

echo "Running $ACLOCAL..."
$ACLOCAL $ACLOCAL_INCLUDES $ACLOCAL_FLAGS || exit 1
test -f aclocal.m4 || \
  { echo "aclocal failed to generate aclocal.m4" 2>&1; exit 1; }

echo "Running $AUTOHEADER..."
$AUTOHEADER || exit 1
test -f config.h.in || \
  { echo "autoheader failed to generate config.h.in" 2>&1; exit 1; }

echo "Running $AUTOCONF..."
$AUTOCONF || exit 1
test -f configure || \
  { echo "autoconf failed to generate configure" 2>&1; exit 1; }

# hack hack hack...i hate autotools.
echo "Running $LIBTOOLIZE --automake..."
$LIBTOOLIZE --automake || exit 1
test -f ltmain.sh || \
  { echo "libtoolize failed to generate ltmain.sh" 2>&1; exit 1; }

# Workaround for Automake 1.5 to ensure that depcomp is distributed.
echo "Running $AUTOMAKE..."
$AUTOMAKE -c -a src/Makefile || exit 1
$AUTOMAKE -c -a || exit 1
test -f Makefile.in || \
  { echo "automake failed to generate Makefile.in" 2>&1; exit 1; }

) || exit 1

echo
echo "Type './configure' to configure embrace."
echo
