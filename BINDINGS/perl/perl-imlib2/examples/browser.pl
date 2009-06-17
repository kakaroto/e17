#!/usr/bin/perl

use Imlib2;
use Gtk;
init Gtk;

$loaded = 0;

gui();
parse_args();
Gtk->main();

sub gui
{

	$win = new Gtk::Window("toplevel");
	$win->set_title("Imlib2::Perl Image browser!");
	$win->set_default_size(500,340);
	$win->set_policy(1,1,0);
	$vbox = new Gtk::VBox(0,0);
	$win->add($vbox);
	$win->signal_connect("destroy", sub { Gtk->exit(0); });
	$hbox = new Gtk::HBox(0,0);
	$hbox->border_width(2);
	$vbox->pack_start($hbox,1,1,0);
	$da = new Gtk::DrawingArea();
	$da->set_events("button_press_mask");
	$hbox->pack_start($da,1,1,0);
	Imlib2_Init();
	$da->signal_connect("expose_event", \&render);
	$da->signal_connect("button_press_event", \&button_press);
	$scrolled_win = new Gtk::ScrolledWindow(undef, undef);
	$scrolled_win->set_policy(-never, -automatic);
	$hbox->pack_start($scrolled_win, 1, 1, 0);

	$clist = new Gtk::CList(1);
	$scrolled_win->add($clist);
	$clist->set_usize(100,100);
	$clist->set_column_auto_resize(0, 1);
	$clist->signal_connect('select_row', \&select_clist);
	$statusbar = new Gtk::Statusbar();
	$vbox->pack_start($statusbar,0,0,0);
	
	$popup = new Gtk::Menu();
	
	$view_full = new Gtk::MenuItem("View Full Size");
	$view_full->signal_connect("activate", \&view_full);
	$popup->append($view_full);
	
	$blur_menu = new Gtk::MenuItem("Blur");
	$blur_menu->signal_connect("activate", \&blur);
	$popup->append($blur_menu);
	
	$sharpen_menu = new Gtk::MenuItem("Sharpen");
	$sharpen_menu->signal_connect("activate", \&sharpen);		
	$popup->append($sharpen_menu);	
	
	$flip_h = new Gtk::MenuItem("Flip horizontal");
	$flip_h->signal_connect("activate", sub { Imlib2::flip_horizontal() });
	$popup->append($flip_h);
	
	$flip_v = new Gtk::MenuItem("Flip vertical");
	$flip_v->signal_connect("activate", sub { Imlib2::flip_vertical() });
	$popup->append($flip_v);
	
	$flip_d = new Gtk::MenuItem("Flip diagonal");
	$flip_d->signal_connect("activate", sub { Imlib2::flip_diagonal() });
	$popup->append($flip_d);
	
	show_all $popup;
	show_all $win;
	
}

sub select_clist {

	($widget,$row,$col,$ev,$data) = @_;
	$text = $widget->get_text($row,$col);
	Imlib2::free_image() if $loaded;	
	
	$im = Imlib2::load_image ($path . "/" . $text);
	if (!$im) {
		$loaded = 0;
		$statusbar->push(1,"Could not load image $text");
		return;
	}	
	Imlib2::set_context($im);	
	$w = Imlib2::get_width();
	$h = Imlib2::get_height();
	
	$statusbar->push(1,"File: $text - Width $w - Height $h");
	
	if($w > $h) {
		$mult = ($w / 300);
		$h = $h/$mult;
		$w = 300;
		$da->size($w,$h);
		$da->window->clear();
		Imlib2::render_image_on_drawable_at_size(0,0,$w,$h);
	} else {
		$mult = ($h / 300);
		$w = $w/$mult; 
		$h = 300;
		$da->size($w,$h);
		$da->window->clear();
		Imlib2::render_image_on_drawable_at_size(0,0,$w,$h);
	}
	
	$loaded = 1 if !$loaded;
}


sub Imlib2_Init
{
	 $da->realize();
         my $cmap = $da->get_colormap()->XCOLORMAP;
	 my $visual = $da->get_visual()->XVISUAL;
	 my $display = $da->window->XDISPLAY;
	 Imlib2::context_set_display($display);
	 Imlib2::context_set_colormap($cmap);
	 Imlib2::context_set_visual($visual);	 
	 Imlib2::context_set_drawable($da->window->XWINDOW);
	 return 1;
}	 
								 
								 
sub render
{	

	Imlib2::render_image_on_drawable_at_size(0,0,$w,$h) if $loaded;
 	return;
}	

sub button_press
{
	($da, $event) = @_;
	return if ($event->{button} != 3);
	return if !$loaded;
	$popup->popup(undef,undef,$event->{button},$event->{time});
	return;
}	

sub parse_args
{
	if (!$ARGV[0]) { 
		$ARGV[0] = "./"; 
		$path = $ARGV[0];
	}
	
	if($ARGV[0] && -d $ARGV[0]) {	
		$path = $ARGV[0];
		foreach(`ls -1 $ARGV[0]`) {
			chomp;
			#check if its a file to begin with and then if its an image.
			if (/(bmp|gif|xcf|jpg|jpeg|png|tif|tiff)$/i) {
				if(-f $path . "/" . $_) {
					$clist->append($_);				
				}
			}	
		}
	} else {
		die "usage: browser.pl directory\n";		
	}
}

sub view_full
{
	system("./pfeh.pl $path/$text &");
	return;
}	

sub blur
{
	$blur = 0 if !$blur;
	Imlib2::blur($blur+10);
	return;
}	
	
sub sharpen
{
	$sharpen = 0 if !$sharpen;
	Imlib2::sharpen($sharpen+10);
	return;
}

