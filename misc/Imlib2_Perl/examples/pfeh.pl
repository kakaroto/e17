#!/usr/bin/perl
use Imlib2;
use strict;
eval 'use Gtk; init Gtk;' or die "Did not find Gtk::Perl\n";

init Gtk::Gdk::Rgb;

Gtk::Widget->set_default_colormap(Gtk::Gdk::Rgb->get_cmap());
Gtk::Widget->set_default_visual(Gtk::Gdk::Rgb->get_visual());

#Imlib2::context_set_display(Gtk::Gdk->get_display);
#Imlib2::context_set_colormap(Gtk::Gdk::Rgb->get_cmap());
#Imlib2::context_set_visual(Gtk::Gdk::Rgb->get_visual());

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
	$win->signal_connect("destroy", sub { Gtk->exit(0) });
	$win->{'da'} = new Gtk::DrawingArea();
	$win->{'da'}->size($w,$h);
	$win->realize();
	$win->add($win->{'da'});

	my $bgcolor = Gtk::Gdk::Color->parse_color('white');
	$bgcolor = $win->{'da'}->window->get_colormap()->color_alloc($bgcolor);
	$win->{'da'}->window->set_background($bgcolor);

	#Imlib2::context_set_drawable($win->{'da'}->window);
	#Imlib2::render_image_on_drawable($w,$h);
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
