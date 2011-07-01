#!/bin/sh

CFLAGS="-O2 -Wall -g"
MAKEFLAGS="-j4"

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
		flags="--enable-g-main-loop"
		;;
	*)
		flags=""
		;;
	esac
	[ $no_autogen = 1 ] || (cd "$e" && ./autogen.sh "$flags") || exit 1
	(cd "$e" && make "$MAKEFLAGS" && sudo make install && sudo ldconfig) || exit 1
	echo
	echo "Built $e"
	echo
done
