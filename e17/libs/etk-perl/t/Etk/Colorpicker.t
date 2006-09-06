use Test::More tests => 4;
use Etk;

my $b = Etk::Colorpicker->new();

ok( defined $b, 	"Colorpicker new()");
ok( $b->isa("Etk::Colorpicker"),	"Class Check");

$b->CurrentColorSet( { r=>10, g=>20, b=>30 });

is($b->CurrentColorGet()->{r}, 10,	"Color set and get");

$b->ModeSet(1); 
is($b->ModeGet(), 1,	"Mode set and get");


