#!/bin/sh
cat << __EOF__
<page columns=1 padding=16 name=front background=Edoc_bg.png linkcolor=#88dddd>
<font face=aircut3/12 color=#ffffff>
__EOF__
# find the users .xinitrc, .xsession, .Xclients
EXISTS="0"
if [ -e ~/.xsession ]; then
  EXISTS="1"
fi
if [ -e ~/.xinitrc ]; then
  EXISTS="1"
fi
if [ -e ~/.Xclients ]; then
  EXISTS="1"
fi
E_BIN=$E_BINDIR/enlightenment
E_EESH=$E_BINDIR/eesh
E_DOX=$E_BINDIR/dox
E_DATA=$E_BINDIR/enlightenment


if [ $EXISTS = "1" ]; then
cat << __EOF__
<p align=50%>
It appears you already have some of your own X Login startup scripts (such as 
.xsession, .xinitrc and .Xclients). If you have no idea what this is all
about just click Contiue below and everything will be taken care of for you.
<p align=50%>
If you have customised these files and wish to keep your modifications, it is 
suggested that you review each or all of them and manually ensure they run
$E_BIN as the window manager on the last line of your script(s).
<p align=50%>
If you wish to abort just close this window.
<p align=50%>
_Contiune(INPUT.first16-2.sh)
__EOF__
else
cat << __EOF__
<p align=50%>
You appear to have no custom X Login setup for your user. If you click on
Contiue below Enlightenment will set everything up for you
<p align=50%>
If you wish to abort just close this window.
<p align=50%>
_Contiune(INPUT.first16-2.sh)
__EOF__
fi
