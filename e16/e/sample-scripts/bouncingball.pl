#!/usr/bin/perl

# Copyright (C) 2000-2004 Carsten Haitzler, Geoff Harrison and various contributors
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

# This is a little script that will create a window that says
# "Follow the Bouncing Ball" and then drops it to the bottom of the
# screen and slowly bounces it around.
# then when it's done bouncing it gets rid of it.


$screen_size = `eesh -ewait \"general_info screen_size\"`;

chomp($screen_size);

($crap,$width,$height) = split(/\s+/,$screen_size);


# we'll create the ball here and by process of elimination determine what
# the winid is.
@winlist1 = `eesh -ewait window_list`;

`eesh -e \"dialog_ok Follow the Bouncing Ball\"`;
@winlist2 = `eesh -ewait window_list`;

# run through the two lists and figure out which one is new.

foreach $item1 (@winlist2) {
	$inside = 0;
	foreach $item2 (@winlist1) {
		$inside = 1 if($item1 eq $item2);
	}
	$ballwininfo = $item1 if(!$inside);
}

# call the ball, ace
# (now we have the windowid of our ball)

($ball,$message) = split(/ \: /,$ballwininfo);
$ball =~ s/\s+//g;

$ballloc = `eesh -ewait \"win_op $ball move ?\"`;
$ballsize = `eesh -ewait \"win_op $ball resize ??\"`;

$ballloc =~ s/^.*\: //g;
$ballloc =~ s/\n//g;
$ballsize =~ s/^.*\: //g;
$ballsize =~ s/\n//g;

($ballx,$bally) = split(/\s+/,$ballloc);
($ballw,$ballh) = split(/\s+/,$ballsize);

# now for the fun part.  make that baby bounce up and down.
# we're going to open a big pipe for this one and just shove data
# to it.

open IPCPIPE,"| eesh";

@fallspeed = (30,25,20,15,10,5,4,3,2);
$i = 0;
foreach(@fallspeed) {
	$originalbally = $bally;
	$fallspeed = $fallspeed[i];
	while($bally < ($height - $ballh)) {
		if(($bally + $fallspeed + $ballh) < $height) {
			$bally += $fallspeed;
		} else {
			$bally = $height - $ballh;
		}
		print IPCPIPE "win_op $ball move $ballx $bally\n";
	}

	if($fallspeed[i+1]) {
		$fallspeed = $fallspeed[i+1];
	} else {
		$fallspeed = 1;
	}

	while($bally > ($originalbally + int($originalbally * (1/$#fallspeed)))) {
		if(($bally - $fallspeed) > 
				($originalbally + int($originalbally * (1/$#fallspeed)))) {
			$bally -= $fallspeed;
		} else {
			$bally = $originalbally + int($originalbally * (1/$#fallspeed));
		}
		print IPCPIPE "win_op $ball move $ballx $bally\n";
	}
	$i++;
}

print IPCPIPE "win_op $ball close\n";
close IPCPIPE;

# that's all folks.
