package Etk::Bin;
use strict;
use vars qw(@ISA);
require Etk::Container;
@ISA = ("Etk::Container");
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    bless($self, $class);
    return $self;
}

sub ChildSet
{
    my $self = shift;
    my $widget = shift;
    Etk::etk_bin_child_set($self->{WIDGET}, $widget->{WIDGET});
    return $self;
}

1;
