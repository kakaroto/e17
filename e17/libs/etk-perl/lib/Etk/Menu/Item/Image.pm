package Etk::Menu::Item::Image;
use strict;
use vars qw(@ISA);
require Etk::Menu::Item;
@ISA = ("Etk::Menu::Item");
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    if(@_ == 1) {
	my $arg = shift;
	$self->{WIDGET} = $arg + 0 eq $arg ?
		Etk::etk_menu_item_image_new_from_stock($arg):
		Etk::etk_menu_item_image_new_with_label($arg);
    } else {
	$self->{WIDGET} = Etk::etk_menu_item_image_new();
    }
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
