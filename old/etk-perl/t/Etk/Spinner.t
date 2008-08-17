use Test::More tests => 2;
use Etk;

my $b = Etk::Spinner->new(0.0, 1.0, 0.3, 0.1, 0.5);

ok( defined $b, 	"Spinner new()");
ok( $b->isa("Etk::Spinner"),	"Class Check");

