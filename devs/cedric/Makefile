PKG_CONFIG_PATH=/usr/devel/lib/pkgconfig:/usr/lib/pkgconfig:/usr/share/pkgconfig:/usr/local/lib64/pkgconfig/
PATH=/usr/devel/bin:/usr/bin/:/bin
CFLAGS="-g -ggdb3 -O0 -Wall -Wshadow -W -Wextra -pipe"
CC="ccache colorgcc"
#CC=clang
#PREFIX=/opt/e17
PREFIX=/usr/devel
MFLAGS=-j 16
#SCANNER=scan-build -o /home/cedric/work/enlightenment/llvm-analysis/
SCANNER=

all: e17 entrance modules expedite eyelight evas_generic_loaders edje_viewer excessive enjoy espionnage enna-explorer equate esskyuehl ekbd ephoto clouseau elev8 efx terminology ephysics etrophy eobj

modules: comp-scale engage places mail

e17: builddir/e17
builddir/e17: edje efreet e_dbus eeze elementary eio
	cd e && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} --enable-mount-eeze && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/e17

entrance: builddir/entrance
builddir/entrance: efl elementary
	cd PROTO/entrance && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/entrance

vigrid: builddir/vigrid
builddir/vigrid: eina elementary
	cd PROTO/vigrid && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/vigrid

ekbd: builddir/ekbd
builddir/ekbd: efl elementary
	cd PROTO/ekbd && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/ekbd

efl: edje efreet e_dbus eeze eio

eina: builddir/eina
builddir/eina:
	cd eina && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --enable-valgrind --enable-on-off-threads --enable-magic-debug --prefix=/usr/devel --disable-debug-threads --enable-tests # --enable-benchmark #  --enable-amalgamation --enable-debug-malloc 
	make -C eina ${MFLAGS}
	sudo make -C eina install
	touch builddir/eina

eet: builddir/eet
builddir/eet: eina
	cd eet && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/eet

embryo: builddir/embryo
builddir/embryo: eina
	cd embryo && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/embryo

evas: builddir/evas
builddir/evas: eina eet
	cd evas && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --disable-async-render   --enable-pipe-render --enable-gl-sdl --enable-evas-magic-debug --enable-gl-xlib --disable-wayland-egl --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install # --enable-software-xcb=static  --enable-gl-xcb
	touch builddir/evas

evas_generic_loaders: builddir/evas_generic_loaders
builddir/evas_generic_loaders: eina evas
	cd evas_generic_loaders && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh  --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/evas_generic_loaders

ecore: builddir/ecore
builddir/ecore: eina evas
	cd ecore && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} --enable-build-examples --enable-ecore-sdl --disable-ecore-wayland && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install #  --enable-ecore-x-xcb
	touch builddir/ecore

efx: builddir/efx
builddir/efx: eina ecore evas
	cd efx && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/efx

edje: builddir/edje
builddir/edje: ecore evas eet eina embryo eio
	cd edje && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} --enable-edje-program-cache --enable-edje-calc-cache && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/edje

ephysics: builddir/ephysics
builddir/ephysics: ecore evas eet eina
	cd ephysics && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/ephysics

efreet: builddir/efreet
builddir/efreet: ecore eina
	cd efreet && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/efreet

e_dbus: builddir/e_dbus
builddir/e_dbus: ecore eina
	cd e_dbus && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/e_dbus

eeze: builddir/eeze
builddir/eeze: ecore eina
	cd eeze && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/eeze

eio: builddir/eio
builddir/eio: ecore eina
	cd eio && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/eio

elementary: builddir/elementary
builddir/elementary: edje ecore evas eina eet ethumb
	cd elementary && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} --enable-build-examples && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/elementary

clouseau: builddir/clouseau
builddir/clouseau: elementary evas
	cd clouseau && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/clouseau

terminology: builddir/terminology
builddir/terminology: elementary evas edje
	cd terminology && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/terminology

emotion: builddir/emotion
builddir/emotion: eina evas ecore edje eeze
	cd emotion && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/emotion

ephoto: builddir/ephoto
builddir/ephoto: eina evas edje elementary
	cd ephoto && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/ephoto

ethumb: builddir/ethumb
builddir/ethumb: edje emotion eina
	cd ethumb && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/ethumb

expedite: builddir/expedite
builddir/expedite: evas eina
	cd expedite && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/expedite

mail: builddir/mail
builddir/mail: e17
	cd E-MODULES-EXTRA/mail/ && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/mail

places: builddir/places
builddir/places: e17
	cd E-MODULES-EXTRA/places && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/places

engage: builddir/engage
builddir/engage: e17
	cd E-MODULES-EXTRA/engage && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/engage

elfe: builddir/elfe
builddir/elfe: e17 elementary
	cd E-MODULES-EXTRA/elfe && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/elfe

comp-scale: builddir/comp-scale
builddir/comp-scale: e17
	cd E-MODULES-EXTRA/comp-scale && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/comp-scale

drawer: builddir/drawer
builddir/drawer: e17
	cd E-MODULES-EXTRA/drawer && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/drawer

enki: builddir/enki
builddir/enki: eina evas elementary edje enlil
	cd enki/ && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/enki

enlil: builddir/enlil
builddir/enlil: eina ecore edje ethumb azy
	cd enlil/ && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo PATH=${PATH} ${SCANNER} make install
	touch builddir/enlil

azy: builddir/azy
builddir/azy: eina ecore eet
	cd PROTO/azy && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/azy

eobj: builddir/eobj
builddir/eobj: eina
	cd PROTO/eobj && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/eobj

esskyuehl: builddir/esskyuehl
builddir/esskyuehl: eina ecore
	cd PROTO/esskyuehl && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} NOCONFIGURE=1 ./autogen.sh && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./configure --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/esskyuehl

eyelight: builddir/eyelight
builddir/eyelight: eina ecore edje emotion
	cd PROTO/eyelight && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/eyelight

etrophy: builddir/etrophy
builddir/etrophy: eina ecore edje emotion
	cd PROTO/etrophy && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/etrophy

edje_viewer: builddir/edje_viewer
builddir/edje_viewer: elementary edje
	cd edje_viewer && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/edje_viewer

excessive: builddir/excessive
builddir/excessive: elementary emotion edje
	cd excessive && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/excessive

espionnage: builddir/espionnage
builddir/espionnage: elementary emotion edje
	cd espionnage && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/espionnage

enjoy: builddir/enjoy
builddir/enjoy: elementary emotion edje
	cd enjoy && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/enjoy

eve: builddir/eve
builddir/eve: elementary edje
	cd eve && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/eve

enna-explorer: buildir/enna-explorer
buildir/enna-explorer: elementary
	cd PROTO/enna-explorer && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/enna-explorer

elev8: buildir/elev8
buildir/elev8: elementary
	cd PROTO/elev8 && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/elev8

equate: builddir/equate
builddir/equate: elementary edje
	cd PROTO/equate && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/equate

ecrire: builddir/ecrire
builddir/ecrire: elementary ecore
	cd ecrire && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/ecrire

efbb: builddir/efbb
builddir/efbb: elementary evas ephysics
	cd GAMES/efbb && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} ${SCANNER} make ${MFLAGS} && sudo make install
	touch builddir/efbb
