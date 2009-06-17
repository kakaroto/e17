#!/usr/bin/perl

# CVS Commits List Messages
# Copyright (C) 2000 Geoff Harrison and various contributors
#
# For questions, concerns, please consult the e-develop mailing list.
# please see:
# http://lists.sourceforge.net/mailman/listinfo/enlightenment-devel
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies of the Software, its documentation and marketing & publicity
# materials, and acknowledgment shall be given in the documentation, materials
# and software packages that this Software was used.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


use Imlib2;
use Gtk;
use Gtk::Atoms;

init Gtk;


#$pixmap = 0;
$path = "";
$do_not_edit = 0;
$first = 0;
$lastfile = 0;

%descriptions = ();
%orientations = ();
sub set_direction {

	my ($widget,$dir) = @_;

	if(!$do_not_edit) {
		$orientations{$text} = $dir;
	}
}

sub destroy_window {
    my($widget, $windowref, $w2) = @_;
    $$windowref = undef;
    $w2 = undef if defined $w2;
    0;
}

sub exit_application {

	destroy_window(@_);
	Gtk->exit(0);

}

sub file_selection_ok {
	my($widget,$fs) = @_;

	$lastfile = $fs->get_filename;

	destroy $fs_window;

	if($filemode == 1) {
		save_data(0,"$lastfile");
	} elsif ($filemode == 2) {
		load_data($lastfile);
	}

}

sub set_last_file {

# This should open a filename prompt dialog.

	$fs_window = new Gtk::FileSelection "Please Select a File";

	$fs_window->ok_button->signal_connect("clicked", \&file_selection_ok,
			$fs_window);
	$fs_window->cancel_button->signal_connect("clicked", sub { destroy
			$fs_window});
	$fs_window->set_modal(1);
	$fs_window->show;

}

sub load_data {

	my $filename = shift;

	return if(!$filename);
	return if(!(-f $filename));
	%orientations = ();
	%descriptions = ();
	$clist->clear;
	$num_list_items=0;

	my $curfile = "";
	open INFILE, "<$filename";

	while(<INFILE>) {
		if(/^file:/) {
			chomp;
			$curfile = substr $_,6;
			$descriptions{$curfile} = "";
			$num_list_items++;
			$clist->append($curfile);
		} elsif(/^prefix:/) {
			chomp;
			$path = substr($_,8);
		} elsif(/^orient:/) {
			chomp;
			$orientations{$curfile} = substr($_,8);
		} elsif(/^miniwidth:/) {
			chomp;
			$miniwidth = substr($_,10);
		} elsif(/^miniheight:/) {
			chomp;
			$miniheight = substr($_,11);
		} elsif(/^thumbwidth:/) {
			chomp;
			$thumbwidth = substr($_,11);
		} elsif(/^thumbheight:/) {
			chomp;
			$thumbheight = substr($_,12);
		} else {
			chomp;
			$descriptions{$curfile} .= $_;
		}
	}
	close INFILE;

}

sub save_data {

	my $filename = shift;
	my $filename = shift;

	if(!$filename) {
		if(!$lastfile) {
			$filemode = 1;
			set_last_file;
			return;
		}
		$filename = $lastfile;
	}

	open OUTFILE,">$filename";

	print OUTFILE "prefix: $path\n";
	print OUTFILE "miniwidth:$miniwidth\n";
	print OUTFILE "miniheight:$miniheight\n";
	print OUTFILE "thumbwidth:$thumbwidth\n";
	print OUTFILE "thumbheight:$thumbheight\n";
	my $i=0;
	while($i < $num_list_items) {
		my $current = $clist->get_text($i,0);
		print OUTFILE "file: $current\n";
		if($orientations{$current}) {
			print OUTFILE "orient: $orientations{$current}\n";
		} else {
			print OUTFILE "orient: Left->Right\n";
		}
		if($descriptions{$current}) {
			print OUTFILE "$descriptions{$current}\n";
		}

		$i++;
	}

	close OUTFILE;

}

sub load_file {
	$filemode = 2;
	set_last_file;
}

sub save_data_as {
	$file_mode = 1;
	set_last_file;
}

