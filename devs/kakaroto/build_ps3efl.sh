#!/bin/bash
set -e

# Must set this option, else script will not expand aliases.
shopt -s expand_aliases

EXPEDITE_TITLE="Expedite - Evas Benchmarking Suite"
EXPEDITE_APPID="EXPEDITE_"
EXPEDITE_DATADIR="/dev_hdd0/game/$EXPEDITE_APPID/USRDIR/"

ESKISS_TITLE="Eskiss"
ESKISS_APPID="ESKISS_00"
ESKISS_DATADIR="/dev_hdd0/game/$ESKISS_APPID/USRDIR/"


MINIMAL_TOC="-mminimal-toc"
#DEBUG_CFLAGS="-g -ggdb -O0"
DEBUG_CFLAGS="-g -O3"
CONFIGURE=1
CLEAN_RULE="clean"
FSELF="make_fself"

alias ps3-configure='AR="powerpc64-ps3-elf-ar" CC="powerpc64-ps3-elf-gcc" RANLIB="powerpc64-ps3-elf-ranlib" CFLAGS="$DEBUG_CFLAGS -Wall -I$PSL1GHT/ppu/include -I$PS3DEV/portlibs/ppu/include $MINIMAL_TOC $MYCFLAGS" CPPFLAGS="-I$PSL1GHT/ppu/include -I$PS3DEV/portlibs/ppu/include" CXXFLAGS="-I$PSL1GHT/ppu/include -I$PS3DEV/portlibs/ppu/include"  LDFLAGS="-L$PSL1GHT/ppu/lib -L$PS3DEV/portlibs/ppu/lib" PKG_CONFIG_LIBDIR="$PSL1GHT/ppu/lib/pkgconfig" PKG_CONFIG_PATH="$PS3DEV/portlibs/ppu/lib/pkgconfig"  PKG_CONFIG="pkg-config --static" ./configure   --prefix="$PS3DEV/portlibs/ppu"   --host=powerpc64-ps3-elf    --includedir="$PS3DEV/portlibs/ppu/include"   --libdir="$PS3DEV/portlibs/ppu/lib" '
alias ps3-smi='sudo -E PATH=$PATH make install'

function generate_cmake_toolchain {
    cat > Toolchain-ps3.cmake <<EOF
# this one is important
SET(CMAKE_SYSTEM_NAME Generic)
#this one not so much
SET(CMAKE_SYSTEM_VERSION 1)

SET(CMAKE_SYSTEM_PROCESSOR powerpc64)

SET(PS3DEV /usr/local/ps3dev)
SET(PSL1GHT ${PS3DEV}/psl1ght)

# specify the cross compiler
SET(CMAKE_C_COMPILER ppu-gcc)
SET(CMAKE_CXX_COMPILER ppu-g++)
SET(CMAKE_C_FLAGS "-mcpu=cell -I${PSL1GHT}/ppu/include -I${PS3DEV}/portlibs/ppu/include")
SET(CMAKE_CXX_FLAGS "-mcpu=cell -I${PSL1GHT}/ppu/include -I${PS3DEV}/portlibs/ppu/include")

# where is the target environment 
SET(CMAKE_FIND_ROOT_PATH ${PS3DEV}/ppu ${PSL1GHT}/ppu )

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
EOF
}

generate_ps3sfo() {
    title=$1
    appid=$2

    sed -e "s/Test - PSL1GHT/$title/" -e "s/TEST00000/$appid/" $PS3DEV/bin/sfo.xml > ps3sfo.xml || return 1
}

