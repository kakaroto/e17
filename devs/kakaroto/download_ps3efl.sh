#!/bin/bash
set -e

function efl {
    if test ! -d e17_src ; then
        git clone git://github.com/kakaroto/e17.git e17_src
        ln -s e17_src/PROTO/escape escape
        ln -s e17_src/eina eina
        ln -s e17_src/eet eet
        ln -s e17_src/evas evas
        ln -s e17_src/expedite expedite
        ln -s e17_src/ecore ecore
        ln -s e17_src/embryo embryo
        ln -s e17_src/edje edje
        ln -s e17_src/GAMES/eskiss eskiss
        ln -s e17_src/elementary elementary
        ln -s e17_src/devs/kakaroto/build_ps3efl.sh build_ps3efl.sh
    fi
}
function expat {
    if test ! -d expat-2.0.1 ; then
        wget -O expat-2.0.1.tar.gz http://sourceforge.net/projects/expat/files/expat/2.0.1/expat-2.0.1.tar.gz/download && \
            tar -xzf expat-2.0.1.tar.gz 
    fi
}
function fontconfig {
    if test ! -d expat-2.0.1 ; then
        wget http://freedesktop.org/software/fontconfig/release/fontconfig-2.8.0.tar.gz  && \
            tar -xzf fontconfig-2.8.0.tar.gz 
    fi
}
function lua {
    if test ! -d lua-5.1.4 ; then
        wget http://www.lua.org/ftp/lua-5.1.4.tar.gz && \
            tar -xzf lua-5.1.4.tar.gz 
    fi
}
function cares {
    if test ! -d c-ares-1.7.4 ; then
        wget -O c-ares-1.7.4.tar.gz http://c-ares.haxx.se/download/c-ares-1.7.4.tar.gz && \
            tar -xzf c-ares-1.7.4.tar.gz 
    fi
}
function chipmunk {
    if test ! -d Chipmunk-5.3.5 ; then
        wget http://chipmunk-physics.net/release/Chipmunk-5.x/Chipmunk-5.3.5.tgz && \
            tar -xzf Chipmunk-5.3.5.tgz 
    fi
}

function msg {
    message=$1
    len=$(expr length "$message")
    halflen=$(expr \( 80 - \( $len + 6 \) \) / 2)
    for ((i=0 ; i < 80; i++)); do
        echo -ne "*"
    done
    echo ""
    for ((i=0 ; i < $halflen; i++)); do
        echo -ne "*"
    done
    echo -ne "   $message   "
    if test "$(expr $len % 2)" == "1"; then
        echo -ne " "
    fi
    for ((i=0 ; i < $halflen; i++)); do
        echo -ne "*"
    done
    echo ""
    for ((i=0 ; i < 80; i++)); do
        echo -ne "*"
    done
    echo ""
}

function download {
    what=$1
    msg "Downloading $what"
    $what || (msg "Error downloading $what!" && exit 1)
    msg "Download of $what done!"
}

function patch_dir {
    what=$1
    msg "Patching '$what'"
    pwd=`pwd`
    cd $what && \
        patch -N -p0 < ../e17_src/devs/kakaroto/$what || true
    msg "Patching of $what done!"
    cd $pwd
}

function autogen_lib {
    what=$1
    echo -ne "Autogen $what : "
    pwd=$(pwd)
    cd $what && \
        NOCONFIGURE=1 ./autogen.sh >& ../autogen_${what}.log  || \
        (echo "Error!" && \
          cd $pwd && \
          (tail autogen_${what}.log || true) && \
          echo -ne "\n\nSee autogen_${what}.log for details.\n" && \
          exit 1)
    echo "Done"
    cd $pwd
}

# Setup dirs
download efl
download expat
download fontconfig
download cares
download lua
download chipmunk

# Patch
patch_dir c-ares-1.7.4
patch_dir lua-5.1.4

# Autogen
autogen_lib escape
autogen_lib eina
autogen_lib eet
autogen_lib evas
autogen_lib expedite
autogen_lib ecore
autogen_lib embryo
autogen_lib edje
autogen_lib eskiss
autogen_lib elementary

rm *.log