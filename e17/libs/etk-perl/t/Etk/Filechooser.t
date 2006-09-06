use Test::More tests => 4;
use Etk;

my $b = Etk::Filechooser->new();

ok( defined $b, 	"Filechooser new()");
ok( $b->isa("Etk::Filechooser"),	"Class Check");

$b->ShowHiddenSet(1);
is($b->ShowHiddenGet(), 1, 	"Show Hidden set/get");

$b->CurrentFolderSet("/");
is($b->CurrentFolderGet(), "/", 	"CurrentFolder Set/Get");
