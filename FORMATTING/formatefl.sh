#!/bin/bash
DIR="$1"
SCRIPTDIR="$0"
PREFIX="$HOME/.uncrustify"

if which uncrustify &> /dev/null ;then
	UNC="$(which uncrustify)"
else
	UNC="$PREFIX/bin/uncrustify"
fi
if [[ -z "$DIR" ]]; then
  echo "==================================================================="
  echo " Checking if uncrustify installed already and is the right version"
  echo "==================================================================="
  VER=$($UNC --version | awk '{printf("%s\n", $2);}')
  if [[ "$VER" != "0.56" ]]; then
    echo "==================================================================="
    echo " Not there or wrong version. Need to install it in ~/.uncrustify"
    echo " Doing that now."
    echo "==================================================================="
    pushd $(dirname $SCRIPTDIR)
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
FMT1="true"
FMT2="$UNC --no-backup --replace -l C"
FMT3="$UNC --no-backup --replace -l CPP"
#FMT2="indent -kr"
#FMT3="indent -kr"
F=$(find $1 -name '*.[chx]' -print)
if [[ -n "$F" ]]; then $FMT1 $F; $FMT2 $F; fi
F=$(find $1 -name '*.h.in' -print)
if [[ -n "$F" ]]; then $FMT1 $F; $FMT2 $F; fi
F=$(find $1 -name '*.cpp' -print)
if [[ -n "$F" ]]; then $FMT1 $F; $FMT3 $F; fi
  echo "==================================================================="
  echo " DONE"
  echo "==================================================================="
exit 0
