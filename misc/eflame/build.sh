USE_ECORE_EVAS=`ecore-config --libs | grep evas`
if [ -n "USE_ECORE_EVAS" ] ; then gcc -O2 -Wall `ecore-config --cflags --libs` eflame.c -o eflame;
else echo Error: you must compile Ecore with Evas support;
fi
