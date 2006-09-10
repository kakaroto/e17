use Test::More tests => 2;
use Etk;

Etk::Tooltips::Init();

my $b = Etk::Button->new();

Etk::Tooltips::Disable();
is(Etk::Tooltips::EnabledGet(), 0, "Disable");
Etk::Tooltips::Enable();
is(Etk::Tooltips::EnabledGet(), 1, "Enable");


