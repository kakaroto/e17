use Test::More tests => 5;
use Etk;

my $b = Etk::ToggleButton->new();

ok( defined $b, 	"ToggleButton new()");
ok( $b->isa("Etk::ToggleButton"),	"Class Check");

is($b->ActiveGet(), 0, "ActiveGet()");
$b->ActiveSet(1);
is($b->ActiveGet(), 1, "ActiveSet()");

$b->Toggle();
is($b->ActiveGet(), 0, "Toggle()");

