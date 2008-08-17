use Test::More tests => 4;
use Etk;

my $b = Etk::Button->new();

ok( defined $b, 	"Button new()");
ok( $b->isa("Etk::Object"),	"Class Check");

$b->DataSet("moo", "test");

is($b->DataGet("moo"), "test", "Data set/get");

my $var = "unset";

$b->NotificationCallbackAdd("moo", sub {
	my $self = shift;
	my $property = shift;
	my $data = shift;
	$$data = "set";
}, \$var);

$b->Notify("moo");

is($var, "set", "Notification and callback"); 

