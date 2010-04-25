#!/usr/bin/env bash

dest=./css
#style="compressed"
style="expanded"


for i in *.sass;
do
    sass --style $style $i > $dest/${i%.sass}.css
done