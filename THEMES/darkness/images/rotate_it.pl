#! /usr/local/bin/perl -w

use strict;

my $path = "/home/saturn_vk/Projects/themes/darkness/images/";
foreach my $image (qw (clock_hour clock_minutes clock_seconds)) {
	foreach my $sec (1 .. 59) {
		my $angle =  6 * $sec;
		my $filename = sprintf "${image}_%02d.png", $sec;
		print qq{./rotate_image "${image}_00.png" $angle -o $filename\n};
		`./rotate_image "${path}${image}_00.png" $angle -o ${path}$filename`
	}
}
