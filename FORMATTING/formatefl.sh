#!/bin/bash
PREFIX="$HOME/.uncrustify"
UNC="$PREFIX/bin/uncrustify"
if [ -z "$1" ]; then
  echo "==================================================================="
  echo " Checking if uncrustify installed already and is the right version"
  echo "==================================================================="
  VER=`$UNC --version | awk '{printf("%s\n", $2);}'`
  if [ "$VER" != "0.56" ]; then
    echo "==================================================================="
    echo " Not there or wrong version. Need to install it in ~/.uncrustify"
    echo " Doing that now."
    echo "==================================================================="
    pushd `dirname $0`
    tar zxf uncrustify-0.56.tar.gz
    pushd uncrustify-0.56
      ./configure --prefix=$PREFIX
      make
      make install
    popd
    rm -rf uncrustify-0.56
    ln -sf $PWD/efl_uncrustify.cfg $HOME/.uncrustify.cfg
    popd
  fi
  echo "==================================================================="
  echo " DONE"
  echo "==================================================================="
  exit 0
fi

echo "==================================================================="
echo " Uncrustifying your source to match EFL programming guidelines for"
echo " formatting etc."
echo "==================================================================="
F=`find $1 -name '*.[chx]' -print`
if [ -n "$F" ]; then $UNC --no-backup --replace -l C $F; fi
F=`find $1 -name '*.h.in' -print`
if [ -n "$F" ]; then $UNC --no-backup --replace -l C $F; fi
F=`find $1 -name '*.cpp' -print`
if [ -n "$F" ]; then $UNC --no-backup --replace -l CPP $F; fi
  echo "==================================================================="
  echo " DONE"
  echo "==================================================================="
exit 0
