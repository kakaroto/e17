use Test::More tests => 2;
use Etk;

my $b = Etk::HSlider->new(0, 1, 0, 0.1, 0.5);

ok( defined $b, 	"HSlider new()");
ok( $b->isa("Etk::HSlider"),	"Class Check");

