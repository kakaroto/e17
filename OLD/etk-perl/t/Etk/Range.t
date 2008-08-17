use Test::More tests => 6;
use Etk;

my $b = Etk::ScrolledView->new();

my $r = $b->HScrollbarGet();

ok( defined $r, 	"Range defined");
ok( $r->isa("Etk::Range"),	"Class Check");

$r->RangeSet(0.0, 1.0);
my ($l, $u) = $r->RangeGet();
ok( $l == 0 && $u == 1, "Range Set/Get");

$r->ValueSet(0.6);
is($r->ValueGet(), 0.6, "Value Set/Get");

$r->IncrementsSet(0.1, 0.4);
my ($s, $p) = $r->IncrementsGet();
ok( $s == 0.1 && $p == 0.4, "Increments Set/Get");

$r->PageSizeSet(0.3);
is($r->PageSizeGet(), 0.3, "PageSize Set/Get");


