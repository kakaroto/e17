package Etk::Widget;
use strict;
use vars qw(@ISA);
require Etk::Object;
@ISA = ("Etk::Object");

use AutoLoader;

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

sub SizeRecalcQueue
{
    my $self = shift;
    Etk::etk_widget_size_recalc_queue($self->{WIDGET});
}

sub RedrawQueue
{
    my $self = shift;
    Etk::etk_widget_redraw_queue($self->{WIDGET});
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

sub PassMouseEventsGet
{
    my $self = shift;
    return Etk::etk_widget_pass_mouse_events_get($self->{WIDGET});
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

# TODO drag_*

sub ThemeFileSet
{
    my $self = shift;
    my $theme_file = shift;
    Etk::etk_widget_theme_file_set($self->{WIDGET}, $theme_file);

}

sub ThemeFileGet
{
    my $self = shift;
    return Etk::etk_widget_theme_file_get($self->{WIDGET});

}

sub ThemeGroupSet
{
    my $self = shift;
    my $theme_group = shift;
    Etk::etk_widget_theme_group_set($self->{WIDGET}, $theme_group);
}

sub ThemeGroupGet
{
    my $self = shift;
    return Etk::etk_widget_theme_group_get($self->{WIDGET});
}

sub ThemeParentSet
{
    my $self = shift;
    my $parent = shift;
    Etk::etk_widget_theme_parent_set($self->{WIDGET}, $parent->{WIDGET});
}

sub ThemeParentGet
{
    my $self = shift;
    return Etk::etk_widget_theme_parent_get($self->{WIDGET});
}

sub GeometryGet
{
    my $self = shift;
    return Etk::etk_widget_geometry_get($self->{WIDGET});
}

sub InnerGeometryGet
{
    my $self = shift;
    return Etk::etk_widget_inner_geometry_get($self->{WIDGET});
}

sub HasEventObjectSet
{
    my $self = shift;
    my $has = shift;
    Etk::etk_widget_has_event_object_set($self->{WIDGET}, $has);
}

sub HasEventObjectGet
{
    my $self = shift;
    return Etk::etk_widget_has_event_object_get($self->{WIDGET});
}

sub RepeatMouseEventsSet
{
    my $self = shift;
    my $repeat = shift;
    Etk::etk_widget_repeat_mouse_events_set($self->{WIDGET}, $repeat);
}

sub RepeatMouseEventsGet
{
    my $self = shift;
    return Etk::etk_widget_repeat_mouse_events_get($self->{WIDGET});
}

sub KeyEventPropagationStop
{
    Etk::etk_widget_key_event_propagation_stop();
}


sub SwallowWidget
{
    my $self = shift;
    my $part = shift;
    my $widget = shift;
    return Etk::etk_widget_swallow_widget($self->{WIDGET}, $part, $widget->{WIDGET});
}

sub UnswallowWidget
{
    my $self = shift;
    my $widget = shift;
    Etk::etk_widget_unswallow_widget($self->{WIDGET}, $widget->{WIDGET});
}

sub IsSwallowingWidget
{
    my $self = shift;
    my $widget = shift;
    return Etk::etk_widget_is_swallowing_widget($self->{WIDGET}, $widget->{WIDGET});
}

sub IsSwallowed
{
    my $self = shift;
    return Etk::etk_widget_is_swallowed($self->{WIDGET});
}

sub ThemeObjectMinSizeCalc
{
    my $self = shift;
    return Etk::etk_widget_theme_object_min_size_calc($self->{WIDGET});
}

sub ThemeObjectSignalEmit
{
    my $self = shift;
    my $signal = shift;
    Etk::etk_widget_theme_object_signal_emit($self->{WIDGET}, $signal);
}

sub ThemeObjectPartTextSet
{
    my $self = shift;
    my $part = shift;
    my $text = shift;
    Etk::etk_widget_theme_object_part_text_set($self->{WIDGET}, $part, $text);
}

# This is just a start.
sub AUTOLOAD
{
    our $AUTOLOAD;

    my $package;
    ($package = $AUTOLOAD) =~ s/.*:://;

    if ($package =~ /^Add(.*)/) 
    {
	shift;
	my $p = $1;
    	my $return;

    	eval("use Etk::$p");
	die("Cannot load package Etk::$p - $@") if $@;
    	eval("\$return = Etk::${p}->new(\@_);");
    	return $return;
    }
}

1;

__END__

