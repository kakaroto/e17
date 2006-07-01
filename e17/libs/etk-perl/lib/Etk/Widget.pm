package Etk::Widget;
use strict;
use vars qw(@ISA);
require Etk::Object;
@ISA = ("Etk::Object");
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    $self->{WIDGET} = undef;
    bless($self, $class);
    return $self;
}

sub Show
{
    my $self = shift;
    Etk::etk_widget_show($self->{WIDGET});
}

sub ShowAll
{
    my $self = shift;
    Etk::etk_widget_show_all($self->{WIDGET});
}

sub Hide
{
    my $self = shift;
    Etk::etk_widget_hide($self->{WIDGET});
}

sub HideAll
{
    my $self = shift;
    Etk::etk_widget_hide_all($self->{WIDGET});
}

sub isVisible
{
    my $self = shift;
    return Etk::etk_widget_is_visible($self->{WIDGET});
}

sub VisibilityLockedSet
{
    my $self = shift;
    Etk::etk_widget_visibility_locked_set($self->{WIDGET}, shift);
}

sub VisibilityLockedGet
{
    my $self = shift;
    return Etk::etk_widget_visibility_locked_get($self->{WIDGET});
}

sub Raise
{
    my $self = shift;
    Etk::etk_widget_raise($self->{WIDGET});
}

sub Lower
{
    my $self = shift;
    Etk::etk_widget_lower($self->{WIDGET});
}

sub SizeRequestSet
{
    my $self = shift;
    my $width = shift;
    my $height = shift;
    Etk::etk_widget_size_request_set($self->{WIDGET}, $width, $height);
}

sub Enter
{
    my $self = shift;
    Etk::etk_widget_enter($self->{WIDGET});
}

sub Leave
{
    my $self = shift;
    Etk::etk_widget_leave($self->{WIDGET});
}

sub Focus
{
    my $self = shift;
    Etk::etk_widget_focus($self->{WIDGET});
}

sub Unfocus
{
    my $self = shift;
    Etk::etk_widget_unfocus($self->{WIDGET});
}

sub PassMouseEventsSet
{
    my $self = shift;
    Etk::etk_widget_pass_mouse_events_set($self->{WIDGET}, shift);
}

sub DndDestSet
{
    my $self = shift;
    my $on = shift;
    Etk::etk_widget_dnd_dest_set($self->{WIDGET}, $on);
}

sub DndDestGet
{
    my $self = shift;
    return Etk::etk_widget_dnd_dest_get($self->{WIDGET});
}

sub DndDestWidgetsGet
{
    return map {
	my $widget = Etk::Widget->new();
	$widget->{WIDGET} = $_;
	$_ = $widget;
    } Etk::etk_widget_dnd_dest_widgets_get();

}

sub DndSourceSet
{
    my $self = shift;
    my $on = shift;
    Etk::etk_widget_dnd_source_set($self->{WIDGET}, $on);
}

sub DndSourceGet
{
    my $self = shift;
    return Etk::etk_widget_dnd_source_get($self->{WIDGET});
}

sub DndDragWidgetSet
{
    my $self = shift;
    my $drag = shift;
    Etk::etk_widget_dnd_drag_widget_set($self->{WIDGET}, $drag->{WIDGET});
}

sub DndDragWidgetGet
{
    my $self = shift;
    return Etk::etk_widget_dnd_drag_widget_get($self->{WIDGET});
}

sub DndDragDataSet
{
    # TODO
}

sub DndFilesGet
{
    my $self = shift;
    return Etk::etk_widget_dnd_files_get($self->{WIDGET});
}

sub DndTypesSet
{
    my $self = shift;
    my @types = @_;
    if (@types > 0) 
    {
    	Etk::etk_widget_dnd_types_set($self->{WIDGET}, @types);
    }
}

sub DndTypesGet
{
    my $self = shift;
    return Etk::etk_widget_dnd_types_get($self->{WIDGET});
}

sub DndInternalGet
{
    my $self = shift;
    return Etk::etk_widget_dnd_internal_get($self->{WIDGET});
}

sub DndInternalSet
{
    my $self = shift;
    my $on = shift;
    Etk::etk_widget_dnd_internal_set($self->{WIDGET}, $on);
}

1;
