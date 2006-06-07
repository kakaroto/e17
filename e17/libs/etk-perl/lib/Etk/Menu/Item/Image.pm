package Etk::Menu::Item::Image;
use strict;
use vars qw(@ISA);
require Etk::Menu::Item;
@ISA = ("Etk::Menu::Item");
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    if(@_ == 1)
    {
	$self->{WIDGET} = Etk::etk_menu_item_image_new_with_label(shift);
    }
    else
    {
	$self->{WIDGET} = Etk::etk_menu_item_image_new();
    }
    bless($self, $class);
    return $self;
}

sub new_from_stock
{
    my $class = shift;
    my $self = $class->SUPER::new();
    my $stock_id = shift;
    $self->{WIDGET} = Etk::etk_menu_item_image_new_from_stock($stock_id);
    bless($self, $class);
    return $self;
}

sub ImageSet
{
    my $self = shift;
    my $image = shift;
    Etk::etk_menu_item_image_set($self->{WIDGET}, $image->{WIDGET});
}

1;
