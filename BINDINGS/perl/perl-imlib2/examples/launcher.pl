#!/usr/bin/perl
use Gtk;
use Imlib2;
use strict;
init Gtk;



my $boxtype = 0;
my %images = {};
my $background = 0;
my $background_color = 'black';
my $default_width = 24;
my $default_height = 24;

parse_args();
gui();
Gtk->main();

sub gui
{
	my ($win,$box,$tooltips);	
	$win = new Gtk::Window("toplevel");
	$win->signal_connect("destroy", sub { Gtk->exit(0); });
	$win->signal_connect("key_press_event" => \&key_press);
	$win->set_policy(9,0,1);
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


sub key_press
{
	my ($win,$event) = @_;
	Gtk->exit(0) if $event->{keyval} == 65307;
	respawn($win) if $event->{keyval} == 114;

}	

sub respawn
{
	my ($win) = @_;
	foreach(keys(%images)) {
		my $image = $images{$_};
		next if !$image;
		Imlib2::set_context($image);
		Imlib2::free_image();
	}	
	$win->signal_handlers_destroy();
	$win->destroy();
	gui();
}	


sub no_rc
{	
	print("
	I could not find an rc, create one in $ENV{'HOME'}/.gtk_launcher
	with the following format,
	command,tooltip text,icon
	for e.g,
	pronto,Pronto Email App,/usr/share/pixmaps/pronto.png\n
	");
	exit(0);

}	


sub read_buttons
{
	my ($tooltips,$box) = @_;
	open(TMP,"$ENV{'HOME'}/.gtk_launcher") or no_rc;
	while (my $line = <TMP>) {
		next if $line =~ /^\#/;
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
		if ($background) {
			$bgcolor = Gtk::Gdk::Color->parse_color($background_color);
			$bgcolor = $da->window->get_colormap()->color_alloc($bgcolor);
			$da->window->set_background($bgcolor);		
		}
		show_all $button;
		render_image($da,$tmp[2]);
	}
	close(TMP);
	return;
}	

sub render_image
{
	my ($da,$path) = @_;
	my ($image, $w, $h);
	chomp($path);
	$image = Imlib2::load_image($path);
	Imlib2::set_context($image);
	Imlib2::context_set_drawable($da->window->XWINDOW());
	$da->size($default_width,$default_height);
	$images{$da} = $image;
	return;
}	

sub expose
{
	my ($da) = @_;
	Imlib2::set_context($images{$da});
	Imlib2::context_set_drawable($da->window->XWINDOW());
	$da->window->clear();
	Imlib2::render_image_on_drawable_at_size(0,0,$default_width,$default_height);
	return;
}	

sub launch
{
	my ($button, $cmd) = @_;
	return if (!$cmd || $cmd eq "");
	system($cmd."&");
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
