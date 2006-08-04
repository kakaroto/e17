use Test::More 'no_plan'; # tests => 1;
use Etk;

my $b = Etk::Filechooser->new();

ok( defined $b, 	"Filechooser new()");
ok( $b->isa("Etk::Filechooser"),	"Class Check");

$b->SelectMultipleSet(1);
is($b->SelectMultipleGet(), 1, 	"Select Multiple set/get");

$b->ShowHiddenSet(1);
is($b->ShowHiddenGet(), 1, 	"Show Hidden set/get");
