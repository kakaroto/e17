package Etk::Menu::Item::Separator;
use strict;
use vars qw(@ISA);
require Etk::Menu::Item;
@ISA = ("Etk::Menu::Item");
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    $self->{WIDGET} = Etk::etk_menu_item_separator_new();
    bless($self, $class);
    return $self;
}

1;
