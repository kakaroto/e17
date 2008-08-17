use Test::More 'no_plan'; # tests => 1;
use Etk;

my $window = Etk::Window->new();

ok( defined $window, 	"Window new()");
ok( $window->isa("Etk::Window"),	"Class Check");

is( $window->TitleGet(), undef,	"TitleGet()");
$window->TitleSet("test2");
is( $window->TitleGet(), "test2",	"TitleSet()");

$window->Move(10, 10);
$window->Resize(100, 100);

my @geo = $window->GeometryGet();
ok($geo[2] == 100 && $geo[3] == 100,  	"Resize()");
ok($geo[0] == 10 && $geo[1] == 10,  	"Move()");

ok(1, "GeometryGet()");

$window->IconifiedSet(1);
ok($window->IconifiedGet(),	"IconifiedSet/Get");

SKIP: {
	skip "Need to be visible", 3;
$window->MaximizedSet(1);
is($window->MaximizedGet(),1,	"MaximizedSet/Get");

$window->StickySet(1);
is($window->StickyGet(),1,	"StickSet/Get");

$window->FullscreenSet(1);
is($window->FullscreenGet(),1,	"FullscreenSet/Get");

}


