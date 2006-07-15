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
    my $self = shift;
    my @types = @_;
    if (@types > 0) 
    {
	Etk::etk_drag_types_set($self->{WIDGET}, @types);
    }
}

sub DataSet
{
    my $self = shift;
    my $data = shift;
    Etk::etk_drag_data_set($self->{WIDGET}, $data);
}

sub Begin
{
    my $self = shift;
    Etk::etk_drag_begin($self->{WIDGET});
}

sub ParentWidgetSet
{
    my $self = shift;
    my $widget = shift;
    Etk::etk_drag_parent_widget_set($self->{WIDGET}, $widget->{WIDGET});
}

sub ParentWidgetGet
{
    my $self = shift;
    my $widget = Etk::Widget->new();
    $widget->{WIDGET} = Etk::etk_drag_parent_widget_get($self->{WIDGET});
    return $widget;
}

1;
