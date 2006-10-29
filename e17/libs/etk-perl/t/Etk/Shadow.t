use Test::More 'no_plan'; # tests => 1;
use Etk;

my $s = Etk::Shadow->new();

ok( defined $s, 	"Shadow new()");
ok( $s->isa("Etk::Shadow"),	"Class Check");

$s->ShadowColorSet(10, 20, 30);
my ($r, $g, $b) = $s->ShadowColorGet();
ok( $r == 10 && $g == 20 && $b == 30, "ShadowColorSet/Get");


$s->ShadowSet(0, 1, 2, 3, 4, 5);

my @sp = $s->ShadowGet();

ok(
 ($sp[0] == 0 &&
  $sp[1] == 1 &&
  $sp[2] == 2 &&
  $sp[3] == 3 &&
  $sp[4] == 4 &&
  $sp[5] == 5) , "ShadowSet/Get");


$s->BorderSet(10);

is($s->BorderGet(), 10, "Border Set/Get");

$s->BorderColorSet(10, 20, 30, 40);
@sp = $s->BorderColorGet();

ok(
 ($sp[0] == 10 &&
  $sp[1] == 20 &&
  $sp[2] == 30 &&
  $sp[3] == 40) , "BorderColorSet/Get");



