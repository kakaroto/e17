use Test::More 'no_plan'; # tests => 1;
use Etk;

my $b = Etk::Label->new("test");

ok( defined $b, 	"Label new()");
ok( $b->isa("Etk::Label"),	"Class Check");

SKIP: {
	skip "Must be visible", 4;
	
	is($b->LabelGet(), "test", 	"Label Get");

	$b->LabelSet("moo");
	is($b->LabelGet(), "moo", 	"Label Set");

	$b->AlignmentSet(0.5, 0.2);
	
	my @al = $b->AlignmentGet();

	is($al[0], 0.5,	"Alignment (xalign)");
	is($al[1], 0.2,	"Alignment (yalign)");
}

