#!/bin/sh

# Ubuntu (11.04) setup
#
deplist="autotools-dev automake autopoint libtool zlib1g-dev
	libjpeg62-dev libfreetype6-dev libx11-dev subversion git
	libglib2.0-dev libxext-dev libxcursor-dev libudev-dev
	libcurl4-gnutls-dev libc-ares-dev liblua5.1-0-dev libpng12-dev
	libtiff4-dev libfontconfig1-dev libxcb-shape0-dev
	libxrender-dev libgif-dev libglu1-mesa-dev mesa-common-dev
	libxpm-dev librsvg2-dev libfribidi-dev libpixman-1-dev
	libxcb-shm0-dev libxcb-image0-dev libxss-dev libxp-dev
	libxtst-dev graphviz libasound2-dev libpam0g-dev"

defpkgs="eina eet evas ecore eeze embryo edje e_dbus efreet PROTO/libeweather elementary e"
#
# Evas fails to build on x86-64 due to this bug.
# https://bugs.launchpad.net/ubuntu/+source/libgcrypt11/+bug/751142
# Hack around it as follows as root:
# cd /lib/x86_64-linux-gnu/ ; ln -s /usr/lib/x86_64-linux-gnu/libgcrypt.la .
#

# fail on errors
set -e
#set -x

do_build_and_install()
{
	local e flags

	e="$1"
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
}

do_build_deb()
{
	local e
	local pkgdir

	e="$1"
	echo "Building debian package for $e"
	rm -rf "$e"/debian
	mkdir "$e/debian"
	case "$e" in
	PROTO/libeweather)
		pkgdir="extras/libeweather"
		;;
	*)
		pkgdir="main/$e"
		;;
	esac
	cp -aR "packaging/debian/$pkgdir"/* "$e/debian/"
	(cd "$e" && dpkg-buildpackage -sa -rfakeroot -uc -us $MAKEFLAGS)
}

do_install_deb()
{
	local deb
	local e
	local epkg

	e="$1"
	echo "Installing debian package for $e"
	case "$e" in
	e_dbus)
		epkg="edbus"
		;;
	*)
		epkg="$e"
		;;
	esac
	deblist="`dcmd $epkg*.changes | grep "deb$"`"
	sudo dpkg -i $deblist
}

do_install_dependencies()
{
	sudo apt-get -y install $deplist
}

usage()
{
local scriptname
scriptname="`basename $0`"
cat <<EOF
Usage:
$scriptname [--debian] [--up] [--deps] [--help] [--force] [pkgs]

$scriptname builds and installs binaries from the E SVN.
It is written to be run from an SVN checkout or git svn clone.
When used from git svn, it can rebuild only packages that
have changed in git since the last rebuild.

Options:

  --deps    Install build dependencies (Ubuntu 11.04 only)
  --debian  Build and install debian packages
  --up      Update mode (avoid running autoconf & configure)
  --force   Rebuild everything, ignoring git trees
  --help    Show this message

Modules built by default are:
$defpkgs

The default mode of build is simple build and install.

EOF
exit 0
}

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
debian=0
force=0
install_deps=0

while [ $# -ge 1 ]
do
	case "$1" in
	--up)
		no_autogen=1
		;;
	--debian)
		debian=1
		;;
	--deps)
		install_deps=1
		;;
	--help)
		usage
		;;
	--force)
		force=1
		;;
	-*)
		echo "Unknown option $1" >&2
		exit 1
		;;
	*)
		what="$what $1"
		;;
	esac
	shift
done

[ $install_deps = 0 ] || do_install_dependencies

if [ -d .git ]
then
	tsdir=.git/ebuildtimestamps/
	mkdir -p "$tsdir"
fi

if [ ! "$what" ]
then
	what="$defpkgs"
fi

for e in $what
do
	if [ ! -d "$e" ]
	then
		echo "$e directory missing?"
		exit 1
	fi

	tsfile="`echo $e | sed -e 's/\//_/g'`"

	if [ $force = 0 -a -r "$tsdir/$tsfile" ]
	then
		old_sha=`cat "$tsdir/$tsfile" 2> /dev/null`
		new_sha=`git ls-tree -d HEAD "$e" | cut -f1 | cut -f3 -d\ `

		if [ "x$new_sha" = "x$old_sha" ]
		then
			echo "Skipping build of $e"
			continue
		fi
	fi

	if [ $debian = 0 ]
	then
		do_build_and_install "$e"
	else
		do_build_deb "$e"
		do_install_deb "$e"
	fi

	if [ -d "$tsdir" ]
	then
		echo "$new_sha" > "$tsdir/$tsfile"
	fi
done
