#!/bin/sh

# Ubuntu (11.04) setup
#
# apt-get install autotools-dev automake autopoint libtool zlib1g-dev \
#		libjpeg62-dev libfreetype6-dev libx11-dev subversion git \
#		libglib2.0-dev libxext-dev libxcursor-dev libudev-dev \
#		libcurl4-gnutls-dev libc-ares-dev liblua5.1-0-dev libpng12-dev \
#		libtiff4-dev libfontconfig1-dev libxcb-shape0-dev
#
# Evas fails to build on x86-64 due to this bug.
# https://bugs.launchpad.net/ubuntu/+source/libgcrypt11/+bug/751142
# Hack around it as follows as root:
# cd /lib/x86_64-linux-gnu/ ; ln -s /usr/lib/x86_64-linux-gnu/libgcrypt.la .
#

# some EFL libraries don't build with warnings by default...
CFLAGS="-O2 -Wall -g"

# how many CPUs?
if [ -f /proc/cpuinfo ]
then
	cpus="`grep ^processor /proc/cpuinfo | wc -l`"
fi
[ "$cpus" ] || cpus=4
MAKEFLAGS="-j$cpus"

no_autogen=0

while [ $# -ge 1 ]
do
	case "$1" in
	up)
		no_autogen=1
		;;
	esac
	shift
done

for e in eina eet evas ecore eeze embryo edje e_dbus efreet elementary e PROTO/libeweather
do
	echo
	echo "Building $e"
	echo
	case $e in
	evas)
		flags="--enable-gl-x11"
		;;
	ecore)
		flags="--enable-g-main-loop --enable-thread-safety"
		;;
	*)
		flags=""
		;;
	esac
	[ $no_autogen = 1 ] || (cd "$e" && rm -f config.cache && ./autogen.sh $flags) || exit 1
	(cd "$e" && make "$MAKEFLAGS" && sudo make install && sudo ldconfig) || exit 1
	echo
	echo "Built $e"
	echo
done