sub select_clist {
	($widget,$row,$col,$ev,$data) = @_;

	$text = $widget->get_text($row,$col);
	Imlib2::free_image();	
	$im = Imlib2::load_image ($path . "/" . $text);
	Imlib2::set_context($im);
	$w = Imlib2::get_width();
	$h = Imlib2::get_height();
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
#	$p->imlib_free;
#	$p = $im->move_image();
#	$m = $im->move_mask();
#	$im->destroy_image;
	$do_not_edit = 1;
	$menu_items{"Left->Right"}->activate;
	$omenu->set_history(0);
	$menu->reposition;
	if($textbox->get_length) {
		$textbox->delete_text(0,-1);
	}
	if($descriptions{$text}) {
		$textbox->insert_text($descriptions{$text},0);
	}
	if($orientations{$text}) {
		$menu_items{"$orientations{$text}"}->activate;
		if($orientations{$text} eq "Top->Bottom") {
			$omenu->set_history(1);
		} elsif ($orientations{$text} eq "Bottom->Top") {
			$omenu->set_history(2);
		} else {
			$omenu->set_history(0);
		}
		$menu->reposition;
	} else {
		$menu_items{"Left->Right"}->activate;
		$omenu->set_history(0);
		$menu->reposition;
	}
	$do_not_edit = 0;
#	$pixmap->set($p,$m);
}

sub update_text {

	my $test1 = shift;

	if(!$do_not_edit) {
		$descriptions{$text} = $test1->get_chars(0,-1);
	}

}

sub update_entry {

	my $thisentry = shift;
	my $var = shift;

	$$var = $thisentry->get_text;

}

sub create_menuitem {
	my $title = shift;
	my $func = shift;
	$menuitem = new Gtk::MenuItem($title);
	$menuitem->show;

	if($func) {
		$menuitem->signal_connect('activate',$func);
	}

	return $menuitem;
}

sub open_prefs_window {

	$prefs_window = new Gtk::Window;
	$prefs_window->set_title("Web Picture Gallery Preferences");

	$vbox = new Gtk::VBox(0,0);
	$vbox->border_width(2);
	$vbox->show;
	$prefs_window->set_modal(1);
	$prefs_window->add($vbox);

	$table = new Gtk::Table(0,0);
	$vbox->pack_start($table,1,1,0);

	$label = new Gtk::Label("Image Source Directory:");
	$label->show;
	$table->attach($label,0,1,0,1,[-fill],[-fill],0,0);

	$entry = new Gtk::Entry;
	if($path) {
		$entry->set_text($path);
	}
	$entry->show;
	$table->attach($entry,1,4,0,1,[-fill,-expand],[-fill],0,0);

	$label = new Gtk::Label("Thumbnail Dimensions:");
	$label->show;
	$table->attach($label,0,1,1,2,[-fill],[-fill],0,0);

	$entry = new Gtk::Entry;
	if($thumbwidth) {
		$entry->set_text($thumbwidth);
	}
	$entry->show;
	$table->attach($entry,1,2,1,2,[-fill,-expand],[-fill],0,0);
	$entry->signal_connect('changed',\&update_entry,\$thumbwidth);

	$label = new Gtk::Label(" x ");
	$label->show;
	$table->attach($label,2,3,1,2,[-fill],[-fill],0,0);

	$entry = new Gtk::Entry;
	if($thumbheight) {
		$entry->set_text($thumbheight);
	}
	$entry->show;
	$table->attach($entry,3,4,1,2,[-fill,-expand],[-fill],0,0);
	$entry->signal_connect('changed',\&update_entry,\$thumbheight);
	
	$label = new Gtk::Label("Halfsize Dimensions:");
	$label->show;
	$table->attach($label,0,1,2,3,[-fill],[-fill],0,0);

	$entry = new Gtk::Entry;
	if($miniwidth) {
		$entry->set_text($miniwidth);
	}
	$entry->show;
	$table->attach($entry,1,2,2,3,[-fill,-expand],[-fill],0,0);
	$entry->signal_connect('changed',\&update_entry,\$miniwidth);

	$label = new Gtk::Label(" x ");
	$label->show;
	$table->attach($label,2,3,2,3,[-fill],[-fill],0,0);

	$entry = new Gtk::Entry;
	if($miniheight) {
		$entry->set_text($miniheight);
	}
	$entry->show;
	$table->attach($entry,3,4,2,3,[-fill,-expand],[-fill],0,0);
	$entry->signal_connect('changed',\&update_entry,\$miniheight);
	
	$table->show;

	$prefs_window->show;

}

$p_window = new Gtk::Window;
signal_connect $p_window "destroy" => \&exit_application, \$p_window;
signal_connect $p_window "delete_event" => \&exit_application, \$p_window;

$p_window->set_title("Web Picture Gallery");

$vbox = new Gtk::VBox(0,0);
$vbox->border_width(2);
$p_window->add($vbox);
$vbox->show;

$menubar = new Gtk::MenuBar;
$vbox->pack_start($menubar,0,1,0);

