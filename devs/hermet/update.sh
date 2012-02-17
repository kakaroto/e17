#!/bin/sh

echo Updating EFL Packages

svn update

CFLAGS="-02 -g -W -Wall -Wextra -Wundef -Wshadow"

for e in eina eet evas ecore eeze efreet embryo edje e_dbus e emotion PROTO/epdf ethumb eio elementary expedite
do 
	if [ -e $e ] && [ -d $e ]
	then 
		echo Start $e 
		cd $e
		sudo make clean
		./autogen.sh
		CFLAGS=${CFLAGS} sudo make install && sudo ldconfig
		cd ..
		echo End Done
	fi 
done

exit 0
