use Test::More tests => 8;
use Etk;
use Etk::Constants qw/:messagedialog/;


my $hb = Etk::MessageDialog->new(Warning, ButtonsClose, "test");
ok( defined $hb, 	"MessageDialog new()");
ok( $hb->isa("Etk::MessageDialog"),	"Class Check");

is($hb->MessageTypeGet(), Warning, "MessageTypeGet");
is($hb->ButtonsGet(), ButtonsClose, "ButtonsGet");
is($hb->TextGet(), "test", "TextGet");

$hb->MessageTypeSet(Question);
is($hb->MessageTypeGet(), Question, "MessageTypeSet");
$hb->ButtonsSet(ButtonsNone);
is($hb->ButtonsGet(), ButtonsNone, "ButtonsSet");
$hb->TextSet("moo");
is($hb->TextGet(), "moo", "TextSet");
