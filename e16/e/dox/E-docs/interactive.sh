#!/bin/sh
cat << __EOF__
<page columns=1 padding=16 name=front background=Edoc_bg.png linkcolor=#88dddd>
<img src=E_logo.png x=32 y=77>
<font face=rothwell/25 color=#ffffff>
<p align=50%>
Hints and tips
<p align=50%>
<font face=aircut3/12 color=#ffffff>
<p>
__EOF__
# checking OS & arch info
UNAME=`uname -a`
OS=`echo "$UNAME" | awk '{print $1;}'`
OSVER=`echo "$UNAME" | awk '{print $3;}'`
ARCH=`echo "$UNAME" | awk '{print $12;}'`

# checking memory stats
MEM=`free`
TOTAL=`echo "$MEM" | grep Mem: | awk '{print $2;}'`
USED=`echo "$MEM" | grep buffers/cache | awk '{print $3;}'`
SWAPPED=`echo "$MEM" | grep Swap: | awk '{print $3;}'`

# checking X Server 
XDPYINFO=`xdpyinfo`
VENDOR=`echo "$XDPYINFO" | grep "vendor string:" | awk -F: '{print $2;}'`
VERSION=`echo "$XDPYINFO" | grep "vendor release number:" | awk -F: '{print $2;}'`
DEPTH=`echo "$XDPYINFO" | grep "depths (" | awk -F: '{print $2;}' | awk '{print $1;}'`
DEPTHS=`echo "$XDPYINFO" | grep "depths (" | awk "-F(" '{print $2;}' | awk "-F)" '{print $1;}'`

echo "Your Operating System is:<br>"
echo "<font face=aircut3/12 color=#ccaacc>"
echo $OS"<br>"
echo "<font face=aircut3/12 color=#ffffff>"
echo "Your processor type is:<br><font face=aircut3/12 color=#ccaacc>"
echo $ARCH"<br><font face=aircut3/12 color=#ffffff>"
echo "Your system memory use is:<br><font face=aircut3/12 color=#ccaacc>"
echo $USED" Kb /"$TOTAL" Kb<br>"
echo $SWAPPED" Kb swapped out to disk<br><font face=aircut3/12 color=#ffffff>"

echo "Your X Server is:<br><font face=aircut3/12 color=#ccaacc>"
echo $VENDOR"<br>"
echo "<font face=aircut3/12 color=#ffffff>"
echo "Version:<br><font face=aircut3/12 color=#ccaacc>"
echo $VERSION"<br><font face=aircut3/12 color=#ffffff>"
echo "<font face=aircut3/12 color=#ffffff>"
echo "Running a color depth of:<br><font face=aircut3/12 color=#ccaacc>"
echo $DEPTH" bits per pixel<br><font face=aircut3/12 color=#ffffff>"
echo "Number of depths:<br><font face=aircut3/12 color=#ccaacc>"
echo $DEPTHS"<br><font face=aircut3/12 color=#ffffff>"

cat << __EOF__
<p>
Hit Next for more...
<page columns=1 padding=16 name=2 background=Edoc_bg.png linkcolor=#88dddd>
<font face=aircut3/12 color=#ffffff>
<p align=50%>
<p>
__EOF__
if [ "$DEPTHS" -gt 1 ]; then
echo "It seems you have more than one depth available on your display."
echo "Enlightenment will run but several features may be disabled or not work"
echo "correctly due to this."
echo "<p>"
echo "Please re-configure your X Server to only have one depth (eg 16, 24, 32)"
fi
