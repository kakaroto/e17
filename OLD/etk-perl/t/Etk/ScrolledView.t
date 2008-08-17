use Test::More tests => 3;
use Etk;

my $b = Etk::ScrolledView->new();

ok( defined $b, 	"ScrolledView new()");
ok( $b->isa("Etk::ScrolledView"),	"Class Check");

$b->PolicySet(1, 2);
my ($h,$v) = $b->PolicyGet();

ok($h == 1 && $v == 2, "Policy Set/Get");

