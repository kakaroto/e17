package Etk::Combobox::Item;
use strict;
use vars qw(@ISA);
require Etk::Widget;
@ISA = ("Etk::Widget");
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    bless($self, $class);
    return $self;    
}

1;
