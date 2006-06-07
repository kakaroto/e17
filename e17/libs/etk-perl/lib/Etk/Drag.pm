package Etk::Drag;
use strict;
use vars qw(@ISA);
require Etk::Window;
@ISA = ("Etk::Window");
sub new
{
    my $class = shift;
    my $widget = shift;
    my $self = $class->SUPER::new_nocreate();
    $self->{WIDGET} = Etk::etk_drag_new($widget->{WIDGET});
    bless($self, $class);
    return $self;
}

sub TypesSet
{
    # TODO
}

sub DataSet
{
    # TODO
}

1;
