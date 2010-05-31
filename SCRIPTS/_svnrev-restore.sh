#!/bin/sh

set -e

PROJ=$1
cd $PROJ
sed 's/m4_undefine(\[v_rev\])/dnl m4_undefine(\[v_rev\])/' \
< configure.ac > .tmp
mv .tmp configure.ac
exit 0
