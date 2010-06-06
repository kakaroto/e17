#!/bin/sh

set -e

. ./SCRIPTS/_list_release.sh

OUT="$PWD/RELEASES"

for I in $PROJ; do
  ./SCRIPTS/_svnrev-remove.sh "$I"
  ./SCRIPTS/_changelog.sh "$I"
done
for I in $PROJ; do
  ./SCRIPTS/_dist.sh "$I" "$OUT"
done
for I in $PROJ; do
  ./SCRIPTS/_svnrev-restore.sh "$I"
  ./SCRIPTS/_post_release.sh "$I"
done

## upload
DATE=`date +'%F'`
SVR="www.enlightenment.org"
DST="/var/www/download/releases"
SSH="ssh -p 995"
SCP="scp -P 995"

$SSH "$SVR" mkdir "$DST" || true
$SCP "$OUT/"* "$SVR:$DST/"

exit 0
