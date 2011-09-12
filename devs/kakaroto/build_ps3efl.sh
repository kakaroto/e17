#!/bin/bash
set -e

shopt -s expand_aliases
# Must set this option, else script will not expand aliases.


MINIMAL_TOC="-mminimal-toc"
ENABLE_FONTCONFIG=1
#DEBUG_CFLAGS="-g -ggdb -O0"
DEBUG_CFLAGS="-g -O3"
CONFIGURE=1
CLEAN_RULE="clean"
FSELF="make_fself"

alias ps3-configure='AR="powerpc64-ps3-elf-ar" CC="powerpc64-ps3-elf-gcc" RANLIB="powerpc64-ps3-elf-ranlib" CFLAGS="$DEBUG_CFLAGS -Wall -I$PSL1GHT/ppu/include -I$PS3DEV/portlibs/ppu/include $MINIMAL_TOC $MYCFLAGS" CXXFLAGS="-I$PSL1GHT/ppu/include -I$PS3DEV/portlibs/ppu/include"  LDFLAGS="-L$PSL1GHT/ppu/lib -L$PS3DEV/portlibs/ppu/lib" PKG_CONFIG_LIBDIR="$PSL1GHT/ppu/lib/pkgconfig" PKG_CONFIG_PATH="$PS3DEV/portlibs/ppu/lib/pkgconfig"  PKG_CONFIG="pkg-config --static" ./configure   --prefix="$PS3DEV/portlibs/ppu"   --host=powerpc64-ps3-elf    --includedir="$PS3DEV/portlibs/ppu/include"   --libdir="$PS3DEV/portlibs/ppu/lib" '
alias ps3-smi='sudo -E PATH=$PATH make install'

if [ $ENABLE_FONTCONFIG == "1" ]; then
    FONTCONFIG_OPT="--enable-fontconfig"
else
    FONTCONFIG_OPT=""
fi

# escape :
function escape {
    cd escape || cd PROTO/escape

    if [ $CONFIGURE == "1" ]; then
        MYCFLAGS="-I$PS3DEV/ppu/lib/gcc/powerpc64-ps3-elf/4.5.2/plugin/include/" ps3-configure
    fi
    make $CLEAN_RULE all && \
        ps3-smi && \
        cd ..
}

# eina :
function eina {
    cd eina
    if [ $CONFIGURE == "1" ]; then
        ps3-configure
    fi
    make $CLEAN_RULE all && \
        ps3-smi && \
        cd ..
}

# eet :
function eet {
    cd eet
    if [ $CONFIGURE == "1" ]; then
        ps3-configure
    fi
    make $CLEAN_RULE all && \
        ps3-smi && \
        cd ..
}

# expat
function expat {
    cd expat-2.0.1
    if [ $CONFIGURE == "1" ]; then
        ps3-configure
    fi
    make $CLEAN_RULE all && \
        ps3-smi && \
        cd ..
}


# fontconfig
function fontconfig {
    cd fontconfig-2.8.0
    if [ $CONFIGURE == "1" ]; then
        ps3-configure --with-arch=powerpc64 --disable-docs
    fi

    if test $CLEAN_RULE != "" ; then
        make clean
    fi
    cd fc-arch &&  make CFLAGS="$DEBUG_CFLAGS" && cd ../ && \
        cd fc-case && make CFLAGS="$DEBUG_CFLAGS" && cd ../ && \
        cd fc-glyphname && make CFLAGS="$DEBUG_CFLAGS" && cd ../ && \
        cd fc-lang && make CFLAGS="$DEBUG_CFLAGS" && cd ../ && \
        make LIBS="-lescape -lnet -lsysmodule -liberty" && \
        ps3-smi && \
        cd ..
}
    
# evas :
function evas {
    cd evas
    if [ $CONFIGURE == "1" ]; then
        ps3-configure --disable-async-events --disable-async-preload --enable-software-sdl=static --enable-software-16-sdl=static --disable-shared --enable-buffer=static --enable-image-loader-eet=static --enable-font-loader-eet --enable-image-loader-gif=static --enable-image-loader-jpeg=static  --enable-image-loader-png=static  --enable-image-loader-tiff=static --enable-image-loader-bmp=static --enable-image-loader-xpm=static  --enable-image-loader-psd=static --enable-image-loader-pmaps=static  --enable-image-loader-ico=static --enable-image-loader-wbmp=static --enable-image-loader-tga=static --enable-static-software-generic --enable-static-software-16 --enable-psl1ght=static $FONTCONFIG_OPT
    fi
    make $CLEAN_RULE all && \
        ps3-smi && \
        cd ..
}

# ecore :
function ecore {
## TODO: ecore_app uses execvp
## TODO: add ecore_con: MYCFLAGS="-I$PSL1GHT/ppu/include/net "
    cd ecore 

    if [ $CONFIGURE == "1" ]; then
        ps3-configure --disable-ecore-x --enable-ecore-sdl --enable-ecore-evas-software-sdl
    fi
    make $CLEAN_RULE all && \
        ps3-smi && \
        cd ..
}

