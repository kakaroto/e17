use Test::More tests => 4;
use Etk;

my $b = Etk::ToolButton->new();

ok( defined $b, 	"ToolButton new()");
ok( $b->isa("Etk::ToolButton"),	"Class Check");

$b = Etk::ToolToggleButton->new();

ok( defined $b, 	"ToolToggleButton new()");
ok( $b->isa("Etk::ToolToggleButton"),	"Class Check");


