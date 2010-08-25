#!/bin/bash

ARGS=
AUTOGEN=0
TARGET=
TOPDIR=$(pwd)

function usage {
		cat - <<EOF
USAGE:
	run_cmds [-a|-t|-h] script.cocci
OPTIONS:
	-a       Run autogen.sh in each project first
EOF
}


while getopts "at:h" flag
do
	case "$flag" in
	a)
		AUTOGEN=1
		;;
	t)
		TARGET="$OPTARG"
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


SCRIPT=$1
shift
ARGS="$*"


function call_spatch {
	local proj=$1
	local includes=$*
	pushd $proj
	[[ $AUTOGEN -eq 1 ]] && ./autogen.sh
	spatch -sp $SCRIPT -macro_file_builtins ${TOPDIR}/SCRIPTS/coccinelle/ecocci.h \
	       -I /usr/include/ -I /usr/local/include/ \
	       $includes \
	       -all_includes -relax_include_path -in_place $ARGS -dir ./
	popd
}


if [ ! -z "$TARGET" ]; then
	TARGETS=$TARGET
else
	TARGETS="eina eet embryo evas ecore efreet edje e_dbus eeze e TMP/st/elementary E16 E-MODULES-EXTRA BINDINGS editje edje_viewer eio elicit elitaire elmdentica emotion empower emprint enki enlil ensure ephoto eterm ethumb eve exalt EXAMPLES expedite exquisite imlib2 imlib2_loaders MISC PROTO rage TEST"
fi


