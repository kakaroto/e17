use Test::More tests => 4;
use Etk;

my $b = Etk::Dialog->new();

ok( defined $b, 	"Dialog new()");
ok( $b->isa("Etk::Dialog"),	"Class Check");

$b->HasSeparatorSet(1);
is($b->HasSeparatorGet(), 1, "HasSeperator");

my $button = $b->ButtonAdd("bleh", 0);
ok($button->isa("Etk::Button"),	"ButtonAdd");

# other checks are visual
