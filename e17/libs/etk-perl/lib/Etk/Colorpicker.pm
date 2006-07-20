package Etk::Colorpicker;
use strict;
use vars qw(@ISA);
require Etk::Widget;
@ISA = ("Etk::Widget");
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    $self->{WIDGET} = Etk::etk_colorpicker_new();
    bless($self, $class);
    return $self;
}

sub ModeGet
{
    my $self = shift;
    return Etk::etk_colorpicker_mode_get($self->{WIDGET});
}

sub ModeSet
{
    my $self = shift;
    my $mode = shift;
    Etk::etk_colorpicker_mode_set($self->{WIDGET}, $mode);
    return $self;
}

sub ColorSet
{
    my $self = shift;
    my $color = shift;
    Etk::etk_colorpicker_current_color_set($self->{WIDGET}, $color);
    return $self;
}

sub ColorGet
{
    my $self = shift;
    return Etk::etk_colorpicker_current_color_get($self->{WIDGET});
}

1;
