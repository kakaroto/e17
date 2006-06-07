package Etk::PopupWindow;
use strict;
use vars qw(@ISA);
require Etk::Window;
@ISA = ("Etk::Window");
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new_nocreate();
    bless($self, $class);
    return $self;
}

sub PopupAtXY
{
    my $self = shift;
    my $x = shift;
    my $y = shift;
    Etk::etk_popup_window_popup_at_xy($self->{WIDGET}, $x, $y);
}

sub Popup
{
    my $self = shift;
    Etk::etk_popup_window_popup($self->{WIDGET});
}

sub Popdown
{
    my $self = shift;
    Etk::etk_popup_window_popdown($self->{WIDGET});
}

sub PopdownAll
{
    Etk::etk_popup_window_popdown_all();
}

sub IsPoppedUp
{
    my $self = shift;
    return Etk::etk_popup_window_is_popped_up($self->{WIDGET});
}

sub FocusedWindowSet
{
    my $self = shift;
    Etk::etk_popup_window_focused_window_set($self->{WIDGET});
}

sub FocusedWindowGet
{
    my $self = shift;
    return Etk::etk_popup_window_focused_window_get($self->{WIDGET});
}

1;
