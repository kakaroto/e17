package Etk::Container;
use strict;
use vars qw(@ISA);
require Etk::Widget;
@ISA = ("Etk::Widget");
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    $self->{CHILDREN} = ();
    bless($self, $class);
    return $self;
}

sub Add
{
    my $self = shift;
    # confess "usage: \$container->Add(\$widget)" unless @_ == 1;
    my $widget = shift;
    Etk::etk_container_add($self->{WIDGET}, $widget->{WIDGET});
    push @{$self->{CHILDREN}}, $widget;
}

sub Remove
{
    my $self = shift;
    my $widget = shift;
    Etk::etk_container_remove($self->{WIDGET}, $widget->{WIDGET});
}

sub BorderWidthSet
{
    my $self = shift;
    my $width = shift;
    Etk::etk_container_border_width_set($self->{WIDGET}, $width);
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
    return Etk::etk_container_is_child($self->{WIDGET}, $widget->{WIDGET});
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
}

1;
