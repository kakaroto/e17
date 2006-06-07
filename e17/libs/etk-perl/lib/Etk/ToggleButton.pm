package Etk::ToggleButton;
use strict;
use vars qw(@ISA);
require Etk::Button;
@ISA = ("Etk::Button");
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    if(@_ == 1)
    {
	$self->{WIDGET} = Etk::etk_toggle_button_new_with_label(shift);
    }
    else
    {
	$self->{WIDGET} = Etk::etk_toggle_button_new();
    }
    bless($self, $class);
    return $self;
}

sub Toggle
{
    my $self = shift;
    Etk::etk_toggle_button_toggle($self->{WIDGET});
}

sub ActiveSet
{
    my $self = shift;
    my $active = shift;
    Etk::etk_toggle_button_active_set($self->{WIDGET}, $active);
}

sub ActiveGet
{
    my $self = shift;
    return Etk::etk_toggle_button_active_get($self->{WIDGET});
}

1;
