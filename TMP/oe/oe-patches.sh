#!/bin/sh -e
mkdir -p patches-applied || true
cd org.openembedded.dev
for I in `/bin/ls ../patches | sort`; do
  if test -f "../patches-applied/$I"; then
    echo "SKIP: $I"
  else
    echo "PATCH: $I"
    patch -p1 -t < "../patches/$I" || true
    touch "../patches-applied/$I"
  fi
done
cd ..
