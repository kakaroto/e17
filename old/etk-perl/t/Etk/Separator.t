use Test::More tests => 2;
use Etk;

my $b = Etk::HSeparator->new();

ok( defined $b, 	"HSeparator new()");
ok( $b->isa("Etk::HSeparator"),	"Class Check");

