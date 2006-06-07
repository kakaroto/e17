package Etk::Canvas;
use strict;
use vars qw(@ISA);
require Etk::Widget;
@ISA = ("Etk::Widget");
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    $self->{WIDGET} = Etk::etk_canvas_new();
    bless($self, $class);
    return $self;
}

sub ObjectAdd
{
    # TODO
}

sub ObjectRemove
{
    # TODO
}

1;

  
