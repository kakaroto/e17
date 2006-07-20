package Etk::ProgressBar;
use strict;
use vars qw(@ISA);
require Etk::Widget;
@ISA = ("Etk::Widget");

sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    if(@_ == 1) {	
	$self->{WIDGET} = Etk::etk_progress_bar_new_with_text(shift);
    } else {
	$self->{WIDGET} = Etk::etk_progress_bar_new();
    }	
    bless($self, $class);
    return $self;
}

sub TextSet
{
    my $self = shift;
    my $label = shift;
    Etk::etk_progress_bar_text_set($self->{WIDGET}, $label);
    return $self;
}

sub TextGet
{
    my $self = shift;
    return Etk::etk_progress_bar_text_get($self->{WIDGET});
}

sub FractionSet
{
    my $self = shift;
    my $fraction = shift;
    Etk::etk_progress_bar_fraction_set($self->{WIDGET}, $fraction);
    return $self;
}

sub FractionGet
{
    my $self = shift;
    return Etk::etk_progress_bar_fraction_get($self->{WIDGET});
}

sub Pulse
{
    my $self = shift;
    Etk::etk_progress_bar_pulse($self->{WIDGET});
    return $self;
}

sub PulseStepSet
{
    my $self = shift;
    my $pulse_step = shift;
    Etk::etk_progress_bar_pulse_step_set($self->{WIDGET}, $pulse_step);
    return $self;
}

sub PulseStepGet
{
    my $self = shift;
    return Etk::etk_progress_bar_pulse_step_get($self->{WIDGET});
}

sub DirectionSet
{
    my $self = shift;
    my $direction = shift;
    Etk::etk_progress_bar_direction_set($self->{WIDGET}, $direction);
    return $self;
}

sub DirectionGet
{
    my $self = shift;
    return Etk::etk_progress_bar_direction_get($self->{WIDGET});
}

1;
