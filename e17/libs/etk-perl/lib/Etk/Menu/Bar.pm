package Etk::Menu::Bar;
use strict;
use vars qw(@ISA);
require Etk::Menu::Shell;
@ISA = ("Etk::Menu::Shell");
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    $self->{WIDGET} = Etk::etk_menu_bar_new();
    bless($self, $class);
    return $self;
}

1;
