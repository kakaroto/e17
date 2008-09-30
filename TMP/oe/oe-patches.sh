#!/bin/sh
cd org.openembedded.dev
for I in `/bin/ls ../patches/* | sort`; do
  echo "PATCH: $I"
  patch -p1 -t < $I
done
cd ..
