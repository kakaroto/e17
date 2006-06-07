package Etk::VBox;
use strict;
use vars qw(@ISA);
require Etk::Box;
@ISA = ("Etk::Box");
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    my $homogenous = shift;
    my $spacing = shift;
    $self->{WIDGET} = Etk::etk_vbox_new($homogenous, $spacing);
    bless($self, $class);
    return $self;
}

1;
