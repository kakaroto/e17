#!/bin/bash

tocxx=${1:-""};

C_Comm()
{
fn=${1:?"Supply filename to convert"};
cpp=${2:-""};
tfn="/tmp/${fn}.o";

echo "Moving $fn to $tfn";
mv $fn $tfn;
mv_rc=$?;

if [ $mv_rc -ne 1 ]; then

        if [ "X${cpp}" = "X" ]; then
                echo Converting $fn to ANSI C style comments.
                cat $tfn | sed -e 's/\/\/\(.*\)/\/*\1*\//g' >$fn;
        else    
                echo Converting $fn to C++ or C99 compliant comments.
                cat $tfn | sed -e 's/\/\*\(.*\)\*\//\/\/\1/g' >$fn;
        fi;
        rm -f  $tfn;

fi;
}


dodir()
{
for f in *.c *.h;
do
	echo $f;
	C_Comm $f $tocxx;
done;
}

d=`pwd`;

cd $d/src;   dodir;
cd $d/demo;  dodir;

