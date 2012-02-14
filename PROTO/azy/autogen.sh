#!/bin/sh
cwd="$PWD"
bs_dir="$(dirname $(readlink -f $0))"
rm -rf "${bs_dir}"/autom4te.cache
rm -f "${bs_dir}"/aclocal.m4 "${bs_dir}"/ltmain.sh

echo 'Running autoreconf -if...'
cd "${bs_dir}" &> /dev/null
autoreconf -if || exit 1
if test -z "$NOCONFIGURE" ; then
	echo 'Configuring...'
	./configure $@
fi
test "$cwd" = "$bs_dir" || cd "$cwd" &> /dev/null
