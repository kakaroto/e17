package Etk::Dnd;
use strict;
require Etk;
sub Init
{
    Etk::etk_dnd_init();
}

sub Shutdown
{
    Etk::etk_dnd_shutdown();
}

1;
