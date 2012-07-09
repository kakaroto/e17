#!/bin/sh

echo Updating EFL Packages

svn update

CFLAGS="-02 -g -W -Wall -Wextra -Wundef -Wshadow"

for e in eina eet evas_generic_loaders evas ecore eeze efreet embryo edje e_dbus eio emotion PROTO/epdf ethumb elementary e expedite clouseau terminology ephoto
do 
	if [ -e $e ] && [ -d $e ]
	then 
		echo Start $e 
		cd $e
		sudo make clean
		sudo make distclean
		./autogen.sh
		CFLAGS=${CFLAGS} sudo make install && sudo ldconfig
		cd ..
		echo End Done
	fi 
done

exit 0
