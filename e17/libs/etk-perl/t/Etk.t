use Test::More 'no_plan'; # tests => 1;
use Etk;

# testing Window here mainly

my $window = Etk::Window->new("test");

ok( defined $window, 	"Window new()");
ok( $window->isa("Etk::Window"),	"Class Check");

ok( $window->TitleGet() eq "test",	"TitleGet()");
$window->TitleSet("test2");
ok( $window->TitleGet() eq "test2",	"TitleSet()");

$window->Move(10, 10);
$window->Resize(100, 100);

my @geo = $window->GeometryGet();
ok($geo[2] == 100 && $geo[3] == 100,  	"Resize()");
ok($geo[0] == 10 && $geo[1] == 10,  	"Move()");

ok(1, "GeometryGet()");


$window->Iconify();
ok($window->isIconified(),	"Iconify(), isIconified()");
$window->Deiconify();
ok(! $window->isIconified(),	"Deiconify()");

$window->Maximize();
ok($window->isMaximized(),	"Maximize(), isMaximized()");
$window->Unmaximize();
ok(! $window->isMaximized(),	"Unmaximize()");

$window->Fullcreen();
ok($window->isFullscreen(),	"Fullscreen(), isFullscreen()");
$window->Unfullscreen();
ok(! $window->isFullscreen(),	"Unfullscreen()");

$window->Stick();
ok($window->isSticky(),	"Stick(), isSticky()");
$window->Unstick();
ok(! $window->isSticky(),	"Unstick()");


