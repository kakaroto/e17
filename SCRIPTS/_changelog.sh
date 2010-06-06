#!/bin/sh

set -e

PROJ=$1
cd $PROJ
./autogen.sh || true
make clean distclean || true
./autogen.sh || true
make clean maintainer-clean || true
./autogen.sh

## ok - need a way to fetch this from devs/username/info.txt
## problem - don't knwo what your username is - well not easily. need to
## dig throught .svn/entries file and maybe parse the svn+ssh line?
## then we can get svn username... then we can look in devas/username and get
## stuff from info.txt

V=`grep 'VERSION =' Makefile | tail -1 | awk '{printf("%s\n", $3)};'`
NAME="Carsten Haitzler (The Rasterman)"
DATE=`date +'%F'`
PROJNAME=`basename $PROJ`
LASTLINE=`tail -1 ChangeLog | awk '{printf("%s", $1);}'`
if test -n "$LASTLINE"; then
  echo ""                               >> ChangeLog
fi
echo "$DATE  $NAME"                   >> ChangeLog
echo ""                               >> ChangeLog
echo "        * Release $PROJNAME $V" >> ChangeLog
echo ""                               >> ChangeLog
svn commit -m "Release $PROJNAME $V"
./autogen.sh
pwd
make clean maintainer-clean || true

cd ..
svn copy $PROJ ../tags/$PROJ-$V
cd ../tags
svn commit -m "Tag for $PROJ $V"

exit 0
