#!/usr/bin/perl

# Copyright (C) 2000-2003 Carsten Haitzler, Geoff Harrison and various contributors
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

# This script is still VERY VERY VERY early on in development
# but I wanted to give you folks an idea of why the IPC was being
# set up the way it was.  this is just a quick hack.  expect better
# samples in the near future.
# This app will take the parameters "on" and "off", and will basically
# shade all the windows on the current desktop and area.  (or unshade)
#
# --Mandrake

@winlist_temp = `eesh -ewait window_list`;
@intlist_temp = `eesh -ewait \"internal_list internal_ewin\"`;

# here we're going to test to see whether we are shading or unshading
# the window.

if($ARGV[0] eq "on") {
	$shade = 1;
} else {
	$shade = 0;
}


# make sure that we're not an internal window in our list

foreach(@winlist_temp) {
	chomp;
	@stuff = split /\:/;
	$insert = 1;
	foreach $member (@intlist_temp) {
		chomp($member);
		$stuff[0] =~ s/\s+//g;
		$member =~ s/\s+//g;
		if($member eq $stuff[0]) {
			$insert = 0;
		}
	}
	if($insert) {
		push @winlist,$stuff[0] if($stuff[0]);
	}
}

# here we'll retreive the current desk we're on

$current_desk = `eesh -ewait \"goto_desktop ?\"`;
@stuff = split(/\:/,$current_desk);
$current_desk = $stuff[1];
$current_desk =~ s/\n//g;
$current_desk =~ s/^\s+//;

# here we'll retreive the current area we're on

$current_area = `eesh -ewait \"goto_area ?\"`;
@stuff = split(/\:/,$current_area);
$current_area = $stuff[1];
$current_area =~ s/\n//g;
$current_area =~ s/^\s+//;


# get the old shadespeed so that we can set it back later
# because we want this to happen fairly quickly, we'll set
# the speed to something really high

$shadespeed = `eesh -ewait \"fx window_shade_speed ?\"`;
@stuff = split(/\: /,$shadespeed);
$shadespeed = $stuff[1];
chomp($shadespeed);

open IPCPIPE,"| eesh";
print IPCPIPE "fx window_shade_speed 10000000\n";

# now we're going to walk through each of these windows and
# shade them

foreach $window (@winlist) {
	$cur_window_desk = `eesh -ewait \"win_op $window desk ?\"`;
	@stuff = split(/\:/,$cur_window_desk);
	$cur_window_desk = $stuff[1];
	$cur_window_desk =~ s/\n//g;
	$cur_window_desk =~ s/^\s+//;
	if($cur_window_desk eq $current_desk) {
		$cur_window_area = `eesh -ewait \"win_op $window area ?\"`;
		@stuff = split(/\:/,$cur_window_area);
		$cur_window_area = $stuff[1];
		$cur_window_area =~ s/\n//g;
		$cur_window_area =~ s/^\s+//;
		if($cur_window_area eq $current_area) {
			if($shade) {
				print IPCPIPE "win_op $window shade on\n";
			} else {
				print IPCPIPE "win_op $window shade off\n";
			}

		}
	}
}

# now we're going to set the shade speed back to what it was originally

print IPCPIPE "fx window_shade_speed $shadespeed\n";
close IPCPIPE;


# that's it!
