#!/bin/bash
#

sudo pwd

export CFLAGS='-arch i386 -mmacosx-version-min=10.5 -I/opt/local/include -L/opt/local/lib'
export CXXFLAGS=$CFLAGS
export OBJCFLAGS=$CFLAGS
export LDFLAGS="$CFLAGS -headerpad_max_install_names"
export MACOSX_DEPLOYMENT_TARGET=10.5
export PKG_CONFIG_PATH=/opt/efl/lib/pkgconfig

for lib in eina eet evas ecore embryo edje; do
  if [ "$lib" = "evas" ]; then
     CONFIGURE_OPTIONS=" --enable-software-16-x11 --enable-software-8-x11"
  elif [ "$lib" = "ecore" ]; then
     CONFIGURE_OPTIONS="--disable-curl --enable-ecore_x --disable-glib --enable-ecore-evas-software-x11 --enable-ecore-evas-software-16-x11"
  else
     CONFIGURE_OPTIONS=""
  fi
  #cd $lib && ./configure --prefix=/opt/efl $CONFIGURE_OPTIONS && make clean all && sudo make install && cd ..
done

export CFLAGS='-arch ppc -mmacosx-version-min=10.5 -I/opt/local/include -L/opt/local/lib'
export CXXFLAGS=$CFLAGS
export LDFLAGS="$CFLAGS -headerpad_max_install_names"
export OBJCFLAGS=$CFLAGS
export PKG_CONFIG_PATH=/opt/efl-ppc/lib/pkgconfig
for lib in eina eet evas ecore embryo edje; do
  if [ "$lib" = "evas" ]; then
     CONFIGURE_OPTIONS=" --enable-software-16-x11 --enable-software-8-x11 --disable-cpu-altivec"
  elif [ "$lib" = "ecore" ]; then
     CONFIGURE_OPTIONS="--disable-curl --enable-ecore_x --disable-glib --enable-ecore-evas-software-x11 --enable-ecore-evas-software-16-x11"
  else
     CONFIGURE_OPTIONS=""
  fi
  cd $lib && ./configure --host=ppc-apple-darwin10.4 --prefix=/opt/efl-ppc $CONFIGURE_OPTIONS && make clean all && sudo make install && cd ..
done
