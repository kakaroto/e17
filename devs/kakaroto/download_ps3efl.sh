#!/bin/bash
set -e

function eina {
    svn checkout http://svn.enlightenment.org/svn/e/trunk/eina
}
function eet {
    svn checkout http://svn.enlightenment.org/svn/e/trunk/eet
}
function evas {
    svn checkout http://svn.enlightenment.org/svn/e/trunk/evas 
}
function expedite {
    svn checkout http://svn.enlightenment.org/svn/e/trunk/expedite 
}
function ecore {
    svn checkout http://svn.enlightenment.org/svn/e/trunk/ecore 
}
function embryo {
    svn checkout http://svn.enlightenment.org/svn/e/trunk/embryo 
}
function edje {
    svn checkout http://svn.enlightenment.org/svn/e/trunk/edje 
}
function eskiss {
    svn checkout http://svn.enlightenment.org/svn/e/trunk/GAMES/eskiss 
}
function elementary {
    svn checkout http://svn.enlightenment.org/svn/e/trunk/elementary 
}
function expat {
    wget -O expat-2.0.1.tar.gz http://sourceforge.net/projects/expat/files/expat/2.0.1/expat-2.0.1.tar.gz/download && \
    tar -xzvf expat-2.0.1.tar.gz 
}
function fontconfig {
    wget http://freedesktop.org/software/fontconfig/release/fontconfig-2.8.0.tar.gz  && \
        tar -xzvf fontconfig-2.8.0.tar.gz 
}
function lua {
    wget http://www.lua.org/ftp/lua-5.1.4.tar.gz && \
        tar -xzvf lua-5.1.4.tar.gz 
}
function chipmunk {
    wget http://chipmunk-physics.net/release/Chipmunk-5.x/Chipmunk-5.3.5.tgz && \
        tar -xzvf Chipmunk-5.3.5.tgz 
}
function escape {
    wget http://dl.dropbox.com/u/22642664/escape.tar.gz && \
        tar -xzvf escape.tar.gz 
}
function efl_diffs {
    wget http://dl.dropbox.com/u/22642664/efl_diffs.tar.gz  && \
        tar -xzvf efl_diffs.tar.gz 
}
function efl_build {
    wget http://dl.dropbox.com/u/22642664/efl_build && \
        chmod +x efl_build
}

function download {
    what=$1
    echo -ne "Downloading $what : "
    $what >& download_${what}.log || \
        (echo "Error!" && \
          (tail download_${what}.log || true) && \
          echo -ne "\n\nSee download_${what}.log for details.\n" && \
          exit 1)
    echo "Done"
}

function patch_lib {
    what=$1
    echo -ne "Patching $what : "
    pwd=$(pwd)
    cd $what && \
        patch -p0 < ../efl_diffs/$what >& ../patching_${what}.log  || \
        (echo "Error!" && \
          cd $pwd && \
          (tail patching_${what}.log || true) && \
          echo -ne "\n\nSee patching_${what}.log for details.\n" && \
          exit 1)
    echo "Done"
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
download eina
download eet
download evas
download expedite
download ecore
download embryo
download edje
download eskiss
download elementary
download expat
download fontconfig
download lua
download chipmunk
download escape
download efl_diffs
download efl_build

# Patch
for f in efl_diffs/*; do
    patch_lib $(basename $f)
done

# Autogen
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