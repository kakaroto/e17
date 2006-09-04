use Test::More tests => 2;
use Etk;

my $b = Etk::Button->new();
my $b2 = Etk::Button->new();

ok( $b->isa("Etk::Bin"),	"Class Check");

$b->ChildSet($b2);

my $b3 = $b->ChildGet();

ok ( $b3->isa("Etk::Button"), 	"ChildSet/Get");


