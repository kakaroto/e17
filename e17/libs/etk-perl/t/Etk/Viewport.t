use Test::More tests => 2;
use Etk;

my $v = Etk::Viewport->new();

ok( defined $v, 	"Viewport new()");
ok( $v->isa("Etk::Viewport"),	"Class Check");

