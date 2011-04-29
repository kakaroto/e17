#!/bin/sh

for i in `ls *.svg` ; do

name=`echo $i | cut -d'.' -f1`
echo $name


convert -background None +antialias -density 384 $i $name-512.png
convert -background None +antialias -density 192 $i $name-256.png
convert -background None +antialias -density 96 $i $name-128.png
convert -background None +antialias -density 48 $i $name-64.png
convert -background None +antialias -density 24 $i $name-32.png
convert -background None +antialias -density 12 $i $name-16.png

done