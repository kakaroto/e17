#!/bin/bash
#===============================================================================
#
#          FILE:  build.sh
#
#         USAGE:  ./build.sh
#
#   DESCRIPTION:
#
#       OPTIONS:  ---
#  REQUIREMENTS:  ---
#          BUGS:  ---
#         NOTES:  ---
#        AUTHOR:   (),
#       COMPANY:
#       VERSION:  1.0
#       CREATED:  05/10/2008 09:49:42 PM CEST
#      REVISION:  ---
#===============================================================================

gcc -o viewer viewer.c `pkg-config --cflags --libs evas ecore ecore-file ecore-evas edje` -Wall -g
