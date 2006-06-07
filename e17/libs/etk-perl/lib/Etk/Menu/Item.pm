package Etk::Menu::Item;
use strict;
use vars qw(@ISA);
require Etk::Widget;
@ISA = ("Etk::Widget");
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    if(@_ == 1)
    {	
	$self->{WIDGET} = Etk::etk_menu_item_new_with_label(shift);
    }
    else
    {
	$self->{WIDGET} = Etk::etk_menu_item_new();
    }
    bless($self, $class);
    return $self;
}

sub new_from_stock
{
    my $class = shift;
    my $self = $class->SUPER::new();
    my $stock_id = shift;
    $self->{WIDGET} = Etk::etk_menu_item_new_from_stock($stock_id);
    bless($self, $class);
    return $self;
}

sub LabelSet
{
    my $self = shift;
    my $label = shift;
    Etk::etk_menu_item_label_set($self->{WIDGET}, $label);
}

sub LabelGet
{
    my $self = shift;
    return Etk::etk_menu_item_label_get($self->{WIDGET});
}

sub SubmenuSet
{
    my $self = shift;
    my $submenu = shift;
    Etk::etk_menu_item_submenu_set($self->{WIDGET}, $submenu->{WIDGET});
}

sub Select
{
    my $self = shift;
    Etk::etk_menu_item_select($self->{WIDGET});
}

sub Deselect
{
    my $self = shift;
    Etk::etk_menu_item_deselect($self->{WIDGET});
}

sub Activate
{
    my $self = shift;
    Etk::etk_menu_item_activate($self->{WIDGET});
}

sub SetFromStock
{
    my $self = shift;
    my $stock_id = shift;
    Etk::etk_menu_item_set_from_stock($self->{WIDGET}, $stock_id);
}

1;
