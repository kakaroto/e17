use Test::More tests => 6;
use Etk;

my $b = Etk::ProgressBar->new();

ok( defined $b, 	"ProgressBar new()");
ok( $b->isa("Etk::ProgressBar"),	"Class Check");

$b->TextSet("moo");

is($b->TextGet(), "moo", "Text Set/Get");

$b->FractionSet(0.5);
is($b->FractionGet(), 0.5, "Fraction Set/Get");

$b->PulseStepSet(0.3);
is($b->PulseStepGet(), 0.3, "PulseSet Set/Get");

$b->DirectionSet(1);
is($b->DirectionGet(), 1, "Direction Set/Get");
