use Test::More tests => 3;
use Etk;

my $b = Etk::StatusBar->new();

ok( defined $b, 	"StatusBar new()");
ok( $b->isa("Etk::StatusBar"),	"Class Check");

$b->HasResizeGripSet(1);
is($b->HasResizeGripGet(), 1, "HasResizeGrip Set/Get");


