#! /bin/sh

DB="edb_ed $1 add"
STR="str"

$DB "*.ppm"     $STR "image/ppm"
$DB "*.pgm"     $STR "image/pgm"
$DB "*.pbm"     $STR "image/pbm"
$DB "*.pnm"     $STR "image/pnm"
$DB "*.rgba"    $STR "image/rgba"
$DB "*.bmp"     $STR "image/bmp"
$DB "*.xpm"     $STR "image/xpm"

$DB "*.ttf"     $STR "font/true-type"

$DB "*.html"    $STR "text/html"
$DB "*.htm"     $STR "text/html"
$DB "*.txt"     $STR "text/plain"
$DB "README"    $STR "text/readme"
$DB "INSTALL"   $STR "text/install"
$DB "COPYING"   $STR "text/copyright"
$DB "NEWS"      $STR "text/news"
$DB "TODO"      $STR "text/todo"
$DB "configure" $STR "text/configure"
$DB "*.c"       $STR "text/c"
$DB "*.cc"      $STR "text/c++"
$DB "*.cpp"     $STR "text/c++"
$DB "*.c++"     $STR "text/c++"
$DB "*.h"       $STR "text/h"
$DB "*.h++"     $STR "text/h"
$DB "Makefile"  $STR "text/makefile"
$DB "*~"        $STR "document/backup"

$DB "*.epplet"  $STR "application/enlightenment-epplet"
