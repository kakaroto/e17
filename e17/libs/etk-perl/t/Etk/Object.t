use Test::More 'no_plan'; # tests => 1;
use Etk;

my $b = Etk::Button->new();

ok( defined $b, 	"Button new()");
ok( $b->isa("Etk::Object"),	"Class Check");

$b->DataSet("moo", "test");

is($b->DataGet("moo"), "test", "Data set/get");

