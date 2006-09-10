use Test::More 'no_plan';# tests => 7;
use Etk;

my $b = Etk::HScrollbar->new(0, 1, 0.3, 0.1 ,0.5, 0.6);

ok( defined $b, 	"HScrollbar new()");
ok( $b->isa("Etk::HScrollbar"),	"Class Check");

