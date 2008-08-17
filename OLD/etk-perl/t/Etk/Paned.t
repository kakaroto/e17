use Test::More  tests => 7;
use Etk;

my $b = Etk::HPaned->new();

ok( defined $b, 	"HPaned new()");
ok( $b->isa("Etk::HPaned"),	"Class Check");

my $b1 = Etk::Button->new();
my $b2 = Etk::Button->new();

$b->Child1Set($b1, 1);
$b->Child2Set($b2, 0);

is($b->Child1Get()->{ETK}, $b1->{ETK}, "Child1 Set/Get");
is($b->Child2Get()->{ETK}, $b2->{ETK}, "Child2 Set/Get");

is($b->Child1ExpandGet(), 1, "Expand Get");

$b->Child2ExpandSet(1);
is($b->Child2ExpandGet(), 1, "Expand Set");

SKIP: {
	skip "..", 1;
$b->PositionSet(123);

is($b->PositionGet(), 123, "Position Set/Get");
}


