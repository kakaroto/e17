use Test::More tests => 5;
use Etk;

my $b = Etk::Entry->new();

ok( defined $b, 	"Entry new()");
ok( $b->isa("Etk::Entry"),	"Class Check");

$b->PasswordModeSet(1);
is($b->PasswordModeGet(), 1, 	"Password set/get");

$b->TextSet("test");
is($b->TextGet(), "test", 	"Text set/get");

my $i = Etk::Image->new_from_file("/dev/null");
$b->ImageSet(1, $i);
is($b->ImageGet(1)->{ETK}, $i->{ETK}, "Image set/get");

