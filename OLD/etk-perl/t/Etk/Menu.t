use Test::More  tests => 9;
use Etk;

my $hb = Etk::Menu->new();
ok( defined $hb, 	"Menu new()");
ok( $hb->isa("Etk::Menu"),	"Class Check");

my $b = Etk::Menu::Bar->new();
ok( defined $b, 	"Menu::Bar new()");
ok( $b->isa("Etk::Menu::Bar"),	"Class Check");

print "==== Menu::Item ====\n";

my $it = Etk::Menu::Item->new();
ok( defined $it, 	"Menu::Item new()");
ok( $it->isa("Etk::Menu::Item"),	"Class Check");

$it->LabelSet("test");
is($it->LabelGet(), "test",	"Label set/get");

my $it2 = Etk::Menu::Item->new();
$b->Append($it);
$b->Append($it2);

my @items = @{$b->ItemsGet()};

is(@items, 2, "Items Get");

is($items[1]->{ETK}, $it2->{ETK}, "Items ok");


