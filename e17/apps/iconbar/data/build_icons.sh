#!/bin/sh

DATA_PATH="./"
TEMPLATE="$DATA_PATH/template.edc"

setup()
{
  rm -f $DATA_PATH/order.txt
  touch $DATA_PATH/order.txt
}

icon()
{
  NAME="$1.app.eet";
  FILE=`basename $2`
  IMDIR=`echo $2 | sed -e"s/$FILE//"`
  EXEC="exec $3"
  
  cat $TEMPLATE | sed -e"s/IMAGENAME/$FILE/" | sed -e"s/EXEC/$EXEC/" > temp.edc
  edje_cc -id $IMDIR temp.edc $DATA_PATH/icons/$NAME
  rm -f temp.edc
  echo $NAME >> $DATA_PATH/order.txt
}
setup

############ Create your icons here ##################3
# usage: icon "name" "icon path" "executable"
# icon files are called name.app.eet, so name must be unique for each icon

USRIC="/usr/local/share/iconbar/images"
icon "eterm"     $USRIC"/term-32.png"          "Eterm"
icon "galeon"    $USRIC"/globe-32.png"         "galeon"
icon "evolution" $USRIC"/mail-32.png"          "evolution"
icon "gimp"      $USRIC"/paint-32.png"         "gimp-1.3"
icon "gaim"      $USRIC"/chat-32.png"          "gaim"
icon "irc"       $USRIC"/irc-32.png"           "xchat"
icon "xmms"      $USRIC"/speaker-32.png"       "xmms"
icon "gnumeric"  $USRIC"/spreadsheet-32.png"   "gnumeric"
icon "abiword"   $USRIC"/write-32.png"         "abiword"

