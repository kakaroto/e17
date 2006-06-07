package Etk::CheckButton;
use strict;
use vars qw(@ISA);
require Etk::ToggleButton;
@ISA = ("Etk::ToggleButton");
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    if(@_ == 1)
    {
	$self->{WIDGET} = Etk::etk_check_button_new_with_label(shift);
    }
    else
    {
	$self->{WIDGET} = Etk::etk_check_button_new();
    }
    bless($self, $class);
    return $self;
}

1;
