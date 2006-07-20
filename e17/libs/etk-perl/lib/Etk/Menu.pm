package Etk::Menu;
use strict;
use vars qw(@ISA);
require Etk::Menu::Shell;
@ISA = ("Etk::Menu::Shell");

use Etk::Menu::Bar;
use Etk::Menu::Item;

use Etk::Menu::Item::Check;
use Etk::Menu::Item::Image;
use Etk::Menu::Item::Radio;
use Etk::Menu::Item::Separator;

sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    $self->{WIDGET} = Etk::etk_menu_new();
    bless($self, $class);
    return $self;
}

sub PopupAtXY
{
    my $self = shift;
    my $x = shift;
    my $y = shift;
    Etk::etk_menu_popup_at_xy($self->{WIDGET}, $x, $y);
    return $self;
}

sub Popup
{
    my $self = shift;
    Etk::etk_menu_popup($self->{WIDGET});
    return $self;
}

sub Popdown
{
    my $self = shift;
    Etk::etk_menu_popdown($self->{WIDGET});
    return $self;
}

1;
