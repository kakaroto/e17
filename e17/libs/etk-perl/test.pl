use strict;
use Etk;
use Etk::Constants qw/:box/;

my $win = Etk::Window->new();

my $vbox = Etk::VBox->new(0, 0);

my $button = Etk::Button::new_with_label("click me!");
$button->signal_connect("clicked", \&click_cb1, "click_cb1_data");
$vbox->Append($button, BoxStart, BoxNone, 1);

my $button2 = Etk::Button::new_with_label("click me too!");
$button2->signal_connect("clicked", \&click_cb2);
$vbox->Append($button2, BoxStart, BoxNone, 0);


my $tree = Etk::Tree->new();

my $col1 = $tree->ColNew("Col 1", Etk::Tree::Model::Text->new($tree), 90);
my $col2 = $tree->ColNew("Col 2", Etk::Tree::Model::ProgressBar->new($tree), 90);

$tree->SizeRequestSet(320, 400);
$tree->Build();

my $row = $tree->Append();
$row->FieldTextSet($col1, "Wee!!!!");
$row->FieldProgressBarSet($col2, 0.5, " Loading ... ");

$row = $tree->Append();
$row->field_text_set($col1, "Second Line");
$row->FieldProgressBarSet($col2, 0.2, " Reading ... ");

$vbox->Append($tree, BoxStart, BoxNone, 0);

sub click_cb1
{
    my $self = shift;
    my $data = shift;
    print "click_cb1! (data=$data)\n";
    my ($fill, $padding) = $vbox->ChildPackingGet($self);
    print "padding = $padding, fill = $fill\n";
}

sub click_cb2
{
    print "click_cb2!\n";
}

$win->Add($vbox);
$win->ShowAll();

$win->SignalConnect("delete_event", \&quit_cb);

Etk::Main::run();

sub quit_cb
{
    print "quit!\n";
    Etk::Main::quit();
}

