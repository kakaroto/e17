#!/usr/bin/perl

use Gtk::Gdk::ImlibImage;
use Gtk;
use Gtk::Atoms;

init Gtk;
init Gtk::Gdk::ImlibImage;

$pixmap = 0;
$path = "";
$do_not_edit = 0;


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

sub set_last_file {

# This should open a filename prompt dialog.

}

sub save_data {

	my $filename = shift;

	$filename = "bubba";
	if(!$filename) {
		set_last_file if(!$lastfile);
		$filename = $lastfile;
	}

	open OUTFILE,">$filename";

	my $i=0;
	while($i < $num_list_items) {
		my $current = $clist->get_text($i,0);
		print OUTFILE "file: $path/$current\n";
		if($orientations{$current}) {
			print OUTFILE "orient: $orientations{$current}\n";
		} else {
			print OUTFILE "orient: Left->Right\n";
		}
		if($descriptions{$current}) {
			print OUTFILE "desc:\n$descriptions{$current}\n";
		} else {
			print OUTFILE "desc:\n\n";
		}

		$i++;
	}

	close OUTFILE;

}

sub save_data_as {

	set_last_file;
	save_data;

}

sub select_clist {
	($widget,$row,$col,$ev,$data) = @_;

	$text = $widget->get_text($row,$col);
#print "$text\n";
	
	$im = load_image Gtk::Gdk::ImlibImage($path . "/" . $text);
	$im->render(300,300);
	$p->imlib_free;
	$p = $im->move_image();
	$m = $im->move_mask();
	$im->destroy_image;
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
	$pixmap->set($p,$m);
}

sub update_text {

	my $test1 = shift;

	if(!$do_not_edit) {
		$descriptions{$text} = $test1->get_chars(0,-1);
	}

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
$menu->append(create_menuitem("Open"));
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
$menu->append(create_menuitem("Edit Preferences"));

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

$im = load_image Gtk::Gdk::ImlibImage("webpictures.logo.jpg");
$im->render(300,300);
$p = $im->move_image();
$m = $im->move_mask;
$im->destroy_image;

$pixmap = new Gtk::Pixmap($p,$m);
$pixmap->show;

$hbox->pack_start($pixmap,0,0,0);

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
	$path = $ARGV[0];
	foreach(`ls -1 $ARGV[0]`) {
		chomp;
#check if its a file to begin with and then if its an image.
		if (/(bmp|gif|jpg|jpeg|png)$/i) {
			if(-f $path . "/" . $_) {
				$clist->append($_);
				$num_list_items++;
			}
		}	
	}
}

$clist->show;

$p_window->show;
main Gtk;
