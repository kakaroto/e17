#!/bin/sh

PATH=/usr/devel/bin:/usr/bin/:/bin

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
            gcc -o $builddir/$1 $f `PKG_CONFIG_PATH=/usr/devel/lib/pkgconfig:/usr/lib/pkgconfig:/usr/share/pkgconfig:/usr/local/lib64/pkgconfig/ pkg-config elementary --cflags --libs`

        done
}
else
    xelatex -output-driver "xdvipdfmx -V5 -z9 -E" edje_tutorial.tex

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
            gcc -o $builddir/$i $f `PKG_CONFIG_PATH=/usr/devel/lib/pkgconfig:/usr/lib/pkgconfig:/usr/share/pkgconfig:/usr/local/lib64/pkgconfig/ pkg-config elementary --cflags --libs`
        done


    done
fi

