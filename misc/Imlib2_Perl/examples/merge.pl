#!/usr/bin/perl
use Imlib2;

parse_args();

$image1 = Imlib2::load_image($ARGV[0]);
$image2 = Imlib2::load_image($ARGV[1]);
Imlib2::set_context($image1);
$w1 = Imlib2::get_width();
$h1 = Imlib2::get_height();
print "Image 1 has $w1 width and $h1 height\n";

Imlib2::set_context($image2);
$w2 = Imlib2::get_width();
$h2 = Imlib2::get_height();
print "Image 1 has $w2 width and $h2 height\n";

$newimage = Imlib2::create_image($w1+$w2,$h1);
Imlib2::set_context($newimage);

Imlib2::blend_image_onto_image($image1, 1, 0, 0, $w1, $h1, 0, 0, $w1, $h1);
Imlib2::blend_image_onto_image($image2, 1, 0, 0, $w2, $h2, $w1, 0, $w2, $h2);
	
Imlib2::save_image($ARGV[2]);


sub parse_args 
{

	if (!@ARGV) {
		usage();
	} elsif (@ARGV && scalar(@ARGV) < 3) {
		usage();
	}
	foreach(0..1) {
		if (!-f $ARGV[$_] || $ARGV[$_] !~ /(bmp|gif|jpg|jpeg|png)$/i) {
			die "$ARGV[$_] is not an image file or does not exist\n";
       		}
	}	
	if ($ARGV[2] !~ /(bmp|gif|jpg|jpeg|png)$/i) {
		die "$ARGV[2] is not a known image type to save to\n";
	}
	return 1;
}

sub usage 
{
	print "usage: merge.pl image1 image2 output_image\n";
	exit(0);
}


