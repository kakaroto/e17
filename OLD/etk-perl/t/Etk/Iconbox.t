use Test::More tests => 25;
use Etk;

my $b = Etk::Iconbox->new();

ok( defined $b, 	"Iconbox new()");
ok( $b->isa("Etk::Iconbox"),	"Class Check");

print "==== Model ====\n";
my $model = Etk::Iconbox::Model->new($b);
ok( defined $model, 	"Iconbox::Model new()");
ok( $model->isa("Etk::Iconbox::Model"),	"Class Check");

$model->GeometrySet(10, 10, 3, 3);

my @geo = $model->GeometryGet();
is($geo[0], 10,	"Geometry Get (width)");
is($geo[1], 10,	"Geometry Get (height)");
is($geo[2], 3,	"Geometry Get (xpadding)");
is($geo[3], 3,	"Geometry Get (ypadding)");

$model->LabelGeometrySet(1,2,10,10,0.5,1.0);

@geo = $model->LabelGeometryGet();
is($geo[0], 1,	"Label Geometry Get (x)");
is($geo[1], 2,	"Label Geometry Get (y)");
is($geo[2], 10,	"Label Geometry Get (width)");
is($geo[3], 10,	"Label Geometry Get (height)");
is($geo[4], 0.5,	"Label Geometry Get (xalign)");
is($geo[5], 1.0,	"Label Geometry Get (yalign)");

$model->IconGeometrySet(1, 1, 10, 10, 1, 1);

@geo = $model->IconGeometryGet();
is($geo[0], 1,	"Icon Geometry Get (x)");
is($geo[1], 1,	"Icon Geometry Get (y)");
is($geo[2], 10,	"Icon Geometry Get (width)");
is($geo[3], 10,	"Icon Geometry Get (height)");
is($geo[4], 1,	"Icon Geometry Get (fill)");
is($geo[5], 1,	"Icon Geometry Get (keep_aspect)");

print "==== Iconbox::Icon ====\n";

my $icon = $b->Append("/dev/null", "none", "Label");
ok( defined $icon, 	"Iconbox::Icon ");
ok( $icon->isa("Etk::Iconbox::Icon"),	"Class Check");

my @file = $icon->FileGet();
is($file[0], "/dev/null", 	"File Get");
is($icon->LabelGet(), "Label", 	"Label Get");

$icon->DataSet("moo");
is($icon->DataGet(), "moo", 	"Data set/get");


