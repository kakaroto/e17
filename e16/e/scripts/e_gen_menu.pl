#!/bin/sh
##############################################################################
# generates a file.menu format for Enlightenment out of menu hierarchies      #
#
# Copyright (C) 1999-2004 Carsten Haitzler, Geoff Harrison  and various contributors
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies of the Software, its documentation and marketing & publicity
# materials, and acknowledgment shall be given in the documentation, materials
# and software packages that this Software was used.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
##############################################################################

PATH=$PATH:$EBIN:$EROOT/scripts
export PATH

OUT_DIR=$ECONFDIR
OUT=$OUT_DIR"/file.menu"

eesh -e "dialog_ok Rzopoczêto tworzenie Menu... Proszê czekaæ."
WINDOWID=`eesh -ewait window_list | grep Message | awk '{printf("%s", $1);}'`

e_gen_kde_menu menus_kde $OUT_DIR $OUT_DIR"/kde.menu"
e_gen_gnome_menu menus_gnome $OUT_DIR $OUT_DIR"/gnome.menu"
e_gen_gnome_menu menus_gnome $OUT_DIR $OUT_DIR"/gnome_user.menu" $HOME/".gnome/apps"

# if the out dir doesnt exist - make it 
if [ ! -d "$OUT_DIR" ]; then
  mkdir $OUT_DIR
fi

# find eesh
EESH=`which eesh`
# generate file menu if it doesn't exist
if [ ! -f "$OUT" ]; then
  echo \"Menu U¿ytkownika\" > $OUT
  echo \"Menu Aplikacji U¿ytkownia\" NULL menu \"user_apps.menu\" >> $OUT
  echo \"KDE\" NULL menu \"kde.menu\" >> $OUT
  echo \"GNOME\" NULL menu \"gnome.menu\" >> $OUT
  echo \"Menu u¿ytkownika GNOME\" NULL menu \"gnome_user.menu\" >> $OUT
  echo \"Enlightenment Epplets\" NULL menu \"epplets.menu\" >> $OUT
  # if we have eesh available
  if [ -n "$EESH" ]; then
    echo \"Restartuj Enlightenment\" NULL exec \"$EESH -e \'restart\'\" >> $OUT
    echo \"Wyloguj siê\" NULL exec \"$EESH -e \'exit\'\" >> $OUT
  fi
fi
touch $OUT

# generate menu looking for Epplets - you might not have any yet.... but expect to
OUT=$OUT_DIR"/epplets.menu"
EPPLETS=`/bin/ls $EBIN"/"*.epplet 2> /dev/null | awk '{printf("%s ", $1);}'`
if [ -n "$EPPLETS" ]; then
  echo \"Enlightenment Epplets\" > $OUT
  for I in $EPPLETS; do
    NAME=`echo $I | sed 's/.epplet//g'`
    NAME=`basename $NAME`
    echo \"$NAME\" \"$EROOT"/epplet_icons/"$NAME".icon"\" exec \"$I\" >> $OUT
  done
fi
touch $OUT

# generate a stub user_apps menu if it doesnt exist
OUT=$OUT_DIR"/user_apps.menu"
if [ ! -f "$OUT" ]; then
  echo \"Menu Aplikacji U¿ytkownia\" > $OUT
  echo \"Eterm\" NULL exec \"Eterm\" >> $OUT
  echo \"XTerm\" NULL exec \"xterm\" >> $OUT
  echo \"RXVT\" NULL exec \"rxvt\" >> $OUT
  echo \"KTerm\" NULL exec \"kterm\" >> $OUT
  echo \"Gnome Terminal\" NULL exec \"gnome-terminal\" >> $OUT
  echo \"Netscape\" NULL exec \"netscape\" >> $OUT
  echo \"TkRat\" NULL exec \"tkrat\" >> $OUT
  echo \"Netscape Mail\" NULL exec \"netscape -mail -no-about-splash\" >> $OUT
  echo \"Balsa\" NULL exec \"balsa\" >> $OUT
  echo \"Exmh\" NULL exec \"exmh\" >> $OUT
  echo \"Electric Eyes\" NULL exec \"ee\" >> $OUT
  echo \"The GIMP\" NULL exec \"gimp\" >> $OUT
  echo \"XV\" NULL exec \"xv\" >> $OUT
  echo \"GQView\" NULL exec \"gqview\" >> $OUT
  echo \"XMag\" NULL exec \"xmag\" >> $OUT
  echo \"XawTV\" NULL exec \"xawtv\" >> $OUT
  echo \"Imlib Settings\" NULL exec \"imlib_config\" >> $OUT
  echo \"X-Chat\" NULL exec \"xchat\" >> $OUT
  echo \"XMan\" NULL exec \"xman\" >> $OUT
  echo \"TkMan\" NULL exec \"tkman\" >> $OUT
  echo \"GnomeICU\" NULL exec \"gnomeicu -a\" >> $OUT
  echo \"eMusic\" NULL exec \"emusic\" >> $OUT
  echo \"X11Amp\" NULL exec \"x11amp\" >> $OUT
  echo \"XMMS\" NULL exec \"xmms\" >> $OUT
  echo \"FreeAmp\" NULL exec \"freeamp\" >> $OUT
  echo \"Civilization\" NULL exec \"civctp\" >> $OUT
  echo \"Myth 2\" NULL exec \"myth2\" >> $OUT
fi
touch $OUT
eesh -e "win_op "$WINDOWID" close"
eesh -e "reload_menus"
eesh -e "dialog_ok Zakoñczono tworzenie Menu."
