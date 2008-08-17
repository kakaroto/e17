use Test::More 'no_plan'; # tests => 1;
use Etk;

my $b = Etk::Button->new();

ok( defined $b, 	"Widget new()");
ok( $b->isa("Etk::Widget"),	"Class Check");

# TODO: complete this
