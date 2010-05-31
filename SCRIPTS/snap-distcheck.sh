#!/bin/sh

set -e

. ./SCRIPTS/_list_snap.sh

for I in $PROJ; do
  ./SCRIPTS/_distcheck.sh $I
done

exit 0
