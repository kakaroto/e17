use Test::More tests => 7;
use Etk;

my $b = Etk::Button->new();

ok( defined $b, 	"Button new()");
ok( $b->isa("Etk::Button"),	"Class Check");

$b->LabelSet("bleh");

is($b->LabelGet(), "bleh",	"Label test");


my $image = Etk::Image->new();

$b->ImageSet($image);

my $i = $b->ImageGet();
ok($i->isa("Etk::Image"), 	"Image Set/Get");

$b->AlignmentSet(1, 0.5);

my ($xalign, $yalign) = $b->AlignmentGet();
is($xalign, 1, 	"Xalign");
is($yalign, 0.5, "Yalign");

$b->StyleSet(1);
is ($b->StyleGet(), 1, 	"Style set/get");

