use Test::More tests => 5;
use Etk;

my $b = Etk::Window->new();

ok( defined $b, 	"Window new()");
ok( $b->isa("Etk::Window"),	"Class Check");

$b->BorderWidthSet(10);
is($b->BorderWidthGet(), 10,	"Border Width");

my $button = Etk::Button->new();

$b->Add($button);
is($b->IsChild($button), 1, 	"Container Add/Check");

my @children = $b->ChildrenGet();
is(@children, 1, "Children Get");



