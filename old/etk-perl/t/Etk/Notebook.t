use Test::More tests => 11;
use Etk;

my $b = Etk::Notebook->new();

ok( defined $b, 	"Notebook new()");
ok( $b->isa("Etk::Notebook"),	"Class Check");

my $button = Etk::Button->new();
$button->LabelSet("button");

is($b->PageAppend("test", $button), 0, "PageAppend");
is($b->PageTabLabelGet(0), "test", "PageTabLabelGet");
$b->PageTabLabelSet(0, "moo");
is($b->PageTabLabelGet(0), "moo", "PageTabLabelSet");

my $child = $b->PageChildGet(0);
is($child->LabelGet(), "button", "PageChildGet");

SKIP: {
	skip "fixme", 5;
is($b->PagePrepend("test", $child), 0, "PagePrepend");
is($b->NumPagesGet(), 2, "NumPagesGet");

$b->CurrentPageSet(1);
is($b->CurrentPageGet(), 1, "CurrentPageGet/Set");

$b->PagePrev();
is($b->CurrentPageGet(), 0, "PagePrev");

$b->PageNext();
is($b->CurrentPageGet(), 1, "PageNext");
}

