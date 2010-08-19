#!/bin/bash

SCRIPT=$1
ARGS=
AUTOGEN=0

function usage {
		cat - <<EOF
USAGE:
	run_cmds [-a] script.cocci
OPTIONS:
	-a       Run autogen.sh in each project first
EOF
}


while getopts "ah" flag
do
	case "$flag" in
	a)
		AUTOGEN=1
		;;
	h)
		usage
		exit 0
		;;
	?)
		usage
		exit 1
		;;
	esac
done
shift $((OPTIND - 1))

ARGS="$*"

# eina
#========================
pushd eina
[[ $AUTOGEN -eq 1 ]] && ./autogen.sh
spatch -sp $SCRIPT -macro_file_builtins ../SCRIPTS/coccinelle/ecocci.h \
	-I /usr/include/  -I /usr/include/glib-2.0/ \
	-I win32/common/ -I src/include/ \
	-all_includes -relax_include_path -in_place $ARGS -dir ./
popd

        # missing headers:
	#  windows.h, config.h, Evil.h,
	#  ememoa_mempool_fixed.h, ememoa_mempool_unknown_size.h,

# eet
#========================
pushd eina
[[ $AUTOGEN -eq 1 ]] && ./autogen.sh
spatch -sp $SCRIPT -macro_file_builtins ../SCRIPTS/coccinelle/ecocci.h \
	-I /usr/include/ -I ../eina/src/include/  \
	-I src/lib/ \
	-all_includes -relax_include_path -in_place $ARGS -dir ./
popd

        # missing headers:
	#  config.h, winsock2.h, stddef.h, Evil.h
	#  ememoa_mempool_fixed.h, ememoa_mempool_unknown_size.h,

# embryo
#========================
pushd embryo
[[ $AUTOGEN -eq 1 ]] && ./autogen.sh
spatch -sp $SCRIPT -macro_file_builtins ../SCRIPTS/coccinelle/ecocci.h \
	-I /usr/include/ -I ../eina/src/include/  \
	-I src/lib/ \
	-all_includes -relax_include_path -in_place $ARGS -dir ./
popd

        # missing headers:
	#  config.h, stdarg.h, stddef.h, Evil.h

# evas
#========================
pushd evas
[[ $AUTOGEN -eq 1 ]] && ./autogen.sh
spatch -sp $SCRIPT -macro_file_builtins ../SCRIPTS/coccinelle/ecocci.h \
	-I /usr/include/ -I /usr/include/valgrind/ -I /usr/include/librsvg-2 \
	-I ../eet/src/lib/ \
	-I src/lib/ -I src/modules/engines/buffer/ \
	$(for i in src/lib/{include,file,cserve}; do echo -I $i; done) \
	-all_includes -relax_include_path -in_place $ARGS -dir ./
popd

	# missing headers:
	#  windows.h, config.h, Evil.h, altivec.h, Edb.h,
	#  pixman.h, GL/wglew.h, ApplicationServices/ApplicationServices.h
	#  evas_engine.h

# ecore
#========================
pushd ecore
[[ $AUTOGEN -eq 1 ]] && ./autogen.sh
spatch -sp $SCRIPT -macro_file_builtins ../SCRIPTS/coccinelle/ecocci.h \
       -I /usr/include/ -I /usr/include/glib-2.0/ \
       -I ../eina/src/include/ -I ../eet/src/lib/ \
       $(for i in src/lib/ecore*; do echo -I $i; done) \
       -all_includes -relax_include_path -in_place $ARGS -dir ./
popd

	# missing headers:
	#  windows.h, config.h, Evas_Engine_SDL.h, Evas_Engine_GL_SDL.h,
	#  Evil.h, float.h, ieeefp.h, winsock2.h, process.h, winsock2.h,
	#  windowsx.h, basetyps.h, pixman.h, stddef.h, ws2tcpip.h, ares.h

# efreet
#========================
pushd efreet
[[ $AUTOGEN -eq 1 ]] && ./autogen.sh
spatch -sp $SCRIPT -macro_file_builtins ../SCRIPTS/coccinelle/ecocci.h \
       -I /usr/include/ \
       -I ../eina/src/include/ \
       $(for i in ../ecore/src/lib/{ecore,ecore_file}; do echo -I $i; done) \
       -I src/lib/ \
       -all_includes -relax_include_path -in_place $ARGS -dir ./
popd

	# missing headers:
	#  winsock2.h, config.h, stddef.h

# edje
#========================
pushd edje
[[ $AUTOGEN -eq 1 ]] && ./autogen.sh
spatch -sp $SCRIPT -macro_file_builtins ../SCRIPTS/coccinelle/ecocci.h \
       -I /usr/include/ \
       -I ../eina/src/include/ -I ../evas/src/lib/ \
       $(for i in ../ecore/src/lib/ecore{,_evas,_file}/; do echo -I $i; done) \
       -I src/lib/ \
       -all_includes -relax_include_path -in_place $ARGS -dir ./
popd

	# missing headers:
	#  config.h, stddef.h

# e_dbus
#========================
pushd e_dbus
[[ $AUTOGEN -eq 1 ]] && ./autogen.sh
spatch -sp $SCRIPT -macro_file_builtins ../SCRIPTS/coccinelle/ecocci.h \
       -I /usr/include/ \
       -I ../eina/src/include/ -I ../ecore/src/lib/ecore/  \
       $(for i in src/lib/{bluez,connman,dbus,hal,notification,ofono,ukit}; do echo -I $i; done) \
       -all_includes -relax_include_path -in_place $ARGS -dir ./
