#!/bin/sh

# Copyright (C) 1999-2004 Hallvar Helleseth (hallvar@ii.uib.no)
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

# Author: Hallvar Helleseth (hallvar@ii.uib.no) - Not that I'm proud ;)
# Instructions: move the top window and see what happends to the lower one.
#

# The very wierd and bad way of getting the windowids of the two message
# windows in bash, there must be a better way!

# Open a window then get the windowid of it (hopefully)
eesh -e "dialog_ok Move this window"
window=`eesh -ewait window_list|grep Message`
a=0
for i in $window;do
	a=$(($a + 1))
	if [ $a = 1 ];then
		windowid=$i
	fi
done

# open a new dialog, then get the windowids of all windows named "Message" then
# get the id that's not equal the first window we created
eesh -e "dialog_ok Watch me follow the above window"
window2=`eesh -ewait window_list|grep Message|grep -v $windowid`
a=0
for i in $window2;do
    a=$(($a + 1))
	if [ $a = 1 ];then
      windowid2=$i
	fi
done

# In one endless loop, get window position of the first window, then move the
# second one accordingly...
while true;do

	# Get position
	pos=`eesh -ewait "win_op $windowid move ? ?"`
	a=0
	for i in $pos;do
		a=$(($a + 1))
		if [ $a = 3 ];then
			xpos=$i
		fi
		if [ $a = 4 ];then
			ypos=$i
		fi
	done
		
	# Move the second window to the new position
	eesh -e "win_op $windowid2 move $xpos $(($ypos + 74))"
done
