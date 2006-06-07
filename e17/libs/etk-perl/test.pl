use strict;
use Etk;
use Etk::Window;
use Etk::Button;
use Etk::VBox;
use Etk::Tree;
use Etk::Tree::Model::Text;
use Etk::Tree::Model::ProgressBar;
use Etk::Tree::Col;

Etk::etk_init();

my $win = Etk::Window->new();
my $button1 = Etk::Button->new("click me!");
my $button2 = Etk::Button->new("click me too!");
my $vbox = Etk::VBox->new(0, 0);
my $tree = Etk::Tree->new();
my $col1 = Etk::Tree::Col->new($tree, "Col 1",
    Etk::Tree::Model::Text->new($tree), 90);
my $col2 = Etk::Tree::Col->new($tree, "Col 2",
    Etk::Tree::Model::ProgressBar->new($tree), "90");

$tree->SizeRequestSet(320, 400);
$tree->Build();

my $row1 = $tree->Append();
$row1->FieldTextSet($col1, "Weee!");
$row1->FieldProgressBarSet($col2, 0.5, " Loading ... ");

my $row2 = $tree->Append();
$row2->FieldTextSet($col1, "Second line");
$row2->FieldProgressBarSet($col2, 0.2, " Reading ... ");

$vbox->PackStart($button1, 1, 1, 5);
$vbox->PackStart($button2, 0, 0, 2);
$vbox->PackStart($tree, 0, 0, 0);

$win->Add($vbox);
$win->ShowAll();

$button1->SignalConnect("clicked", \&click_cb1);
$button2->SignalConnect("clicked", \&click_cb2);
$win->SignalConnect("delete_event", \&quit_cb);

Etk::etk_main();
Etk::etk_shutdown();

sub click_cb1
{
    print "click_cb1!\n";
    my ($padding, $expand, $fill, $pack_end) = $vbox->ChildPackingGet($button1);
    print "padding = $padding, expand = $expand, fill = $fill, pack_end = $pack_end\n";
}

sub click_cb2
{
    print "click_cb2!\n";
}

sub quit_cb
{
    print "quit!\n";
    Etk::etk_main_quit();
}
