package Etk::Viewport;
use strict;
use vars qw(@ISA);
require Etk::Bin;
@ISA = ("Etk::Bin");

sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    $self->{WIDGET} = Etk::etk_viewport_new();
    bless($self, $class);
    return $self;
}

sub ViewportGet
{
    my $self = shift;
    return Etk::etk_viewport_type_get($self->{WIDGET});
}

1;
      
	  

 

