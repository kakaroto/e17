#!/bin/bash

if [[ -z $@ ]];then
	echo "No modules provided, creating ebuilds for all modules by default"
	mods=($(svn ls https://svn.enlightenment.org/svn/e/trunk/E-MODULES-EXTRA|grep '/'|grep -v 'debian\|skel'|sed 's#/##g'))
else
	mods=($@)
	echo "Creating ebuilds for the following modules: $@"
fi

for x in ${mods[@]};do
	echo "Creating ebuild for $x:"
	echo -e "\tAttempting to grab a description from svn..."
	desc="$(svn cat https://svn.enlightenment.org/svn/e/trunk/E-MODULES-EXTRA/${x}/README|head -n1)"
	if [[ -n $desc ]];then
		echo -e "\tGrabbed \"$desc\""
	else
		echo -e "\tNo description grabbed, using something generic"
	fi
	mkdir -p e_module-${x}
	cat > e_module-${x}/e_module-${x}-9999.ebuild <<EOF
# Copyright 1999-2010 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# \$Header: $

EAPI="2"
ESVN_SUB_PROJECT="E-MODULES-EXTRA"
ESVN_URI_APPEND="\${PN#e_module-}"
E_NO_DOC="1"
inherit efl

DESCRIPTION="${desc:-$x module for enlightenment}"
IUSE=""
LICENSE="BSD"
SLOT="0"

RDEPEND=">=x11-wm/enlightenment-9999"
DEPEND="\${RDEPEND}"
EOF
echo -e "Success!\n"
done
