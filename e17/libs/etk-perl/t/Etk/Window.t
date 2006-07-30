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

$window->Iconify();
ok($window->IsIconified(),	"Iconify(), isIconified()");
$window->Deiconify();
ok(! $window->IsIconified(),	"Deiconify()");

$window->Maximize();
ok($window->IsMaximized(),	"Maximize(), isMaximized()");
$window->Unmaximize();
ok(! $window->IsMaximized(),	"Unmaximize()");

$window->Fullcreen();
ok($window->IsFullscreen(),	"Fullscreen(), isFullscreen()");
$window->Unfullscreen();
ok(! $window->IsFullscreen(),	"Unfullscreen()");

$window->Stick();
ok($window->IsSticky(),	"Stick(), isSticky()");
$window->Unstick();
ok(! $window->IsSticky(),	"Unstick()");


