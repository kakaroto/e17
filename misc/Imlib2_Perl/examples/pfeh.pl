use Imlib2;
use strict;
eval 'use Gtk; init Gtk;' or die "Did not find Gtk::Perl\n";

parse_args();
load_image();
Gtk->main();

sub load_image
{
	my $image = Imlib2::load_image($ARGV[0]);
	Imlib2::set_context($image);
	my $w = Imlib2::get_width();
	my $h = Imlib2::get_height();
	show_image($image,$w,$h);
	return 1;
}	

sub show_image
{
	my ($image,$w,$h) = @_;
	my $win;
	$win = new Gtk::Window("toplevel");
	$win->set_title("Viewing " . $ARGV[0]);
	$win->set_default_size($w,$h);
	$win->{'da'} = new Gtk::DrawingArea();
	$win->add($win->{'da'});
	show_all $win;
	Imlib2::free_image();
	return 1;
}	


sub parse_args
{
	if (!$ARGV[0]) {
		die "usage: pfeh.pl image\n";
	} else {
		if ($ARGV[0] !~ /(bmp|gif|jpg|jpeg|bmp|png)$/i) {
			die "$ARGV[0] is not an image\n";
		}
	}
	
	return 1;
}	
