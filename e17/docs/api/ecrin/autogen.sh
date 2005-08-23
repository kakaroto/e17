#!/bin/sh
#
# - Bootstrap script -
#
# Copyright(C) 2005 Vincent Torri <torri@iecn.u-nancy.fr>
#
# This file builds the configure script and copies all needed files
# provided by automake/libtoolize
#
# $Id$


##############################################################################
# Helper functions
##############################################################################

set_package_freebsd()
{
# Check if the package exists (by checking the version),
# and set the variable PACKAGE to the package name
#
# argument 1: name of the package
# argument 2: major version
# argument 3: minimum minor version
# argument 4: set of minor versions

    NAME=$1
    MAJOR=$2
    MINOR=$3
    SET=$4

    PACKAGE=""
    for minor in $SET
    do
      name=$NAME$MAJOR$minor
      $name --version 1>/dev/null 2>&1
      if test "$?" -eq "0" ; then
	  PACKAGE=$name
      fi
    done
    if test "x$PACKAGE" = "x" ; then
	echo ""
	echo "Error: $NAME >= $MAJOR.$MINOR required"
	exit 127
    else
	echo "     + $NAME: $PACKAGE"
    fi
}

set_package_openbsd()
{
# Check if the package exists (by checking the version),
# and set the variable PACKAGE to the package name
#
# argument 1: name of the package
# argument 2: major version
# argument 3: minimum minor version
# argument 4: set of minor versions

    NAME=$1
    MAJOR=$2
    MINOR=$3
    SET=$4

    PACKAGE=""
    for minor in $SET
    do
      name=$NAME-$MAJOR.$minor
      $name --version 1>/dev/null 2>&1
      if test "$?" -eq "0" ; then
	  PACKAGE=$name
      fi
    done
    if test "x$PACKAGE" = "x" ; then
	echo ""
	echo "Error: $NAME >= $MAJOR.$MINOR required"
	exit 127
    else
	echo "     + $NAME: $PACKAGE"
    fi
}

set_package_linux()
{
# Check if the package exists (by checking the version),
# and set the variable PACKAGE to the package name
#
# argument 1: name of the package
# argument 2: minimum major version
# argument 3: minimum minor version

    NAME=$1
    MAJOR=$2
    MINOR=$3

    VER=`$NAME --version | head -1 | sed 's/'^[^0-9]*'/''/'`
    MAJORVER=`echo $VER | cut -f1 -d'.'`
    MINORVER=`echo $VER | cut -f2 -d'.'`
    if [ "$MAJORVER" -lt "$MAJOR" ]; then
	echo ""
	echo "Error: $NAME >= $MAJOR.$MINOR required (found $VER)"
	exit 127
    fi
    if [ "$MINORVER" -lt "$MINOR" ]; then
	echo ""
	echo "Error: $NAME >= $MAJOR.$MINOR required (found $VER)"
	exit 127
    fi
    echo "     + $NAME: $NAME $MAJORVER.$MINORVER"
}

##############################################################################
# Detect the right autoconf script
##############################################################################

# Check for the OS. They all have specific versioning of the autotools
OS=`uname -s`

echo " + Checking autotools versions"
echo "     + OS: $OS"

AUTOCONF=""
AC_MAJOR=2
AC_MINOR=54

ACLOCAL=""
AL_MAJOR=1
AL_MINOR=7

AUTOMAKE=""
AM_MAJOR=1
AM_MINOR=7

LIBTOOLIZE=""
LT_MAJOR=1
LT_MINOR=4

case $OS in
    FreeBSD)
	set_package_freebsd "autoconf" "$AC_MAJOR" "$AC_MINOR" "54 55 56 57 58 59"
	AUTOCONF=$PACKAGE
	set_package_freebsd "aclocal" "$AL_MAJOR" "$AL_MINOR" "7 8 9"
	ACLOCAL=$PACKAGE
	set_package_freebsd "automake" "$AM_MAJOR" "$AM_MINOR" "7 8 9"
	AUTOMAKE=$PACKAGE
	set_package_freebsd "libtoolize" "$LT_MAJOR" "$LT_MINOR" "4 5"
	LIBTOOLIZE=$PACKAGE
	;;
    OpenBSD)
	set_package_openbsd "autoconf" "$AC_MAJOR" "$AC_MINOR" "54 55 56 57 58 59"
	AUTOCONF=$PACKAGE
	set_package_openbsd "aclocal" "$AL_MAJOR" "$AL_MINOR" "7 8 9"
	ACLOCAL=$PACKAGE
	set_package_openbsd "automake" "$AM_MAJOR" "$AM_MINOR" "7 8 9"
	AUTOMAKE=$PACKAGE
	set_package_openbsd "libtoolize" "$LT_MAJOR" "$LT_MINOR" "4 5"
	LIBTOOLIZE=$PACKAGE
	;;
    Linux|Darwin)
	set_package_linux "autoconf" "$AC_MAJOR" "$AC_MINOR"
	AUTOCONF="autoconf"
	set_package_linux "aclocal" "$AL_MAJOR" "$AL_MINOR"
	ACLOCAL="aclocal"
	set_package_linux "automake" "$AM_MAJOR" "$AM_MINOR"
	AUTOMAKE="automake"
	set_package_linux "libtoolize" "$LT_MAJOR" "$LT_MINOR"
	LIBTOOLIZE="libtoolize"
	;;
    *)
	echo "Operating system not supported."
	echo "Send a mail to <torri@iecn.u-nancy.fr>"
	;;
esac

##############################################################################
# Bootstrap the configure script
##############################################################################

echo " + Creating ./configure"
$ACLOCAL -I ./m4
$AUTOCONF

echo " + Copying files provided by libtool"
$LIBTOOLIZE -f -c

echo " + Copying files provided by automake"
$AUTOMAKE -c -a

echo " + Removing files that are not needed"
rm -rf autom4*
