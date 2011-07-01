#!/bin/bash

set -e

export PREFIX=/usr/local
export CFLAGS="-O2 -march=native -ffast-math -g3 -W -Wall -Wextra" # -Wshadow"
export CC="ccache gcc"
export MAKEFLAGS="make -j6"
alias make='make -j6'

export BUILD_E_DEPENDS="eeze"
export BUILD_ELM_DEPENDS="PROTO/emap eio emotion"
export BUILD_BASIC="eina eet evas ecore embryo edje e_dbus efreet expedite "$BUILD_E_DEPENDS" e ethumb "$BUILD_ELM_DEPENDS" elementary"
export BUILD_PYTHON_BINDINGS="BINDINGS/python/python-evas BINDINGS/python/python-elementary BINDINGS/python/python-ecore BINDINGS/python/python-edje BINDINGS/python/python-emotion"
export BUILD_C_BINDINGS="BINDINGS/cxx/eflxx BINDINGS/cxx/einaxx BINDINGS/cxx/evasxx BINDINGS/cxx/ecorexx BINDINGS/cxx/elementaryxx"
export BUILD_BINDINGS=$BUILD_PYTHON_BINDINGS" "$BUILD_C_BINDINGS" "
export BUILD_E_MODULES="E-MODULES-EXTRA/comp-scale E-MODULES-EXTRA/elfe E-MODULES-EXTRA/engage"
export BUILD_EXAMPLE="EXAMPLES/elementary/calculator EXAMPLES/elementary/converter EXAMPLES/elementary/phonebook EXAMPLES/elementary/sticky-notes"
export BUILD_ETC="editje PROTO/eyelight FORMATTING/ecrustify ephoto edje_viewer PROTO/emap PROTO/azy elmdentica eio enlil" #enki
export BUILD=$BUILD_BASIC" "$BUILD_BINDINGS" "$BUILD_E_MODULES" "$BUILD_EXAMPLE" "$BUILD_ETC

PWD=`pwd`
LOG_WARN_FILE=$PWD"/warnings.txt"
rm $LOG_WARN_FILE -f
touch $LOG_WARN_FILE

for I in $BUILD; do
  pushd $I
	echo " "
	echo "============ "$I" ============"
	echo "" >> $LOG_WARN_FILE
	echo "["$I"]" >> $LOG_WARN_FILE
	make clean distclean || true
	./autogen.sh
	make 2>> $LOG_WARN_FILE 
	sudo make install
	sudo ldconfig
	#ctags -h ".h.x.cpp.c" -R
  popd
done

#efenniht theme
echo ""
echo "============ efenniht ============"
pushd THEMES/efenniht
	make install-home
popd

#detour theme (elm)
echo ""
echo "============ detour elm ============"
pushd THEMES/detour-elm
	make clean
#	make
#	make install
popd

#detourious
echo ""
echo "============ detourious ============"
pushd THEMES/detourious
	make
	make install
popd

#darkness theme
echo ""
echo "============ darkness ============"
pushd THEMES/darkness
	#./build.sh -r
	./build.sh
	pushd elm
		./build.sh
		cp *.edj ~/.elementary/themes
	popd
popd

rm tags -f
ctags -h ".h.x.cpp.c" -R
enlightenment_remote -restart
