#!/bin/bash
DIR="$1"
SCRIPTDIR="$0"
PREFIX="$HOME/.ecrustify"

if which ecrustify &> /dev/null ;then
  UNC="$(which ecrustify)"
  VER=$($UNC --version | awk '{printf("%s\n", $2);}')
  [[ "$VER" = "0.56" ]] || UNC="$PREFIX/bin/ecrustify"
else
  UNC="$PREFIX/bin/ecrustify"
fi
if [[ -z "$DIR" ]]; then
  echo "==================================================================="
  echo " Checking if ecrustify installed already and is the right version"
  echo "==================================================================="
  VER=$($UNC --version | awk '{printf("%s\n", $2);}')
  if [[ "$VER" != "0.56" ]]; then
    echo "==================================================================="
    echo " Not there or wrong version. Need to install it in ~/.ecrustify"
    echo " Doing that now."
    echo "==================================================================="
    pushd $(dirname $SCRIPTDIR)
    pushd ecrustify
      ./autogen.sh --prefix=$PREFIX
      make
      make install
    popd
    ln -sf $PWD/ecrustify.cfg $HOME/.ecrustify.cfg
    ln -sf $PWD/ecrustify-headers.cfg $HOME/.ecrustify-headers.cfg
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
function funcnewlines ()
{
  T="/tmp/tmpfl"
  X="$1"
  while [ -n "$X" ]; do
    shift
    echo "Fixing function newlines in $X..."
    sed -r 's/^([a-zA-Z].*\*?) ([a-z][a-zA-Z0-9_]*\()/\1\n\2/' < $X > $T
    cp $T $X
    rm $T
    X="$1"
  done
}
FMT1="funcnewlines"
FMT2="$UNC -c $HOME/.ecrustify.cfg --no-backup --replace -l C"
FMT3="$UNC -c $HOME/.ecrustify.cfg --no-backup --replace -l CPP"
FMTH="$UNC -c $HOME/.ecrustify-headers.cfg --no-backup --replace -l C"

F=$(find $1 -name '*.c' -print)
if [[ -n "$F" ]]; then $FMT1 $F; $FMT2 $F; fi
F=$(find $1 -name '*.cpp' -print)
if [[ -n "$F" ]]; then $FMT1 $F; $FMT3 $F; fi

F=$(find $1 -name '*.[hx]' -print)
if [[ -n "$F" ]]; then $FMT1 $F; $FMTH $F; fi
F=$(find $1 -name '*.h.in' -print)
if [[ -n "$F" ]]; then $FMT1 $F; $FMTH $F; fi

  echo "==================================================================="
  echo " DONE"
  echo "==================================================================="
exit 0
