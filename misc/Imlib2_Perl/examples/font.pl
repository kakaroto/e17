#!/usr/bin/perl
use Imlib2;
use Gtk;
use strict;
Gtk->init();


gui();
Gtk->main();

sub gui
{
	my $win;
	$win = new Gtk::Window("toplevel");
	$win->set_title("Imlib2::Perl Font Rendering example");
	$win->set_default_size(600,440);
	$win->signal_connect("destroy", sub { Gtk->exit(0); });
	$win->{'vbox'} = new Gtk::VBox(0,0);
	$win->add($win->{'vbox'});
	$win->{'lbl'} = new Gtk::Label("Enter some text below to render: ");
	$win->{'vbox'}->pack_start($win->{'lbl'},0,0,0);
	$win->{'hbox'} = new Gtk::HBox(0,0);
	$win->{'vbox'}->pack_start($win->{'hbox'},0,0,0);	
	
	$win->{'entry'} = new Gtk::Entry();
	$win->{'render'} = new Gtk::Button("Render");
	$win->{'hbox'}->pack_start($win->{'entry'},1,1,0);
	$win->{'hbox'}->pack_start($win->{'render'},0,0,0);
	$win->{'lbl'} = new Gtk::Label("Text Direction: ");
	$win->{'right'} = new Gtk::RadioButton("Right");
	$win->{'left'} = new Gtk::RadioButton("Left", $win->{'right'});
	$win->{'down'} = new Gtk::RadioButton("Down", $win->{'left'});
	$win->{'up'} = new Gtk::RadioButton("Up", $win->{'down'});
	$win->{'angle'} = new Gtk::RadioButton("Angled", $win->{'up'});
	$win->{'angle_entry'} = new Gtk::Entry();
	$win->{'angle_entry'}->set_text("-1.0");
	
	$win->{'hbox'} = new Gtk::HBox(0,0);
	$win->{'hbox'}->pack_start($win->{'lbl'},0,0,0);
	$win->{'hbox'}->pack_start($win->{'left'},0,0,0);
	$win->{'hbox'}->pack_start($win->{'right'},0,0,0);
	$win->{'hbox'}->pack_start($win->{'up'},0,0,0);
	$win->{'hbox'}->pack_start($win->{'down'},0,0,0);
	$win->{'hbox'}->pack_start($win->{'angle'},0,0,0);
	$win->{'hbox'}->pack_start($win->{'angle_entry'},0,0,0);
	$win->{'vbox'}->pack_start($win->{'hbox'},0,0,0);
	
	$win->{'frame'} = new Gtk::Frame("Rendered Image");
	$win->{'da'} = new Gtk::DrawingArea();	
	$win->{'da'}->signal_connect("expose_event", \&expose_event);
	$win->{'frame'}->add($win->{'da'});
	$win->{'vbox'}->pack_start($win->{'frame'},1,1,0);
	$win->{'entry'}->signal_connect("activate", \&render, $win);
	$win->{'render'}->signal_connect("clicked", \&render, $win);
	$win->{'angle_entry'}->signal_connect("activate", \&render, $win);
	
	Imlib2_Init($win);	
	Imlib2::context_set_blend(1);
	Imlib2::context_set_dither(1);
	my $image = Imlib2::create_image(600,350);
	Imlib2::set_context($image);

	$win->{'entry'}->grab_focus();
	demo();
	
	show_all $win;
	return;
}	

sub fill_image
{
	Imlib2::context_set_color(255,255,255,255);
	Imlib2::draw_rectangle(0,0,600,350);
	Imlib2::fill_rectangle(0,0,600,350);
	return;
}	

sub demo
{
	Imlib2::context_set_angle(-1.0);
	Imlib2::context_set_direction(4);
	Imlib2::text_draw(150,20,"Imlib2::Perl");
	return;
}	

sub render
{
	my ($widget, $win) = @_;
	my $image;
	my $text = $win->{'entry'}->get_text();		
	return if (!$text || $text eq "");
	Imlib2::context_set_direction( get_direction($win) );
	Imlib2::free_image();
	$image = Imlib2::create_image(600,350);
	Imlib2::set_context($image);
	Imlib2::text_draw(50,100,$text);	       
	expose_event();
	return;
}

sub get_direction
{
	my ($win) = @_;
	Imlib2::context_set_angle(0);
	if ($win->{'right'}->active()) {
		return 0;
	} elsif ($win->{'left'}->active()) {
		return 1;
	} elsif ($win->{'down'}->active()) {
		return 2;
	} elsif ($win->{'up'}->active()) {
		return 3;
	} elsif ($win->{'angle'}->active()) {
		my $angle = $win->{'angle_entry'}->get_text();
		if ($angle && $angle ne "") {
			Imlib2::context_set_angle($angle);
			return 4;
		} else {
			return 1;
		}
	}	
	
	return 1;
}	

sub expose_event
{
	Imlib2::render_image_on_drawable(0,0);
	return;
}	


sub Imlib2_Init
{
        my ($win) = @_;
	$win->{'da'}->realize();
	my $cmap = $win->{'da'}->get_colormap()->XCOLORMAP;
	my $visual = $win->{'da'}->get_visual()->XVISUAL;
	my $display = $win->{'da'}->window->XDISPLAY;
	Imlib2::context_set_display($display);
	Imlib2::context_set_colormap($cmap);
	Imlib2::context_set_visual($visual);
	Imlib2::context_set_drawable($win->{'da'}->window->XWINDOW);
	Imlib2::add_path_to_font_path("./fonts");
	my $font = Imlib2::load_font("notepad/50");
	Imlib2::context_set_font($font);
	return 1;
}
