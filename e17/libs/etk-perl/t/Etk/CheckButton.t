use Test::More 'no_plan'; # tests => 1;
use Etk;

my $b = Etk::CheckButton->new();

ok( defined $b, 	"CheckButton new()");
ok( $b->isa("Etk::CheckButton"),	"Class Check");


