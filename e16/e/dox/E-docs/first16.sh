#!/bin/sh
cat << __EOF__
<page columns=1 padding=16 name=front background=Edoc_bg.png linkcolor=#88dddd>
<font face=aircut3/12 color=#ffffff>
<p align=50%>
How old are you?
<br>
<br>
_1-12(INPUT.first16-age-12.sh)<br>
_13-15(INPUT.first16-age-15.sh)<br>
_16-18(INPUT.first16-age-18.sh)<br>
_19-21(INPUT.first16-age-21.sh)<br>
_22-27(INPUT.first16-age-27.sh)<br>
_28-35(INPUT.first16-age-35.sh)<br>
_36-45(INPUT.first16-age-45.sh)<br>
_46-59(INPUT.first16-age-59.sh)<br>
_60+(INPUT.first16-age-60+.sh)<br>
__EOF__
cat << __EOF__
