#!/bin/sh

set -e

. ./SCRIPTS/_list_release.sh

for I in $PROJ; do
  ./SCRIPTS/_distcheck.sh $I
done

exit 0
