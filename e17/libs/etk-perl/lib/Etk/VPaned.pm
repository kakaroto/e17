package Etk::VPaned;
use strict;
use vars qw(@ISA);
require Etk::Paned;
@ISA = ("Etk::Paned");

sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    $self->{WIDGET} = Etk::etk_vpaned_new();
    bless($self, $class);
    return $self;
}

1;
