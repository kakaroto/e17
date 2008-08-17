use Test::More tests => 2;
use Etk;

my $t = Etk::TextView->new();
my $b = $t->TextblockGet();

ok( $b->isa("Etk::TextBlock"),	"Class Check");

$b->TextSet("moo", 0);
is($b->TextGet(0), "moo", "Text Set/Get");

# TODO: continue this


