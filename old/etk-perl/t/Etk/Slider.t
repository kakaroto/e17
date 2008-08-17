use Test::More tests => 5;
use Etk;

my $b = Etk::HSlider->new(0, 1, 0, 0.1, 0.5);

ok( defined $b, 	"HSlider new()");
ok( $b->isa("Etk::HSlider"),	"Class Check");

$b->LabelSet("moo");
is($b->LabelGet(), "moo", "Label Set/Get");

$b->InvertedSet(1);
is($b->InvertedGet(), 1, "Inverted Set/Get");

$b->UpdatePolicySet(2);
is($b->UpdatePolicyGet(), 2, "UpdatePolicy Set/Get");

