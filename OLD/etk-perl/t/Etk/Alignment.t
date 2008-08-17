use Test::More tests => 12;
use Etk;

my $al = Etk::Alignment->new(0.5, 0.5, 1, 1);

ok( defined $al, 	"Alignment new()");
ok( $al->isa("Etk::Alignment"),	"Class Check");

my ($xalign, $yalign, $xscale, $yscale) = $al->Get();
is( $xalign, 0.5,	"xalign");
is( $yalign, 0.5,	"yalign");
is( $xscale, 1,	"xscale");
is( $yscale, 1,	"yscale");

$al->Set(1, 1, 1, 1);

($xalign, $yalign, $xscale, $yscale) = $al->Get();
ok( $xalign == 1 && $yalign == 1,	"Set()");

$al = Etk::Alignment->new();
ok( defined $al, 	"Alignment new() with default values");
($xalign, $yalign, $xscale, $yscale) = $al->Get();
is( $xalign, 0.5,	"xalign");
is( $yalign, 0.5,	"yalign");
is( $xscale, 1,	"xscale");
is( $yscale, 1,	"yscale");
