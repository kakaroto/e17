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
}

sub ColorSet
{
    my $self = shift;
    my $color = shift;
    Etk::etk_colorpicker_current_color_set($self->{WIDGET}, $color);
}

sub ColorGet
{
    my $self = shift;
    return Etk::etk_colorpicker_current_color_get($self->{WIDGET});
}

package Etk::Colorpicker::Mode;

use constant  {

	H	=> 0,
	S	=> 1,
	V	=> 2,
	R	=> 3,
	G	=> 4,
	B	=> 5

};

1;
