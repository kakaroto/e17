package Etk::VSeparator;
use strict;
use vars qw(@ISA);
require Etk::Separator;
@ISA = ("Etk::Separator");
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    $self->{WIDGET} = Etk::etk_vseparator_new();
    bless($self, $class);
    return $self;
}

1;
