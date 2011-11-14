#!/bin/bash
DIR="$1"
SCRIPTDIR=$(dirname "$0")
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
    pushd "$SCRIPTDIR"
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
#    sed -r 's/^([a-zA-Z].*\*?) ([a-z][a-zA-Z0-9_]*\()/\1\n\2/' < $X > $T
#    cp $T $X
#    sed -r 's/{$/\n{/' < $X > $T
#    cp $T $X
#    rm $T
    X="$1"
  done
}

INDENTOPS="-gnu -l9999 -lc9999 -ncs -npcs -nprs"

function fmtc ()
{
  if [[ -z "$@" ]]; then return; fi
  indent $INDENTOPS $@
  funcnewlines $@
  $UNC -c $HOME/.ecrustify.cfg --no-backup --replace -l C $@
}

function fmtcpp ()
{
  if [[ -z "$@" ]]; then return; fi
  indent $INDENTOPS $@
  funcnewlines $@
  $UNC -c $HOME/.ecrustify.cfg --no-backup --replace -l CPP $@
}

function fmth ()
{
  if [[ -z "$@" ]]; then return; fi
  indent $INDENTOPS $@
  funcnewlines $@
  $UNC -c $HOME/.ecrustify-headers.cfg --no-backup --replace -l C $@
}

function fmthpp ()
{
  if [[ -z "$@" ]]; then return; fi
  indent $INDENTOPS $@
  funcnewlines $@
  $UNC -c $HOME/.ecrustify-headers.cfg --no-backup --replace -l CPP $@
}

fmtc `find $1 -name '*.c' -print`
fmtcpp `find $1 -name '*.cpp' -print`
fmth `find $1 -name '*.h' -print`
fmthpp `find $1 -name '*.hpp' -print`
fmth `find $1 -name '*.h.in' -print`
fmthpp `find $1 -name '*.hpp.in' -print`

  echo "==================================================================="
  echo " DONE"
  echo "==================================================================="
exit 0
