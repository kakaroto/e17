#!/bin/sh
mkdir -p patches-applied
cd org.openembedded.dev
for I in `/bin/ls ../patches | sort`; do
  if [ -f "../patches-applied/$I" ]; then
    echo "SKIP: $I"
  else
    echo "PATCH: $I"
    patch -p1 -t < "../patches/$I"
    touch "../patches-applied/$I"
  fi
done
cd ..
