#!/bin/sh

# Use: none!
# instructions: move the lucy in the sky with diamonds window and see what
# happends to the other window.

# The very wierd and bad way of getting the windowids of the two message
# windows in bash, there must be a better way!

a=0
eesh -e "dialog_ok Lucy in the sky with diamonds"
window=`eesh -ewait window_list|grep Message`
for i in $window;do
	a=$(($a + 1))
	if [ $a = 1 ];then
		windowid=$i
	fi
done
a=0
eesh -e "dialog_ok Move me with LSD"
window2=`eesh -ewait window_list|grep Message|grep -v $windowid`
for i in $window2;do
    a=$(($a + 1))
	if [ $a = 1 ];then
      windowid2=$i
	fi
done

# In one endless loop, get window positions and see if the lcd window is moving
# if so we move the other window too
while true;do
	lcdpos=`eesh -ewait "win_op $windowid move ? ?"`
	a=0
	for i in $lcdpos;do
		a=$(($a + 1))
		if [ $a = 3 ];then
			lcdxpos=$i
		fi
		if [ $a = 4 ];then
			lcdypos=$i
		fi
	done
	
	pupos=`eesh -ewait "win_op $windowid move ? ?"`
    a=0
	for i in $pupos;do
		a=$(($a + 1))
        if [ $a = 3 ];then
	      puxpos=$i
        fi
		if [ $a = 4 ];then
		  puypos=$i
		fi
	done
	
	if [ $puxpos = $(($lcdxpos)) ];then
		newxpos=$(($puxpos + 58))
		newypos=$(($puypos + 74))
	   eesh -e "win_op $windowid2 move $newxpos $newypos"
	fi

	# Is it faster if we give it a little time delay?
	# I think it is, atleast we don't stress E too much if we give it a delay
	for y in 1 2 3;do
	     bill=0
	done
done
