#!/bin/sh

case $1 in
	--force) 
	PREFIX="$2"
	[ -z "$PREFIX" ] && PREFIX="$HOME/usr"
	;;
	*)
	PREFIX="$1"
	[ -z "$PREFIX" ] && PREFIX="$HOME/usr"
	;;
esac

python setup.py build || exit 1
python setup.py install --prefix=$PREFIX || exit 1

exit 0
