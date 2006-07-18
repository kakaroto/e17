use strict;
use Etk;

my $win = Etk::Window->new();

my $vbox = $win->AddVBox(0, 0);
$vbox->AddButton("click me!")->SignalConnect("clicked", 
	\&click_cb1, "click_cb1_data")->PackStart(1, 1, 5);
$vbox->AddButton("click me too!")->SignalConnect("clicked", 
	\&click_cb2)->PackStart(0, 0, 2);
my $tree = $vbox->AddTree();

$tree->AddCols(
	["Col 1", "Text", 90],
	["Col 2", "ProgressBar", 90]
);

$tree->SizeRequestSet(320, 400);
$tree->Build();

$tree->AddRows(
	[ 
	  "Weee!!!", [ 0.5, " Loading ... "]
	],
	[
	  "Second Line", [ 0.2, " Reading ... "]
	]
);

$tree->PackStart(0, 0, 0);
$win->ShowAll();
$win->SignalConnect("delete_event", \&quit_cb);

Etk::Main::Run();
Etk::Shutdown();

sub click_cb1
{
    my $self = shift;
    my $data = shift;
    print "click_cb1! (data=$data)\n";
    my ($padding, $expand, $fill, $pack_end) = $vbox->ChildPackingGet($self);
    print "padding = $padding, expand = $expand, fill = $fill, pack_end = $pack_end\n";
}

sub click_cb2
{
    print "click_cb2!\n";
}

sub quit_cb
{
    print "quit!\n";
    Etk::Main::Quit();
}
