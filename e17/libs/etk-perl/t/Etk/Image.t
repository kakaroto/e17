use Test::More 'no_plan'; # tests => 1;
use Etk;

my $b = Etk::Image->new();

ok( defined $b, 	"Image new()");
ok( $b->isa("Etk::Image"),	"Class Check");

$b->SetFromFile("/dev/null");
is($b->FileGet(), "/dev/null", "File set/get");

$b->KeepAspectSet(1);
is($b->KeepAspectGet(), 1, 	"Keep Aspect");

my $image2 = Etk::Image->new();
$image2->Copy($b);

is($b->KeepAspectGet(), 1,	"Copy");

