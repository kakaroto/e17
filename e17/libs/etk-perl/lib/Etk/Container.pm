package Etk::Container;
use strict;
use vars qw(@ISA);
require Etk::Widget;
@ISA = ("Etk::Widget");
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    $self->{CHILDREN} = [];
    bless($self, $class);
    return $self;
}

sub Add
{
    my $self = shift;
    my $widget = shift;
    Etk::etk_container_add($self->{WIDGET}, $widget->{WIDGET});
    push @{$self->{CHILDREN}}, $widget;
    return $self;
}

sub Remove
{
    my $self = shift;
    my $widget = shift;
    Etk::etk_container_remove($self->{WIDGET}, $widget->{WIDGET});
    my @children = @{$self->{CHILDREN}};
    $self->{CHILDREN} = [];
    push @{$self->{CHILDREN}}, grep { $_ != $widget } @children;
    return $self;
}

sub BorderWidthSet
{
    my $self = shift;
    my $width = shift;
    Etk::etk_container_border_width_set($self->{WIDGET}, $width);
    return $self;
}

sub BorderWidthGet
{
    my $self = shift;
    return Etk::etk_container_border_width_get($self->{WIDGET});
}

sub ChildrenGet
{
    my $self = shift;
    return @{$self->{CHILDREN}};
}

sub IsChild
{
    my $self = shift;
    my $widget = shift;
    foreach (@{$self->{CHILDREN}}) 
    {
	return 1 if $_ == $widget;
    }
    return 0;
}

sub ChildSpaceFill
{
    my $self = shift;
    my $child = shift;
    my $child_space = shift;
    my $hfill = shift;
    my $vfill = shift;
    my $xalign = shift;
    my $yalign = shift;
    Etk::etk_container_child_space_fill($child->{WIDGET}, $child_space,
    	$hfill, $vfill, $xalign, $yalign);
    return $self;
}

1;
