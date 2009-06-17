#!/usr/bin/perl
use Imlib2::Object;
use strict;

my $image = Imlib2::Object->new(640,480);
$image->set_colour(255,255,255,255);
$image->fill_rectangle(0,0,640,480);
$image->save("output.png");
