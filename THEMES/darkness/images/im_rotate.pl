#! /usr/local/bin/perl -w

use strict;

foreach my $image (qw (hour minutes seconds)) {
	foreach my $sec (1 .. 59) {
		my $angle =  6 * $sec;
		my $filename = sprintf "clock_${image}_%02d.png", $sec;
		`convert clock_${image}_00.png -background none -matte -rotate $angle -gravity center -crop 135x135+0+0 -quality 100 $filename`;
	}
}
