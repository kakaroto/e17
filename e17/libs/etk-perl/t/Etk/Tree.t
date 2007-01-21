use Test::More 'no_plan'; # tests => 1;
use Etk;

my $t = Etk::Tree->new();

ok( defined $t, 	"Tree new()");
ok( $t->isa("Etk::Tree"),	"Class Check");

my $col = $t->ColNew("moo", 20, 0.5);
ok( $col->isa("Etk::Tree::Col"),	"Col Class Check");

is($t->NumColsGet(), 1, "NumColsGet()");

$t->HeadersVisibleSet(1);
is($t->HeadersVisibleGet(), 1, "HeadersVisible Set/Get");

is($col->TitleGet(), "moo", "Col TitleGet");
$col->TitleSet("test");
is($col->TitleGet(), "test", "Col TitleSet");
SKIP: {
	skip "XXX todo", 3;
is($col->WidthGet(), 20, "Col WidthGet");
$col->WidthSet(24);
is($col->WidthGet(), 24, "Col WidthSet");

$col->MinWidthSet(4);
is($col->MinWidthGet(), 4, "Col MinWidth Set/Get");
}

$col->ResizableSet(1);
is($col->ResizableGet(), 1, "Col Resizable Set/Get");

$col->ExpandSet(1);
is($col->ExpandGet(), 1, "Col Expand Set/Get");

$col->VisibleSet(0);
is($col->VisibleGet(), 0, "Col Visible Set/Get");

# TODO: continue