$menu = new Gtk::Menu;
$menu->append(create_menuitem("New"));
$menu->append(create_menuitem("Open", \&load_file));
$menu->append(create_menuitem("Save",\&save_data));
$menu->append(create_menuitem("Save As"));
$menu->append(create_menuitem("Exit",\&exit_application));
$menu->show;

$menuitem = new Gtk::MenuItem("File");
$menubar->append($menuitem);
$menuitem->set_submenu($menu);
$menuitem->show;

$menu = new Gtk::Menu;
$menu->append(create_menuitem("Next Image"));
$menu->append(create_menuitem("Previous Image"));
$menu->append(create_menuitem("Export Web Site"));
$menu->append(create_menuitem("Edit Preferences",\&open_prefs_window));

$menuitem = new Gtk::MenuItem("Gallery Options");
$menubar->append($menuitem);
$menuitem->set_submenu($menu);
$menuitem->show;

$menu = new Gtk::Menu;
$menu->append(create_menuitem("Documentation"));
$menu->append(create_menuitem("About Web Picture Gallery"));

$menuitem = new Gtk::MenuItem("Help");
$menubar->append($menuitem);
$menuitem->set_submenu($menu);
$menuitem->show;

$menubar->show;


$hbox = new Gtk::HBox(0,0);
$hbox->border_width(2);
$vbox->pack_start($hbox,1,1,0);
$hbox->show;
$da = new Gtk::DrawingArea();
$hbox->pack_start($da,0,0,0);
$da->show();
Imlib2_Init();
$im = Imlib2::load_image ("webpictures.logo.jpg");
Imlib2::set_context($im);
$w = Imlib2::get_width();
$h = Imlib2::get_height();
$da->size($w,$h);
$da->signal_connect("expose_event", \&render);
#Imlib2::render_on_drawable(0,0);
#$p = $im->move_image();
#$m = $im->move_mask;
#$im->destroy_image;

#$pixmap = new Gtk::Pixmap($p,$m);
#$pixmap->show;



$vbox2 = new Gtk::VBox(0,0);
$vbox2->border_width(2);
$hbox->pack_start($vbox2,1,1,0);
$vbox2->show;

$table = new Gtk::Table(0,0);

$label = new Gtk::Label("Orientation: ");
$label->show;
$table->attach($label,0,1,0,1, [-fill],[-fill],0,0);

$omenu = new Gtk::OptionMenu;
$menu = new Gtk::Menu;
$prev = undef;

foreach("Left->Right","Top->Bottom","Bottom->Top") {
	$menu_items{$_} = new_with_label Gtk::MenuItem($_);
	$menu_items{$_}->signal_connect('activate',\&set_direction,$_);
	$menu->append($menu_items{$_});
	$menu_items{$_}->show;
}
$omenu->set_menu($menu);
$omenu->show;

$table->attach($omenu,1,2,0,1, [-fill],[-fill],0,0);

$scrolled_win = new Gtk::ScrolledWindow(undef, undef);
$scrolled_win->set_policy(-never, -automatic);
$scrolled_win->show;

$table->attach($scrolled_win,0,2,1,2, [-expand, -fill],[-expand, -fill],0,0);

$textbox = new Gtk::Text;
$textbox->show;
$textbox->set_editable(1);
$textbox->set_word_wrap(1);
$textbox->signal_connect('changed',\&update_text);
$scrolled_win->add($textbox);

$table->show;
$vbox2->pack_start($table,1,1,0);

$scrolled_win = new Gtk::ScrolledWindow(undef, undef);
$scrolled_win->set_policy(-never, -automatic);
$hbox->pack_start($scrolled_win, 0, 0, 0);
$scrolled_win->show;

$clist = new Gtk::CList(1);
$scrolled_win->add($clist);
$clist->set_column_auto_resize(0, 1);
$clist->signal_connect('select_row', \&select_clist);


$num_list_items = 0;

if($ARGV[0]) {
	if(-d $ARGV[0]) {
		$path = $ARGV[0];
		foreach(`ls -1 $ARGV[0]`) {
			chomp;
#check if its a file to begin with and then if its an image.
			if (/(bmp|gif|jpg|jpeg|png|tif|tiff)$/i) {
				if(-f $path . "/" . $_) {
					$clist->append($_);
					$num_list_items++;
				}
			}	
		}
	} elsif(-f $ARGV[0]) {
		load_data($ARGV[0]);
		$lastfile = $ARGV[0];
	}
}

$clist->show;

$p_window->show;
main Gtk;

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
	if (!$first) {
 		Imlib2::render_image_on_drawable(0,0);
		$first = 1;
		return;
	}	
	Imlib2::render_image_on_drawable_at_size(0,0,$w,$h);
 	return;
}	
