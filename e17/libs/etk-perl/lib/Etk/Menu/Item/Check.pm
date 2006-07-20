package Etk::Menu::Item::Check;
use strict;
use vars qw(@ISA);
require Etk::Menu::Item;
@ISA = ("Etk::Menu::Item");
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    if(@_ == 1) {
	$self->{WIDGET} = Etk::etk_menu_item_check_new_with_label(shift);
    } else {
	$self->{WIDGET} = Etk::etk_menu_item_check_new();
    }
    bless($self, $class);
    return $self;
}

sub ActiveSet
{
    my $self = shift;
    my $active = shift;
    Etk::etk_menu_item_check_active_set($self->{WIDGET}, $active);
    return $self;
}

sub ActiveGet
{
    my $self = shift;
    return Etk::etk_menu_item_check_active_get($self->{WIDGET});
}

1;
