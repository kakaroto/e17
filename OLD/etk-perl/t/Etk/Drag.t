use Test::More tests => 4;
use Etk;

my $t = Etk::Button->new();

my $b = Etk::Drag->new($t);

ok( defined $b, 	"Drag new()");
ok( $b->isa("Etk::Drag"),	"Class Check");

SKIP: {
	skip "hmm..", 2;
is($b->ParentWidgetGet()->{ETK}, $t->{ETK}, "ParentWidgetGet()");

my $t2 = Etk::Button->new();
$b->ParentWidgetSet($t2);
is($b->ParentWidgetGet()->{ETK}, $t2->{ETK}, "ParentWidgetSet()");

}

