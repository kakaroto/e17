#!/bin/sh
set -e

function autogen_lib {
    what=$1
    echo -ne "Autogen-ing $what : "
    pwd=$(pwd)
    cd $what && \
        NOCONFIGURE=1 ./autogen.sh > ../logs/autogen_${what}.log 2>&1  || \
        (echo "Error!" && \
          cd $pwd && \
          (tail logs/autogen_${what}.log || true) && \
          echo -ne "\n\nSee logs/autogen_${what}.log for details.\n" && \
          exit 1)
    echo "Done!"
    cd $pwd
}

function download {
    what=$1
    if test -d $what ; then
        rm -rf $what
    fi
    echo -ne "Downloading $what : "
    svn export http://svn.enlightenment.org/svn/e/trunk/$what > logs/svn_${what}.log  2>&1 || \
        (echo "Error!" && \
          (tail logs/svn_${what}.log  || true ) && \
          echo -ne "\n\n See logs/svn_${what}.log for details.\n" && \
          exit 1)
    echo "Done!"
}

function tarball {
    what=$1
    echo -ne "Tarball-ing $what : "
    tar -czvf ${what}_svn${EFL_VERSION}.tar.gz ${what} > logs/tarball_${what}.log 2>&1 || \
        (echo "Error!" && \
          (tail logs/tarball_${what}.log  || true ) && \
          echo -ne "\n\n See logs/tarball_${what}.log for details.\n" && \
          exit 1)
    echo "Done!"
}

mkdir logs > /dev/null 2>&1 || true

EFL_LIBS=""
if test -z $1 ; then
    EFL_LIBS="eina eet evas ecore embryo edje efreet e_dbus eeze"
else
    while test -n "$1"; do
        EFL_LIBS="$EFL_LIBS $1"
        shift
    done
fi

echo -ne "Getting SVN revision : "
EFL_VERSION=$(LC_ALL=C svn info http://svn.enlightenment.org/svn/e/trunk/ | grep Revision | awk '{print $2}')
echo "${EFL_VERSION}"

# Download, autogen and tarball
for lib in $EFL_LIBS ; do
    download $lib
    autogen_lib $lib
    tarball $lib
    rm -rf $lib
done