make_pkg() {
    name=$1
    logo=$2
    title=$3
    appid=$4
    if [ -z $5 ] ; then
        datadir="/dev_hdd0/game/$appid/USRDIR/"
    else
        datadir=$5
    fi
    contentid="UP0001-$appid-0000000000000000"

    generate_ps3sfo "$title" "$appid" || return 1

    cp src/bin/$name $name.elf && sprxlinker $name.elf && \
        $FSELF $name.elf $name.self && \
        mkdir -p pkg/USRDIR && cp $logo pkg/ICON0.PNG && \
        make_self_npdrm $name.elf pkg/USRDIR/EBOOT.BIN $contentid && \
        sfo.py --title $title --appid $appid -f ps3sfo.xml pkg/PARAM.SFO  && \
        make install DESTDIR=`pwd`/temp_install && \
        cp -rf temp_install/$datadir/* pkg/USRDIR/  && \
        rm -rf temp_install && \
        pkg.py --contentid $contentid pkg/ $name.pkg && \
        cp $name.pkg $name.retail.pkg && package_finalize $name.retail.pkg && \
        rm -rf pkg
}


# escape :
function escape {
    cd escape || cd PROTO/escape || return 1

    if [ $CONFIGURE == "1" ]; then
        MYCFLAGS="-I$PS3DEV/ppu/lib/gcc/powerpc64-ps3-elf/4.5.2/plugin/include/" ps3-configure || return 1
    fi
    make $CLEAN_RULE all && \
        ps3-smi && \
        cd ..
}

# eina :
function eina {
    cd eina || return 1
    if [ $CONFIGURE == "1" ]; then
        #MYCFLAGS="-DDEBUG" ps3-configure || return 1
        ps3-configure || return 1
    fi
    make $CLEAN_RULE all && \
        ps3-smi && \
        cd ..
}

# eet :
function eet {
    cd eet || return 1
    if [ $CONFIGURE == "1" ]; then
        ps3-configure || return 1
    fi
    make $CLEAN_RULE all && \
        ps3-smi && \
        cd ..
}

# expat
function expat {
    cd expat-2.0.1 || return 1
    if [ $CONFIGURE == "1" ]; then
        ps3-configure || return 1
    fi
    make $CLEAN_RULE all && \
        ps3-smi && \
        cd ..
}


# fontconfig
function fontconfig {
    cd fontconfig-2.8.0 || return 1
    if [ $CONFIGURE == "1" ]; then
        ps3-configure --with-arch=powerpc64 --disable-docs || return 1
    fi

    if test $CLEAN_RULE != "" ; then
        make clean || return 1
    fi
    cd fc-arch &&  make CFLAGS="$DEBUG_CFLAGS" CPPFLAGS="" && cd ../ && \
        cd fc-case && make CFLAGS="$DEBUG_CFLAGS" CPPFLAGS="" && cd ../ && \
        cd fc-glyphname && make CFLAGS="$DEBUG_CFLAGS" CPPFLAGS="" && cd ../ && \
        cd fc-lang && make CFLAGS="$DEBUG_CFLAGS" CPPFLAGS="" && cd ../ && \
        make LIBS="-lescape -lnet -lsysmodule -liberty" && \
        ps3-smi && \
        cd ..
}
    
# evas :
function evas {
    cd evas || return 1
    if [ $CONFIGURE == "1" ]; then
        ps3-configure --disable-async-events --disable-async-preload --enable-software-sdl=static --enable-software-16-sdl=static --disable-shared --enable-buffer=static --enable-image-loader-eet=static --enable-font-loader-eet --enable-image-loader-gif=static --enable-image-loader-jpeg=static  --enable-image-loader-png=static  --enable-image-loader-tiff=static --enable-image-loader-bmp=static --enable-image-loader-xpm=static  --enable-image-loader-psd=static --enable-image-loader-pmaps=static  --enable-image-loader-ico=static --enable-image-loader-wbmp=static --enable-image-loader-tga=static --enable-static-software-generic --enable-static-software-16 --enable-psl1ght=static --enable-fontconfig || return 1
    fi
    make $CLEAN_RULE all && \
        ps3-smi && \
        cd ..
}

# expedite
function expedite {
    cd expedite || return 1

    if [ $CONFIGURE == "1" ]; then
        ps3-configure --datadir=$EXPEDITE_DATADIR || return 1
    fi
    make $CLEAN_RULE all && \
        make_pkg expedite "data/logo.png" "$EXPEDITE_TITLE" "$EXPEDITE_APPID" "$EXPEDITE_DATADIR" && \
        cd ..
}

# expat
function cares {
    cd c-ares-1.7.4 || return 1
    if [ $CONFIGURE == "1" ]; then
        LIBS="-lnet -lsysmodule" ps3-configure || return 1
    fi
    make $CLEAN_RULE all && \
        ps3-smi && \
        cd ..
}

# ecore :
function ecore {
## TODO: ecore_app uses execvp
## TODO: add ecore_con: MYCFLAGS="-I$PSL1GHT/ppu/include/net "
    cd ecore  || return 1

    if [ $CONFIGURE == "1" ]; then
        ps3-configure --disable-ecore-x --enable-ecore-sdl --enable-ecore-evas-software-sdl --enable-ecore-psl1ght --enable-ecore-evas-psl1ght --enable-ecore-con --enable-cares || return 1
    fi
    make $CLEAN_RULE all && \
        ps3-smi && \
        cd ..
}

# embryo
function embryo {
    cd embryo || return 1
    if [ $CONFIGURE == "1" ]; then
        ps3-configure || return 1
    fi
    make $CLEAN_RULE all && \
        ps3-smi && \
        cd ..
}

# lua :
function lua {
    cd lua-5.1.4 && \
        make $CLEAN_RULE ps3 && \
        sudo -E PATH=$PATH make INSTALL_TOP="$PS3DEV/portlibs/ppu" install && \
        sudo -E PATH=$PATH make INSTALL_TOP="$PS3DEV/portlibs/ppu" RANLIB=powerpc64-ps3-elf-ranlib  ranlib && \
        sudo -E cp etc/lua.pc $PS3DEV/portlibs/ppu/lib/pkgconfig && \
        sudo -E sed -i -e "s#/usr/local#$PS3DEV/portlibs/ppu#" $PS3DEV/portlibs/ppu/lib/pkgconfig/lua.pc && \
        cd ..
}

# edje
function edje {
    cd edje || return 1

    if [ $CONFIGURE == "1" ]; then
        ps3-configure --disable-edje-player --disable-edje-inspector --disable-edje-external-inspector --disable-edje-cc --disable-edje-decc --disable-edje-recc || return 1
    fi
    make $CLEAN_RULE all && \
        ps3-smi && \
        cd ..
}

# Chipmunk
function chipmunk {
    generate_cmake_toolchain || return 1
    cd Chipmunk-5.3.5 && \
        rm -rf build && \
        mkdir build && \
        cd build  && \
        cmake -DCMAKE_INSTALL_PREFIX=/usr/local/ps3dev/portlibs/ppu/ -DBUILD_SHARED=OFF -DBUILD_DEMOS=OFF -DCMAKE_TOOLCHAIN_FILE=$(pwd)/../../Toolchain-ps3.cmake .. && \
        make && \
        ps3-smi && \
        cd ../..
}

# eskiss
function eskiss {
    cd eskiss || cd GAMES/eskiss || return 1

    if [ $CONFIGURE == "1" ]; then
        ps3-configure --datadir=$ESKISS_DATADIR || return 1
    fi
    make $CLEAN_RULE all EDJE_CC=edje_cc && \
        make_pkg eskiss "data/edje/title.png" "$ESKISS_TITLE" "$ESKISS_APPID" "$ESKISS_DATADIR" && \
        cd ..
}

# elementary
function elementary {
## TODO!! remove dlopen
    cd elementary || return 1
    if [ $CONFIGURE == "1" ]; then
        ps3-configure --disable-quick-launch || return 1
    fi
    make $CLEAN_RULE all EDJE_CC=edje_cc EET_EET=eet && \
        ps3-smi && \
        cp src/bin/elementary_config elementary_config.elf && \
        sprxlinker elementary_config.elf && \
        $FSELF elementary_config.elf elementary_config.self && \
        cp src/bin/elementary_test elementary_test.elf && \
        sprxlinker elementary_test.elf && \
        $FSELF elementary_test.elf elementary_test.self && \
        cd ..

}

# efreet eio eeze e_dbus -- not needed
# azy ethumb -- not needed
# exchange e -- not needed

# eem
function eem {
    cd eem && \
        AR="powerpc64-ps3-elf-ar" CC="powerpc64-ps3-elf-gcc" RANLIB="powerpc64-ps3-elf-ranlib" CFLAGS="-O2 -Wall -I$PSL1GHT/ppu/include -I$PS3DEV/portlibs/ppu/include $MYCFLAGS" CXXFLAGS="-I$PSL1GHT/ppu/include -I$PS3DEV/portlibs/ppu/include"  LDFLAGS="-L$PSL1GHT/ppu/lib -L$PS3DEV/portlibs/ppu/lib" PKG_CONFIG_LIBDIR="$PSL1GHT/ppu/lib/pkgconfig" PKG_CONFIG_PATH="$PS3DEV/portlibs/ppu/lib/pkgconfig" make LIBS="-lescape -leina -ledje -lembryo -levas -leet -lfreetype -ljpeg -lpng -lz -lm -lSDL -lio -laudio -lgcm_sys -lrsx -lgcm_sys -lm -lsysutil -lfreetype -lescape -lecore -leina -lnet -lsysutil -liberty -lecore_imf -lsysmodule -lm -llua -lm -lecore_imf_evas -lecore_evas -lecore_file -levas -leina -lescape -liberty" && \
        cd ..
}

function _hms {
    RUNTIME=$1
    printf "%02d:%02d:%02d" $((RUNTIME/3600)) $((RUNTIME/60%60)) $((RUNTIME%60))
}

function build {
    what=$1
    pwd=$(pwd)
    echo -ne "Building $what : "
    START_TIME=`date +%s`
    $what >& build_${what}.log || \
        (echo "Error!" && \
          cd $pwd && \
          (tail -n 25 build_${what}.log || true) && \
          echo -ne "\n\nSee build_${what}.log for details.\n" && \
          exit 1)
    END_TIME=`date +%s`
    ELAPSED=`expr $END_TIME - $START_TIME || true`
    echo "Done (Elapsed : " `_hms $ELAPSED` ")"
    cd $pwd
}

sudo pwd > /dev/null

start=`date +%s`
if test -z $1 ; then
    build escape
    build eina
    build eet
    build expat
    build fontconfig
    build evas
    build expedite
    build cares
    build ecore
    build embryo
    build lua
    build edje
    build chipmunk
    build eskiss
    build elementary
    # eem
else
    while test -n "$1"; do
        build $1
        shift
    done
fi

rm *.log

end=`date +%s`
ELAPSED=`expr $end - $start`
echo "Total build time : " `_hms $ELAPSED` ""
