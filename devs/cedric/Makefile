PKG_CONFIG_PATH=/usr/devel/lib/pkgconfig:/usr/lib/pkgconfig:/usr/share/pkgconfig
PATH=/usr/devel/bin:/usr/bin/:/bin
CFLAGS="-g -ggdb3 -O3 -Wall -Wshadow -W -Wextra"
CC=colorgcc
#PREFIX=/opt/e17
PREFIX=/usr/devel

all: e17 elsa enki modules expedite eyelight evas_generic_loaders

modules: notification comp-scale engage places mail

e17: builddir/e17
builddir/e17: edje efreet e_dbus eeze
	cd e && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} make -j 8 && sudo make install
	touch builddir/e17

elsa: builddir/elsa
builddir/elsa: efl elementary
	cd PROTO/elsa && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} make -j 8 && sudo make install
	touch builddir/elsa

efl: edje efreet e_dbus eeze eio

eina: builddir/eina
builddir/eina:
	cd eina && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --enable-on-off-threads --disable-magic-debug --enable-amalgamation --prefix=/usr/devel #  --enable-debug-threads
	make -C eina -j 8
	sudo make -C eina install
	touch builddir/eina

eet: builddir/eet
builddir/eet: eina
	cd eet && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} make -j 8 && sudo make install
	touch builddir/eet

embryo: builddir/embryo
builddir/embryo: eina
	cd embryo && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} make -j 8 && sudo make install
	touch builddir/embryo

evas: builddir/evas
builddir/evas: eina eet
	cd evas && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --enable-software-xcb --enable-async-render --enable-software-sdl --prefix=${PREFIX} && PATH=${PATH} make -j 8 && sudo make install #  --enable-pipe-render
	touch builddir/evas

evas_generic_loaders: builddir/evas_generic_loaders
builddir/evas_generic_loaders: eina evas
	cd evas_generic_loaders && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh  --prefix=${PREFIX} && PATH=${PATH} make -j 8 && sudo make install
	touch builddir/evas_generic_loaders

ecore: builddir/ecore
builddir/ecore: eina evas
	cd ecore && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} make -j 8 && sudo make install
	touch builddir/ecore

edje: builddir/edje
builddir/edje: ecore evas eet eina embryo
	cd edje && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} make -j 8 && sudo make install
	touch builddir/edje

efreet: builddir/efreet
builddir/efreet: ecore eina
	cd efreet && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} make -j 8 && sudo make install
	touch builddir/efreet

e_dbus: builddir/e_dbus
builddir/e_dbus: ecore eina
	cd e_dbus && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} make -j 8 && sudo make install
	touch builddir/e_dbus

eeze: builddir/eeze
builddir/eeze: ecore eina
	cd eeze && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} make -j 8 && sudo make install
	touch builddir/eeze

eio: builddir/eio
builddir/eio: ecore eina
	cd eio && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} make -j 8 && sudo make install
	touch builddir/eio

elementary: builddir/elementary
builddir/elementary: edje ecore evas eina eet ethumb
	cd elementary && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} make -j 8 && sudo make install
	touch builddir/elementary

emotion: builddir/emotion
builddir/emotion: eina evas ecore edje
	cd emotion && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} make -j 8 && sudo make install
	touch builddir/emotion

ephoto: builddir/ephoto
builddir/ephoto: eina evas edje elementary
	cd ephoto && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} make -j 8 && sudo make install
	touch builddir/ephoto

ethumb: builddir/ethumb
builddir/ethumb: edje emotion eina
	cd ethumb && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} make -j 8 && sudo make install
	touch builddir/ethumb

expedite: builddir/expedite
builddir/expedite: evas eina
	cd expedite && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} make -j 8 && sudo make install
	touch builddir/expedite

notification: builddir/notification
builddir/notification: e17
	cd E-MODULES-EXTRA/notification/ && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} make -j 8 && sudo make install
	touch builddir/notification

mail: builddir/mail
builddir/mail: e17
	cd E-MODULES-EXTRA/mail/ && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} make -j 8 && sudo make install
	touch builddir/mail

places: builddir/places
builddir/places: e17
	cd E-MODULES-EXTRA/places && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} make -j 8 && sudo make install
	touch builddir/places

engage: builddir/engage
builddir/engage: e17
	cd E-MODULES-EXTRA/engage && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} make -j 8 && sudo make install
	touch builddir/engage

elfe: builddir/elfe
builddir/elfe: e17 elementary
	cd E-MODULES-EXTRA/elfe && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} make -j 8 && sudo make install
	touch builddir/elfe

comp-scale: builddir/comp-scale
builddir/comp-scale: e17
	cd E-MODULES-EXTRA/comp-scale && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} make -j 8 && sudo make install
	touch builddir/comp-scale

enki: builddir/enki
builddir/enki: eina evas elementary edje enlil
	cd enki/ && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} make -j 8 && sudo make install
	touch builddir/enki

enlil: builddir/enlil
builddir/enlil: eina ecore edje ethumb azy
	cd enlil/ && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} make -j 8 && sudo PATH=${PATH} make install
	touch builddir/enlil

azy: builddir/azy
builddir/azy: eina ecore eet
	cd PROTO/azy && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} make -j 8 && sudo make install
	touch builddir/azy

eyelight: builddir/eyelight
builddir/eyelight: eina ecore edje emotion
	cd PROTO/eyelight && CFLAGS=${CFLAGS} CC=${CC} PATH=${PATH} PKG_CONFIG_PATH=${PKG_CONFIG_PATH} ./autogen.sh --prefix=${PREFIX} && PATH=${PATH} make -j 8 && sudo make install
	touch builddir/eyelight

