#!/bin/sh
bs_dir="$(dirname $(readlink -f $0))"
rm -rf "${bs_dir}"/autom4te.cache
rm -f "${bs_dir}"/aclocal.m4 "${bs_dir}"/ltmain.sh

echo 'Running autoreconf -if...'
autoreconf -if || exit 1
echo 'Configuring...'
[[ -n "$NOCONFIGURE" ]] || cd "${bs_dir}" && ./configure $@ && cd - &> /dev/null || exit 1
echo -e "\nRunning make..."
make -C "${bs_dir}" -j2 --no-print-directory V=0 || exit 1
echo -e "\n\nDone!"
echo -e "\nYou must now run 'make install' as a privileged user."
