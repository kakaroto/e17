#!/usr/bin/perl -w
use Gtk;
use Imlib2;



if (!$ARGV[0]) { die "usage gdkimlib.pl image\n"; }

Gtk->init(); 
load_image();
Gtk->main();


sub load_image
{

	$im = Imlib2::load_image($ARGV[0]);
	die "Null image\n" if !$im;
	Imlib2::set_context($im);
	$w = Imlib2::get_width();
	$h = Imlib2::get_height();
	$win = new Gtk::Gdk::Window( {
		'window_type' => 'toplevel',
		'width' => $w,
		'height' => $h,
		'event_mask' => ['structure_mask']
	});

	$win->set_title("Viewing " . $ARGV[0]);
		
	imlib_init();

#	if (Imlib2::image_has_alpha()) {
#		checks();
#		render_checks();
#	}	

	$m = 0;

	($p,$m) = Imlib2::render_pixmaps_for_whole_image();
	
	$pixmp = foreign_new Gtk::Gdk::Pixmap($p);	
	$m = foreign_new Gtk::Gdk::Pixmap($m) if $m;
	$win->set_back_pixmap($pixmp, 0);
	$win->shape_combine_mask($m,0,0) if $m;
	$win->clear();
	Imlib2::free_image();
	Imlib2::free_pixmap_and_mask($p);
	$win->show;
	return;
}	
	
sub imlib_init
{
	my $cmap = $win->get_colormap()->XCOLORMAP;
	my $visual = $win->get_visual()->XVISUAL;
	my $display = $win->XDISPLAY;
	Imlib2::context_set_display($display);
	Imlib2::context_set_colormap($cmap);
	Imlib2::context_set_visual($visual);
	Imlib2::context_set_drawable($win->XWINDOW);
	return;      
}
				   

sub checks
{

	$checks = Imlib2::create_image(16,16);
	Imlib2::set_context($checks);
	for ($y = 0; $y < 16; $y +=8) {
        	$onoff = ($y/8) & 0x1;
	        for ($x=0;$x<16;$x +=8) {
                	if ($onoff) {
				Imlib2::context_set_color(144,144,144,255);
				Imlib2::fill_rectangle($x,$y,8,8);
		       	} else {
				Imlib2::context_set_color(100,100,100,255);
				Imlib2::fill_rectangle($x,$y,8,8);
		       	}
			$onoff++;
			$onoff = 0 if ($onoff == 2);
	       	}
       	}
	return;
}	

sub render_checks
{

	for ($y = 0; $y < $h; $y +=16) {
		for ($x=0; $x<$w; $x +=16) {
			Imlib2::blend_image_onto_image($checks, 0, 0, 0, 16, 16, $x, $y, 16, 16);
		}
	}
	return;
}	
