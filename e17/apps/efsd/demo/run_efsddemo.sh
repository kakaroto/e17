#! /bin/bash

rm -f *-is-flim.demo
touch raster-is-flim.demo

LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../efsd/.libs ./efsddemo $@
