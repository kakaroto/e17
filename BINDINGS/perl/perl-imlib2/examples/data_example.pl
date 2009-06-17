#!/usr/bin/perl -w
use Imlib2;

$data =  undef;
die "Pass a file to load data from and name to save as\n" if (!$ARGV[0] && !$ARGV[1]);
$im = Imlib2::load_image($ARGV[0]);
Imlib2::set_context($im);
$data = Imlib2::image_get_data();
$im2 = Imlib2::create_image_using_data(300,300,$data);
Imlib2::set_context($im2);
Imlib2::save_image($ARGV[1]);
