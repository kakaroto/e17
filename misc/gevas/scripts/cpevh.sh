#!/bin/sh

dstfile=${1:?"must give new filename name as param 1 (without .c/.h)"};

doit() {
sed s/GEVASEVH_POPUP_/GEVASEVH_CLICKS_/g /tmp/t1 > /tmp/t2
sed s/gevasevh_popup_/gevasevh_clicks_/g /tmp/t2 > /tmp/t1
sed s/GtkgEvasEvHPopup/GtkgEvasEvHClicks/g /tmp/t1 >/tmp/t2
sed s/gevasevh_popup.h/gevasevh_clicks.h/g /tmp/t2 > /tmp/t1
sed s/gevasev_to_popup_/gevasev_clicks_/g /tmp/t1 > /tmp/final
}

cp gevasevh_popup.h /tmp/t1
doit
cp /tmp/final ${dstfile}.h

cp gevasevh_popup.c /tmp/t1
doit
cp /tmp/final ${dstfile}.c