popd

	# missing headers:
	#  config.h, stddef.h

# eeze
#========================
pushd eeze
[[ $AUTOGEN -eq 1 ]] && [[ $AUTOGEN -eq 1 ]] && ./autogen.sh
spatch -sp $SCRIPT -macro_file_builtins ../SCRIPTS/coccinelle/ecocci.h \
       -I /usr/include/ -I ../ecore/src/lib/ecore/ \
       -I src/lib/ \
       -all_includes -relax_include_path -in_place $ARGS -dir ./
popd

	# missing headers:
	#  config.h

# e
#========================
pushd e
[[ $AUTOGEN -eq 1 ]] && [[ $AUTOGEN -eq 1 ]] && ./autogen.sh
spatch -sp $SCRIPT -macro_file_builtins ../SCRIPTS/coccinelle/ecocci.h \
       -I /usr/include/  \
       -I ../eina/src/include/ \
       $(for i in ../ecore/src/lib/ecore*; do echo -I $i; done) \
       $(for i in ../e_dbus/src/lib/{bluez,connman,dbus,hal,notification,ofono,ukit}; do echo -I $i; done) \
       -I ../eet/src/lib/ -I ../evas/src/lib/ -I ../edje/src/lib \
       -I ../efreet/src/lib \
       -I src/bin/ -I src/modules/illume2/ \
       -all_includes -relax_include_path -in_place $ARGS -dir ./
popd

	# missing headers:
	#  config.h, stddef.h, stdarg
	#  machine/apm_bios.h, CFBase.h, CFNumber.h, CFArray.h, CFDictionary.h, CFRunLoop.h, ps/IOPSKeys.h, ps/IOPowerSources.h
	#  Exchange.h, evry_api.h

# elementary
#========================
pushd TMP/st/elementary
[[ $AUTOGEN -eq 1 ]] && ./autogen.sh
spatch -sp $SCRIPT -macro_file_builtins ../../../SCRIPTS/coccinelle/ecocci.h \
       -I /usr/include/  \
       -I ../../../eina/src/include/ \
       $(for i in ../../../ecore/src/lib/ecore*; do echo -I $i; done) \
       $(for i in ../../../e_dbus/src/lib/{bluez,connman,dbus,hal,notification,ofono,ukit}; do echo -I $i; done) \
       -I ../../../eet/src/lib/ -I ../../../evas/src/lib/ -I ../../../edje/src/lib \
       -I ../../../efreet/src/lib -I ../../../PROTO/libeweather/src/lib/ \
       -I ./ -I src/lib/ \
       -all_includes -relax_include_path -in_place $ARGS -dir ./
popd

# E16
#========================
pushd E16
[[ $AUTOGEN -eq 1 ]] && ./autogen.sh
spatch -sp $SCRIPT -macro_file_builtins ../SCRIPTS/coccinelle/ecocci.h \
	-I /usr/include/ -I /usr/local/include/ \
	-I /usr/include/gtk-2.0/ -I /usr/include/glib-2.0/ -I /usr/include/pango-1.0/ \
	-I /usr/include/libgtop-2.0/ -I /usr/include/dbus-1.0/ \
	-I epplets/api/ -I epplets/epplets/ \
	-I e/src/ \
	-all_includes -relax_include_path -in_place $ARGS -dir ./
popd

# E-MODULES-EXTRA
#========================
# !@##$&#@$##&% -- don't bother about includes, let's hope it works
pushd E-MODULES-EXTRA
[[ $AUTOGEN -eq 1 ]] && ./autogen.sh
spatch -sp $SCRIPT -macro_file_builtins ../SCRIPTS/coccinelle/ecocci.h \
       -I /usr/include/ -I /usr/local/include/ \
       $(for i in ../ecore/src/lib/ecore*; do echo -I $i; done) \
       $(for i in ../e_dbus/src/lib/{bluez,connman,dbus,hal,notification,ofono,ukit}; do echo -I $i; done) \
       -I ../e/src/bin/ \
       -all_includes -relax_include_path -in_place $ARGS -dir ./
popd


# remaining dirs
#========================
# !@##$&#@$##&% -- don't bother about includes, let's hope it works
for d in BINDINGS editje edje_viewer eio elicit elitaire elmdentica emotion empower emprint enki enlil ensure ephoto eterm ethumb eve ewl exalt EXAMPLES expedite exquisite imlib2 imlib2_loaders MISC PROTO rage TEST
do
	pushd $d
	spatch -sp $SCRIPT -macro_file_builtins ../SCRIPTS/coccinelle/ecocci.h \
	       -I /usr/include/ -I /usr/local/include/ \
	       $(for i in ../ecore/src/lib/ecore*; do echo -I $i; done) \
	       $(for i in ../e_dbus/src/lib/{bluez,connman,dbus,hal,notification,ofono,ukit}; do echo -I $i; done) \
               -I ../eina/src/include/ -I ../eet/src/lib/ -I ../evas/src/lib/ -I ../edje/src/lib \
	       -all_includes -relax_include_path -in_place $ARGS -dir ./
	popd
done


