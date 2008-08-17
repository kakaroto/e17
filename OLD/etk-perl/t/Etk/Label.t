use Test::More tests => 6;
use Etk;

my $b = Etk::Label->new("test");

ok( defined $b, 	"Label new()");
ok( $b->isa("Etk::Label"),	"Class Check");

is($b->Get(), "test", 	"Label Get");

$b->Set("moo");
is($b->Get(), "moo", 	"Label Set");

$b->AlignmentSet(0.5, 0.2);

my @al = $b->AlignmentGet();

is($al[0], 0.5,	"Alignment (xalign)");
# XXX - hack - what's up with the rounding here?
is(sprintf("%0.1f", $al[1]), 0.2,	"Alignment (yalign)");

