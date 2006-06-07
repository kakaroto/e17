package Etk::Box;
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

sub PackStart
{
    my $self = shift;
    my $child = shift;
    my $expand = shift || 1;
    my $fill = shift || 1;
    my $padding = shift || 0;
    Etk::etk_box_pack_start($self->{WIDGET}, $child->{WIDGET}, $expand,
	$fill, $padding);
}

sub PackEnd
{
    my $self = shift;
    my $child = shift;
    my $expand = shift || 1;
    my $fill = shift || 1;
    my $padding = shift || 0;
    Etk::etk_box_pack_end($self->{WIDGET}, $child->{WIDGET}, $expand,
	$fill, $padding);
}

sub ChildPackingSet
{
    my $self = shift;
    my $child = shift;
    my $padding = shift;
    my $expand = shift;
    my $fill = shift;
    my $pack_end = shift;
    Etk::etk_box_child_packing_set($self->{WIDGET}, $child->{WIDGET}, $padding,
	$expand, $fill, $pack_end);
}

sub ChildPackingGet
{
    my $self = shift;
    my $child = shift;
    # RETURNS:
    # padding 
    # expand
    # fill
    # pack_end
    return Etk::etk_box_child_packing_get($self->{WIDGET}, $child->{WIDGET});
}

sub SpacingSet
{
    my $self = shift;
    my $spacing = shift;
    Etk::etk_box_spacing_set($self->{WIDGET}, $spacing);
}

sub SpacingGet
{
    my $self = shift;
    return Etk::etk_box_spacing_get($self->{WIDGET});
}

sub HomogenousSet
{
    my $self = shift;
    my $homogenous = shift;
    Etk::etk_box_homogenous_set($self->{WIDGET}, $homogenous);
}

sub HomogenousGet
{
    my $self = shift;
    return Etk::etk_box_homogenous_Get($self->{WIDGET});
}

1;
