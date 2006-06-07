package Etk::Paned;
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

sub Child1Set
{
    my $self = shift;
    my $child = shift;
    my $expand = shift;
    Etk::etk_paned_child1_set($self->{WIDGET}, $child->{WIDGET}, $expand);
}

sub Child2Set
{
    my $self = shift;
    my $child = shift;
    my $expand = shift;
    Etk::etk_paned_child2_set($self->{WIDGET}, $child->{WIDGET}, $expand);
}

sub Child1Get
{
    my $self = shift;
    my $child = Etk::Widget->new();
    $child->{WIDGET} = Etk::etk_paned_child1_get($self->{WIDGET});
    return $child;
}

sub Child2Get
{
    my $self = shift;
    my $child = Etk::Widget->new();
    $child->{WIDGET} = Etk::etk_paned_child2_set($self->{WIDGET});
    return $child;
}

sub PositionSet
{
    my $self = shift;
    my $position = shift;
    Etk::etk_paned_position_set($self->{WIDGET}, $position);
}

sub PositionGet
{
    my $self = shift;
    return Etk::etk_paned_position_get($self->{WIDGET});
}

1;
