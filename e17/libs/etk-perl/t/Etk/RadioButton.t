use Test::More 'no_plan';# tests => 7;
use Etk;

my $b = Etk::RadioButton::new_with_label("test");

ok( defined $b, 	"RadioButton new()");
ok( $b->isa("Etk::RadioButton"),	"Class Check");

