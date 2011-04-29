#!/bin/sh

topdir=./
builddir=./build

if [ -e "$1" ]; then
{
    echo "building $1"
    if [ -f $topdir/$1/$1*.edc ]; then
        edje_cc -v -id $topdir/images $topdir/$1/$1.edc -o $builddir/$1.edj
    fi

    if [ -f $topdir/$1/$1*.c ]; then
        gcc -o $builddir/$1 $topdir/$1/$1*.c `pkg-config elementary --cflags --libs`
    fi
}
else
    for i in `ls -d tut*`; do
        echo "building $i"
        if [ -f $topdir/$i/$i*.edc ]; then
            edje_cc -id $topdir/images $topdir/$i/$i.edc -o $builddir/$i.edj
        fi

        if [ -f $topdir/$i/$i*.c ]; then
            gcc -o $builddir/$i $topdir/$i/$i*.c `pkg-config elementary --cflags --libs`
        fi
    done
fi

