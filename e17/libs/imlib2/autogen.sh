#! /bin/sh

if [ "$USER" = "root" ]; then
  echo "You cannot do this as "$USER" please use a normal user account"
  exit
fi

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

THEDIR="`pwd`"
cd "$srcdir"
DIE=0

set -x
autoheader
libtoolize --ltdl --force --copy
aclocal
automake --foreign --add-missing
autoconf

if test -z "$*"; then
        echo "I am going to run ./configure with no arguments - if you wish "
        echo "to pass any to it, please specify them on the $0 command line."
fi

cd "$THEDIR"

$srcdir/configure "$@"

set +x

echo "making loader dir "$HOME"/.loaders/image"
mkdir $HOME"/.loaders"
mkdir $HOME"/.loaders/image"
echo "Now type:"
echo
echo "make"
echo "make install"
echo
echo "to compile and install the loaders for imlib2 demo."
echo "please ensure after you type make install there is a png.so and "
echo "jpeg.so in "$HOME"/.loaders/image"
echo "ignore the errors make install emits when make install exits - they are"
echo "ok to have."
echo "also make sure you run imlib2 from THIS directory - ie:"
echo 
echo "./imlib2"
echo
echo "have fun."

