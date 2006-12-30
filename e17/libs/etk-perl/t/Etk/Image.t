use Test::More 'no_plan'; # tests => 1;
use Etk;

my $b = Etk::Image->new();

ok( defined $b, 	"Image new()");
ok( $b->isa("Etk::Image"),	"Class Check");

$b->SetFromFile("/dev/null","key");
is($b->FileGet(), "key", "File set/get");

$b->KeepAspectSet(1);
is($b->KeepAspectGet(), 1, 	"Keep Aspect");

$b->AspectRatioSet(0.3);
is($b->AspectRatioGet(), 0.3, 	"Aspect Ratio");

my $image2 = Etk::Image->new();
$image2->Copy($b);

is($b->KeepAspectGet(), 1,	"Copy");

