#!/bin/sh

for file in `find -name "*.[hc]"`; do
  echo "Re-indenting '$file' to Rasterman E style"
  indent -i3 -bl -bad -nbap -sob -ncdb -di20 -nbc -lp -nce -npcs -sc -ncs -l80 "$file"
done
