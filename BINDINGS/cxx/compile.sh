#!/bin/bash

PKG="eflxx
     einaxx
     evasxx
     eetxx
     edjexx
     ecorexx
     emotionxx
     elementaryxx
     eflxx_examples"

BASEDIR=`pwd`

## some defaults...
val_bootstrap=true
val_configure=true
val_skip_error=false
val_clean=false
val_make=true
val_debug=true
val_install=false

## include all needed functions
source compile.functions

# do tests
for i
	do case "$i" in
	--help) print_help; 	        exit 0 ;;
	--no-bootstrap)               val_bootstrap=false ;;
	--no-configure)               val_configure=false ;;
	--no-debug)                   val_debug=false ;;
	--skip-error)                 val_skip_error=true ;;
	--clean)                      val_clean=true ;;
	--no-make)                    val_make=false ;;
	--install)		      val_install=true ;;

	*)
    echo "$0: wrong parameter"
    echo "--help to show help" >&2; exit 1 ;;

	esac;
done

## create log directory
rm -rf logs
mkdir -p logs

for module in $PKG
do
  $val_bootstrap && do_bootstrap $module

  ($val_configure || $val_bootstrap) && do_configure $module

  $val_clean && do_make $module clean
  $val_make && do_make $module all
  $val_install && do_make $module install

done


