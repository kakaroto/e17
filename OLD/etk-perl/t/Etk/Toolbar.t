use Test::More tests => 5;
use Etk;

my $b = Etk::Toolbar->new();

ok( defined $b, 	"Toolbar new()");
ok( $b->isa("Etk::Toolbar"),	"Class Check");

$b->OrientationSet(1);
is($b->OrientationGet(), 1, "Orientation Set/Get");

$b->StyleSet(3);
is($b->StyleGet(), 3, "Style Set/Get");

$b->StockSizeSet(1);
is($b->StockSizeGet(), 1, "StockSize Set/Get");

