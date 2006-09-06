use Test::More tests => 20;
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
$hb->Append($button, 0, 0, 5);
my ($fill, $padding) = $hb->ChildPackingGet($button);

is($padding, 5, 	"Padding set");
is($fill, 0, 	"Fill set");

my $button2 = Etk::Button->new();
$hb->Append($button2, 0, 0, 5);
my ($group, $pos) = $hb->ChildPositionGet($button2);

is($group, 0, 	"Position: group");
is($pos, 1, 	"Position: pos");

$hb->ChildPositionSet($button2, 0, 0);
($group, $pos) = $hb->ChildPositionGet($button2);
is($pos, 0, 	"Position set");

my $button3 = $hb->ChildGetAt(0, 1);

is ($button3->{ETK}, $button->{ETK}, "ChildGetAt");

$hb = Etk::VBox->new(0, 1);

ok( defined $hb, 	"VBox new()");
ok( $hb->isa("Etk::VBox"),	"Class Check");

is($hb->SpacingGet(), 1,	"Spacing Get");
$hb->SpacingSet(4);
is($hb->SpacingGet(), 4,	"Spacing Set");
is($hb->HomogeneousGet(), 0,	"Homo Get");
$hb->HomogeneousSet(5);
is($hb->HomogeneousGet(), 5,	"Homo Set");


$hb->Append($button, 0, 0, 5);
($fill, $padding) = $hb->ChildPackingGet($button);

is($padding, 5, 	"Padding set");
is($fill, 0, 	"Fill set");


