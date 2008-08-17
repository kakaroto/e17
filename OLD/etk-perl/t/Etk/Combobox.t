use Test::More 'no_plan'; # tests => 1;
use Etk;

my $b = Etk::Combobox->new();

ok( defined $b, 	"Combobox new()");
ok( $b->isa("Etk::Combobox"),	"Class Check");

$b->ItemsHeightSet(100);
is($b->ItemsHeightGet(), 100, 	"Items Height");

$b->ColumnAdd(0, 1, 1, 1.0);
$b->Build();

my $item = $b->ItemAppend("bleh");

ok( ref $item, 	"Item Append");
ok($item->isa("Etk::Combobox::Item"), 	"Class Check");

$item->DataSet("moo");

is($item->DataGet(), "moo", 	"Item Data set/get");

my $i = $b->ItemAppend("bleh");
$i = $b->ItemAppend("bleh");
$i = $b->ItemAppend("bleh");
$i = $b->ItemAppend("bleh");


$b->ActiveItemSet($item);

$i = $b->ActiveItemGet();
ok( ref $i, 	"ActiveItemGet ");
ok($i->isa("Etk::Combobox::Item"), 	"Class Check");

is($i->DataGet(), "moo",	"Which Item");

print "TODO: Complete this...\n";