# expedite
function expedite {
    cd expedite

    if [ $CONFIGURE == "1" ]; then
        ps3-configure
    fi
    make  $CLEAN_RULE all && \
        cp src/bin/expedite expedite.elf && sprxlinker expedite.elf && \
        $FSELF expedite.elf expedite.self && \
        mkdir -p pkg/USRDIR && cp data/logo.png pkg/ICON0.PNG && \
        make_self_npdrm expedite.elf pkg/USRDIR/EBOOT.BIN UP0001-EXPEDITE_00-0000000000000000 && \
        sfo.py --title "Expedite - Evas Benchmark suite" --appid "EXPEDITE" -f ps3sfo.xml pkg/PARAM.SFO  && \
        cp -rf data/ pkg/USRDIR/  && \
        pkg.py --contentid UP0001-EXPEDITE_00-0000000000000000 pkg/ expedite.pkg && \
        cp expedite.pkg expedite.retail.pkg && package_finalize expedite.retail.pkg && \
        rm -rf pkg && \
        cd ..
}

# embryo
function embryo {
    cd embryo
    if [ $CONFIGURE == "1" ]; then
        ps3-configure
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
    cd edje

    if [ $CONFIGURE == "1" ]; then
        ps3-configure --disable-edje-player --disable-edje-inspector --disable-edje-external-inspector --disable-edje-cc --disable-edje-decc --disable-edje-recc
    fi
    make $CLEAN_RULE all && \
        ps3-smi && \
        cd ..
}

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

# Chipmunk
function chipmunk {
    generate_cmake_toolchain
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
    ESKISS_DATADIR="/app_home/"

    cd eskiss || cd GAMES/eskiss

    if [ $CONFIGURE == "1" ]; then
        MYCFLAGS="-mminimal-toc" ps3-configure --datadir=$ESKISS_DATADIR
    fi
    make $CLEAN_RULE all EDJE_CC=edje_cc && \
        cp src/bin/eskiss eskiss.elf && sprxlinker eskiss.elf && \
        $FSELF eskiss.elf eskiss.self && \
        mkdir -p pkg/USRDIR && cp data/edje/title.png pkg/ICON0.PNG && \
        make_self_npdrm eskiss.elf pkg/USRDIR/EBOOT.BIN UP0001-ESKISS_00-0000000000000000 && \
        sfo.py --title "Eskiss" --appid "ESKISS" -f ps3sfo.xml pkg/PARAM.SFO  && \
        make install DESTDIR=`pwd`/temp_install && \
        cp -rf temp_install/$ESKISS_DATADIR/eskiss pkg/USRDIR/  && \
        rm -rf temp_install && \
        pkg.py --contentid UP0001-ESKISS_00-0000000000000000 pkg/ eskiss.pkg && \
        cp eskiss.pkg eskiss.retail.pkg && package_finalize eskiss.retail.pkg && \
        rm -rf pkg && \
        cd ..
}

# elementary
function elementary {
## TODO!! remove dlopen
    cd elementary
    if [ $CONFIGURE == "1" ]; then
        ps3-configure --disable-quick-launch --disable-elementary-config --disable-elementary-test
    fi
    make $CLEAN_RULE all EDJE_CC=edje_cc EET_EET=eet && \
        ps3-smi && \
        cd ..

}

# efreet eio eeze e_dbus -- not needed
# azy ethumb -- not needed
# exchange e -- not needed

# eem
function eem {
    cd eem && \
        AR="powerpc64-ps3-elf-ar" CC="powerpc64-ps3-elf-gcc" RANLIB="powerpc64-ps3-elf-ranlib" CFLAGS="-O2 -Wall -I$PSL1GHT/ppu/include -I$PS3DEV/portlibs/ppu/include $MYCFLAGS" CXXFLAGS="-I$PSL1GHT/ppu/include -I$PS3DEV/portlibs/ppu/include"  LDFLAGS="-L$PSL1GHT/ppu/lib -L$PS3DEV/portlibs/ppu/lib" PKG_CONFIG_LIBDIR="$PSL1GHT/ppu/lib/pkgconfig" PKG_CONFIG_PATH="$PS3DEV/portlibs/ppu/lib/pkgconfig" make LIBS="-lescape -leina -ledje -lembryo -levas -leet -lfreetype -ljpeg -lpng -lz -lm -lSDL -lio -laudio -lgcm_sys -lrsx -lgcm_sys -lm -lsysutil -lfreetype -lescape -lecore -leina -lnet -lsysutil -liberty -lecore_imf -lsysmodule -lm -llua -lm -lecore_imf_evas -lecore_evas -lecore_file -levas -leina -lescape -liberty"
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
    ELAPSED=`expr $END_TIME - $START_TIME`
    echo "Done (Elapsed : " `_hms $ELAPSED` ")"
    cd $pwd
}

sudo pwd > /dev/null

start=`date +%s`
if test -z $1 ; then
    build escape
    build eina
    build eet
    if [ $ENABLE_FONTCONFIG == "1" ]; then
        build expat
        build fontconfig
    else
        pwd=$(pwd)
        cd fontconfig-2.8.0 && \
        sudo make uninstall >& /dev/null
        cd $pwd
    fi
    build evas
    build expedite
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
