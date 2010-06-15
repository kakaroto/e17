#!/bin/sh

SIZE="32x32"

for i in *.svg ; do
        echo converting $i...
        PNG_NAME=`ls $i | sed s:.svg::`
        convert -resize $SIZE $PNG_NAME.svg "$PNG_NAME"_small.png
done

