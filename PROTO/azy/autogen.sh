#!/bin/bash
bs_dir="$(dirname $(readlink -f $0))"
rm -rf "${bs_dir}"/autom4te.cache
rm -f "${bs_dir}"/aclocal.m4 "${bs_dir}"/ltmain.sh

echo 'Running autoreconf -if...'
autoreconf -if || exit 1
echo 'Configuring...'
[[ -z "$NOCONFIGURE" ]] && cd "${bs_dir}" && ./configure $@ && cd - &> /dev/null || exit 1
echo -e "\n\nDone!"
echo -e "\nYou must now run make and then 'make install' as a privileged user."
