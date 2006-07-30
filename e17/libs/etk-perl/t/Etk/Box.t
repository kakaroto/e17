use Test::More 'no_plan'; # tests => 1;
use Etk;

my $hb = Etk::HBox->new(0, 1);

ok( defined $hb, 	"HBox new()");
ok( $hb->isa("Etk::HBox"),	"Class Check");

is($hb->SpacingGet(), 1,	"Spacing Get");
$hb->SpacingSet(4);
is($hb->SpacingGet(), 4,	"Spacing Set");
is($hb->HomogeneousGet(), 0,	"Homo Get");
$hb->HomogeneousSet(5);
is($hb->HomogeneousGet(), 5,	"Homo Set");


my $button = Etk::Button->new();
$hb->PackStart($button, 1, 0, 5);
my ($padding, $expand, $fill, $pack_end) = $hb->ChildPackingGet($button);

is($padding, 5, 	"Padding set");
is($expand, 1, 	"Expand set");
is($fill, 0, 	"Fill set");


$hb = Etk::VBox->new(0, 1);

ok( defined $hb, 	"VBox new()");
ok( $hb->isa("Etk::VBox"),	"Class Check");

is($hb->SpacingGet(), 1,	"Spacing Get");
$hb->SpacingSet(4);
is($hb->SpacingGet(), 4,	"Spacing Set");
is($hb->HomogeneousGet(), 0,	"Homo Get");
$hb->HomogeneousSet(5);
is($hb->HomogeneousGet(), 5,	"Homo Set");


$hb->PackStart($button, 1, 0, 5);
my ($padding, $expand, $fill, $pack_end) = $hb->ChildPackingGet($button);

is($padding, 5, 	"Padding set");
is($expand, 1, 	"Expand set");
is($fill, 0, 	"Fill set");


