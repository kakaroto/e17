use Test::More tests => 2;
use Etk;

my $b = Etk::TextView->new();

ok( defined $b, 	"TextView new()");
ok( $b->isa("Etk::TextView"),	"Class Check");



