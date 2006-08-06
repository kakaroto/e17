use Test::More 'no_plan'; # tests => 1;
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