for d in $TARGETS; do
	case $d in

	eina)
		call_spatch $d -I /usr/include/glib-2.0/ -I win32/common/ -I src/include/
        # missing headers:
	#  windows.h, config.h, Evil.h,
	#  ememoa_mempool_fixed.h, ememoa_mempool_unknown_size.h,

		;;
	eet)
		call_spatch $d -I ${TOPDIR}/eina/src/include/ -I src/lib/
        # missing headers:
	#  config.h, winsock2.h, stddef.h, Evil.h
	#  ememoa_mempool_fixed.h, ememoa_mempool_unknown_size.h,

		;;
	embryo)
		call_spatch $d -I ${TOPDIR}/eina/src/include/ -I src/lib/
        # missing headers:
	#  config.h, stdarg.h, stddef.h, Evil.h
		;;
	evas)
		call_spatch $d -I /usr/include/valgrind/ -I /usr/include/librsvg-2 -I ${TOPDIR}/eet/src/lib/ -I src/lib/ -I src/modules/engines/buffer/ $(for i in src/lib/{include,file,cserve}; do echo -I $i; done)
	# missing headers:
	#  windows.h, config.h, Evil.h, altivec.h, Edb.h,
	#  pixman.h, GL/wglew.h, ApplicationServices/ApplicationServices.h
	#  evas_engine.h

		;;
        ecore)
		call_spatch $d -I /usr/include/glib-2.0/ -I ${TOPDIR}/eina/src/include/ -I ${TOPDIR}/eet/src/lib/ $(for i in src/lib/ecore*; do echo -I $i; done) 
	# missing headers:
	#  windows.h, config.h, Evas_Engine_SDL.h, Evas_Engine_GL_SDL.h,
	#  Evil.h, float.h, ieeefp.h, winsock2.h, process.h, winsock2.h,
	#  windowsx.h, basetyps.h, pixman.h, stddef.h, ws2tcpip.h, ares.h

		;;
	efreet)
		call_spatch $d -I ${TOPDIR}/eina/src/include/ $(for i in ${TOPDIR}/ecore/src/lib/ecore*; do echo -I $i; done) -I src/lib/
	# missing headers:
	#  winsock2.h, config.h, stddef.h

		;;
	edje)
		call_spatch $d -I ${TOPDIR}/eina/src/include/ -I ${TOPDIR}/evas/src/lib/  -I ${TOPDIR}/eet/src/lib/ -I ${TOPDIR}/embryo/src/lib/ $(for i in ${TOPDIR}/ecore/src/lib/ecore*; do echo -I $i; done) -I src/lib/
	# missing headers:
	#  config.h, stddef.h
		;;
	e_dbus)
		call_spatch $d -I ${TOPDIR}/eina/src/include/ $(for i in ${TOPDIR}/ecore/src/lib/ecore*; do echo -I $i; done) $(for i in src/lib/{bluez,connman,dbus,hal,notification,ofono,ukit}; do echo -I $i; done) \
	# missing headers:
	#  config.h, stddef.h
		;;
	eeze)
		call_spatch $d -I ${TOPDIR}/ecore/src/lib/ecore src/lib/
	# missing headers:
	#  config.h
		;;
	e)
		call_spatch $d -I ${TOPDIR}/eina/src/include/ $(for i in ${TOPDIR}/ecore/src/lib/ecore*; do echo -I $i; done) $(for i in ${TOPDIR}/e_dbus/src/lib/{bluez,connman,dbus,hal,notification,ofono,ukit}; do echo -I $i; done) -I ${TOPDIR}/eet/src/lib/ -I ${TOPDIR}/evas/src/lib/ -I ${TOPDIR}/edje/src/lib -I ${TOPDIR}/efreet/src/lib -I src/bin/ -I src/modules/illume2/
	# missing headers:
	#  config.h, stddef.h, stdarg
	#  machine/apm_bios.h, CFBase.h, CFNumber.h, CFArray.h, CFDictionary.h, CFRunLoop.h, ps/IOPSKeys.h, ps/IOPowerSources.h
	#  Exchange.h, evry_api.h

		;;
	TMP/st/elementary)
		call_spatch $d -I ${TOPDIR}/eina/src/include/ $(for i in ${TOPDIR}/ecore/src/lib/ecore*; do echo -I $i; done) $(for i in ${TOPDIR}/e_dbus/src/lib/{bluez,connman,dbus,hal,notification,ofono,ukit}; do echo -I $i; done) -I ${TOPDIR}/eet/src/lib/ -I ${TOPDIR}/evas/src/lib/ -I ${TOPDIR}/edje/src/lib -I ${TOPDIR}/efreet/src/lib -I ${TOPDIR}/PROTO/libeweather/src/lib/ -I ./ -I src/lib/ 
		;;
	E16)
		call_spatch $d -I /usr/include/gtk-2.0/ -I /usr/include/glib-2.0/ -I /usr/include/pango-1.0/ -I /usr/include/libgtop-2.0/ -I /usr/include/dbus-1.0/ -I epplets/api/ -I epplets/epplets/ -I e/src/
		;;
	E-MODULES-EXTRA)
		# !@##$&#@$##&% -- don't bother about includes, let's hope it works
		call_spatch $d $(for i in ${TOPDIR}/ecore/src/lib/ecore*; do echo -I $i; done) $(for i in ${TOPDIR}/e_dbus/src/lib/{bluez,connman,dbus,hal,notification,ofono,ukit}; do echo -I $i; done) -I ${TOPDIR}/e/src/bin/
		;;
	*)
		# !@##$&#@$##&% -- don't bother about includes, let's hope it works
		#BINDINGS editje edje_viewer eio elicit elitaire elmdentica emotion empower emprint enki enlil ensure ephoto eterm ethumb eve exalt EXAMPLES expedite exquisite imlib2 imlib2_loaders MISC PROTO rage TEST"
		call_spatch $d $(for i in ${TOPDIR}/ecore/src/lib/ecore*; do echo -I $i; done) $(for i in ${TOPDIR}/e_dbus/src/lib/{bluez,connman,dbus,hal,notification,ofono,ukit}; do echo -I $i; done) -I ${TOPDIR}/eina/src/include/ -I ${TOPDIR}/eet/src/lib/ -I ${TOPDIR}/evas/src/lib/ -I ${TOPDIR}/edje/src/lib
		;;
	esac
done

