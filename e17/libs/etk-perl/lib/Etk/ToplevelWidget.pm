package Etk::ToplevelWidget;
use strict;
use vars qw(@ISA);
require Etk::Bin;
@ISA = ("Etk::Bin");

use constant {

   PointerDefault	=> 0,
   PointerMove		=> 1,
   PointerHDoubleArrow	=> 2,
   PointerVDoubleArrow	=> 3,
   PointerResize	=> 4,
   PointerResizeTL	=> 5,
   PointerResizeT	=> 6,
   PointerResizeTR	=> 7,
   PointerResizeR	=> 8,
   PointerResizeBR	=> 9,
   PointerResizeB	=> 10,
   PointerResizeBL	=> 11,
   PointerResizeL	=> 12,
   PointerTextEdit	=> 13,
   PointerDndDrop	=> 14
};

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
}

sub PointerPop
{
    my $self = shift;
    my $pointer = shift;
    Etk::etk_toplevel_widget_pointer_pop($self->{WIDGET}, $pointer);
}

1;
