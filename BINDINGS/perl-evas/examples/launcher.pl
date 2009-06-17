#!/usr/bin/perl
use Gtk;
use Imlib2;
use Evas;
use strict;
init Gtk;

my $boxtype = 0; #vertical or horizontal - switchable from cmdline also
my $size = 48;   #button size
my $fontsize = 9; #font size
my $font = "notepad"; #ttf font
my $text_enabled = 1; #enable text description or not
my $images_enabled = 1; #enable images
my $rectangle_r = 255; #rgba for rectangle
my $rectangle_g = 255;
my $rectangle_b = 255;
my $rectangle_a = 255;
my $text_r = 0;	#rgba of text
my $text_g = 0;
my $text_b = 0;
my $text_a = 200;

parse_args();
gui();
Gtk->main();

sub gui
{
	my ($win,$box,$tooltips);
	$win = new Gtk::Window("toplevel");
	$win->set_policy(1,1,1);
	$win->set_uposition(0,0);
	$win->realize();
	$win->window->set_decorations(0);
	$tooltips = new Gtk::Tooltips();
	$tooltips->enable();
	$tooltips->set_delay(500);
	
	Imlib_Init($win);
	
	if ($boxtype == 0) {
		$box = new Gtk::HBox(0,0);
	} else {
		$box = new Gtk::VBox(0,0);
	}	
	
	$win->add($box);		            	
	show_all $win;
	read_buttons($tooltips,$box);
}	

sub no_rc
{	
	print("
	I could not find an rc, create one in $ENV{'HOME'}/.evas_launcher
	with the following format,
	command,tooltip,icon,text
	for e.g,
	pronto,Pronto Email App,/usr/share/pixmaps/pronto.png,Pronto.\n
	");
	exit(0);

}	

sub read_buttons
{
	my ($tooltips,$box) = @_;
	open(TMP,"$ENV{'HOME'}/.evas_launcher") or no_rc();
	while (my $line = <TMP>) {
		my @tmp = split(/,/,$line); #command,tooltip,icon
		my ($button,$da,$bgcolor);
		$button = new Gtk::Button();
		$da = new Gtk::DrawingArea();
		$button->add($da);		
		$box->pack_start($button,0,0,0);
		$button->signal_connect("clicked", \&launch, $tmp[0]);
		$tooltips->set_tip($button,$tmp[1], "");
		$da->realize();
		$da->signal_connect("expose_event", \&expose);
		$bgcolor = Gtk::Gdk::Color->parse_color('grey');
		$bgcolor = $da->window->get_colormap()->color_alloc($bgcolor);
		$da->window->set_background($bgcolor);
		show_all $button;
		render_image($da,$tmp[2],$tmp[3]);
	}
	close(TMP);
	return;
}	

sub render_image
{
	my ($da,$path,$text) = @_;
	my ($image, $w, $h);
	chomp($text);
	$image = make_evas($path,$text);	
	$da->size($size,$size);
	$da->{'image'}=$image;
	return;
}	

sub expose
{
	my ($da) = @_;
	Imlib2::set_context($da->{'image'});
	Imlib2::context_set_drawable($da->window->XWINDOW());
	$da->window->clear();
	Imlib2::render_image_on_drawable_at_size(0,0,$size,$size);
	return;
}	

sub launch
{
	my ($button, $cmd) = @_;
#	system($cmd . "&") if $cmd && $cmd ne "";
#	return if !$cmd || $cmd eq "";
	open(TMP, "$cmd 2>&1 &|");
	close(TMP);
	return;
}	

sub Imlib_Init
{
	my ($win) = @_;
	my $cmap = $win->get_colormap()->XCOLORMAP;
	my $visual = $win->get_visual()->XVISUAL;
	my $display = $win->window->XDISPLAY;
	Imlib2::context_set_display($display);
	Imlib2::context_set_colormap($cmap);
	Imlib2::context_set_visual($visual);
	return;
}

sub parse_args
{
	return if (!$ARGV[0]);
	if ($ARGV[0] eq "v") {
		$boxtype = 1;
	}	
	else {
		die "usage: launch h for horizontal or v vertical\n";
	}
}	



sub make_image
{
	my $image = Imlib2::create_image(64,64); #create our image
	Imlib2::set_context($image);
	Imlib2::set_format("png");
	return $image;
}

sub make_evas
{
	my ($path,$text) = @_;
	my ($o,$o2);
	my $evas = Evas::new(); 
	Evas::set_output_method($evas,4); #set to image render method
	my $image = make_image();
	Evas::set_output_image($evas,$image); #output to the image we created
	Evas::set_output_size($evas,64,64); #set evas size
	Evas::set_output_viewport($evas,0.0,0.0,64.0,64.0); #evas viewport
	Evas::font_add_path($evas,"./fonts"); #lookup fonts in this dir
	my $o3 = Evas::add_rectangle($evas); # add a rectangle
	Evas::set_color($evas,$o3,$rectangle_r,$rectangle_g,$rectangle_b,$rectangle_a); #fill it with white
	Evas::set_layer($evas,$o3,0); #set as lowest layer
	Evas::show($evas,$o3); #show it
	Evas::move($evas, $o3, 0.0, 0.0); #position it
	Evas::resize($evas, $o3, 64.0 , 64.0); #resize it
	
	if ($images_enabled) {	
      		$o = Evas::add_image_from_file($evas,$path); #add an image_layer
		Evas::set_layer($evas,$o,100); #layer it atop the rectangle
		Evas::resize($evas, $o, 48.0,48.0);
	}
	
	if ($text_enabled && $images_enabled) {
		Evas::move($evas,$o,8.0,0.0);
	}
	else {		
		Evas::move($evas,$o,8.0,8.0) if $images_enabled;
	}
	
	Evas::show($evas,$o); #show it
	
	if ($text_enabled) {
		$o2 = Evas::add_text($evas,$font,$fontsize,$text); #add a text_layer
		Evas::set_layer($evas,$o2,200); #set its layer 
		Evas::set_color($evas,$o2,$text_r,$text_g,$text_b,$text_a); #color it
		Evas::move($evas,$o2,0.0,48.0) if $images_enabled; #position it		
		Evas::show($evas,$o2); #show it
       	}

	Evas::update_rect($evas,0,0,64,64); #and tell evas to update that area
	Evas::render($evas); #render the evas
	Evas::free($evas);
	return $image;
}

