PROJ="e TMP/st/elementary"
if test "$#" -eq 1; then
  if test -n "$1"; then
    PROJ="$1"
  fi
else
  if test "$#" -eq 2; then
    VA=$1
    VB=$2
  fi
fi
