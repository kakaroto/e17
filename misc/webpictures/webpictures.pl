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
		print "changing to $dir\n";
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

sub select_clist {
	($widget,$row,$col,$ev,$data) = @_;

	$text = $widget->get_text($row,$col);
	#check if its a file to beign with and then if its an image.
	if (!-f $text || $text !~ /(bmp|gif|jpg|jpeg|png)$/i) {
		return;
	}	
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
		$menu->reposition;
	} else {
		$menu_items{"Left->Right"}->activate;
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

$p_window = new Gtk::Window;
signal_connect $p_window "destroy" => \&exit_application, \$p_window;
signal_connect $p_window "delete_event" => \&exit_application, \$p_window;

$p_window->set_title("Web Picture Gallery");

$vbox = new Gtk::VBox(0,0);
$vbox->border_width(2);
$p_window->add($vbox);
$vbox->show;

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


if($ARGV[0]) {
	$path = $ARGV[0];
	foreach(`ls -1 $ARGV[0]`) {
		chomp;
		$clist->append($_);
	}
}

$clist->show;

$p_window->show;
main Gtk;
