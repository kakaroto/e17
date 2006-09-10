use Test::More tests => 4;
use Etk;

my $b = Etk::Table->new(2, 2, 1);

ok( defined $b, 	"Table new()");
ok( $b->isa("Etk::Table"),	"Class Check");

is($b->HomogeneousGet(), 1, "Homogeneous Get");
$b->HomogeneousSet(0);
is($b->HomogeneousGet(), 0, "Homogeneous Set");


