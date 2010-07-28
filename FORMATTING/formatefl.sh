#!/bin/sh
UNC="$HOME/bin/uncrustify"
if [ -z "$1" ]; then
  echo "==================================================================="
  echo " Checking if uncrustify installed already and is the right version"
  echo "==================================================================="
  VER=`$UNC --version | awk '{printf("%s\n", $2);}'`
  if [ "$VER" != "0.56" ]; then
  echo "==================================================================="
  echo " Not there or wrong version. Need to install it in ~/bin"
  echo " Doing that now."
  echo "==================================================================="
    tar zxf uncrustify-0.56.tar.gz
    cd uncrustify-0.56
      ./configure --prefix=$HOME
      make
      make install
    cd ..
    rm -rf uncrustify-0.56
    ln -sf $PWD/efl_uncrustify.cfg $HOME/.uncrustify.cfg
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
F=`find $1 -name '*.[ch]' -print`
if [ -n "$F" ]; then $UNC --no-backup --replace -l C $F; fi
F=`find $1 -name '*.h.in' -print`
if [ -n "$F" ]; then $UNC --no-backup --replace -l C $F; fi
F=`find $1 -name '*.cpp' -print`
if [ -n "$F" ]; then $UNC --no-backup --replace -l CPP $F; fi
  echo "==================================================================="
  echo " DONE"
  echo "==================================================================="
exit 0
