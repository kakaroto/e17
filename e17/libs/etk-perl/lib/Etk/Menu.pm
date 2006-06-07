package Etk::Menu;
use strict;
use vars qw(@ISA);
require Etk::Menu;
@ISA = ("Etk::Menu::Shell");
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
}

sub Popup
{
    my $self = shift;
    Etk::etk_menu_popup($self->{WIDGET});
}

sub Popdown
{
    my $self = shift;
    Etk::etk_menu_popdown($self->{WIDGET});
}

1;
