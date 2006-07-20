package Etk::ToplevelWidget;
use strict;
use vars qw(@ISA);
require Etk::Bin;
@ISA = ("Etk::Bin");

sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    bless($self, $class);
    return $self;
}

sub GeometryGet
{
    my $self = shift;
    return Etk::etk_toplevel_widget_geometry_get($self->{WIDGET});
}

sub FocusedWidgetSet
{
    my $self = shift;
    my $widget = shift;
    Etk::etk_toplevel_widget_focused_widget_set($self->{WIDGET}, $widget->{WIDGET});
    return $self;
}

sub FocusedWidgetGet
{
    my $self = shift;
    my $widget = Etk::Widget->new();
    $widget->{WIDGET} = Etk::etk_toplevel_widget_focused_widget_get($self->{WIDGET});
    return $widget;
}

sub FocusedWidgetNextGet
{
    my $self = shift;
    my $widget = Etk::Widget->new();
    $widget->{WIDGET} = Etk::etk_toplevel_widget_focused_widget_next_get($self->{WIDGET});
    return $widget;
}

sub FocusedWidgetPrevGet
{
    my $self = shift;
    my $widget = Etk::Widget->new();
    $widget->{WIDGET} = Etk::etk_toplevel_widget_focused_widget_prev_get($self->{WIDGET});
    return $widget;
}

sub PointerPush
{
    my $self = shift;
    my $pointer = shift;
    Etk::etk_toplevel_widget_pointer_push($self->{WIDGET}, $pointer);
    return $self;
}

sub PointerPop
{
    my $self = shift;
    my $pointer = shift;
    Etk::etk_toplevel_widget_pointer_pop($self->{WIDGET}, $pointer);
    return $self;
}

1;
