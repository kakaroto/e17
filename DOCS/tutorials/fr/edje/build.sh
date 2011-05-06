#!/bin/sh

topdir="./"
builddir="./build"


if [ -e "$1" ]; then
{
    echo "building $1"
        for f in `ls $1/*.edc`;  do
            file=`basename $f`
            file=$(echo $file | cut -f1 -d '.')
            echo "building $f"
            edje_cc -id $topdir/images $f -o $builddir/$file.edj
        done

        for f in `ls $1/*.c`; do
            file=`basename $f`
            file=$(echo $file | cut -f1 -d '.')
            echo "building $f"
            gcc -o $builddir/$1 $f `pkg-config elementary --cflags --libs`

        done
}
else
    for i in `ls -d tut*`; do
        echo "building $i"

        for f in `ls $i/*.edc`; do
            file=`basename $f`
            file=$(echo $file | cut -f1 -d '.')
            echo "building $f"
            edje_cc -id $topdir/images $f -o $builddir/$file.edj
        done


        for f in `ls $i/*.c`; do
            file=`basename $f`
            file=$(echo $file | cut -f1 -d '.')
            echo "building $f"
            gcc -o $builddir/$i $f `pkg-config elementary --cflags --libs`
        done


    done
fi

