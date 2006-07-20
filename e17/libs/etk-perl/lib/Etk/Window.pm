package Etk::Window;
use strict;
use vars qw(@ISA);
require Etk::Container;
@ISA = qw(Etk::Container);
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    $self->{WIDGET} = Etk::etk_window_new();
    bless($self, $class);    
    if(@_ == 1)
    {
	my $title = shift;
	$self->TitleSet($title);
    }
    return $self;
}

sub new_nocreate
{
    my $class = shift;
    my $self = $class->SUPER::new();
    bless($self, $class);
    return $self;
}

sub TitleSet
{
    my $self = shift;
    my $title = shift;
    Etk::etk_window_title_set($self->{WIDGET}, $title);
    return $self;
}

sub TitleGet
{
    my $self = shift;
    return Etk::etk_window_title_get($self->{WIDGET});
}

sub WMClassSet
{
    my $self = shift;
    my $window_name = shift;
    my $window_class = shift;
    Etk::etk_window_wmclass_set($self->{WIDGET}, $window_name, $window_class);
    return $self;
}

sub Move
{
    my $self = shift;
    Etk::etk_window_move($self->{WIDGET}, shift, shift);
    return $self;
}

sub Resize
{
    my $self = shift;
    Etk::etk_window_resize($self->{WIDGET}, shift, shift);
    return $self;
}

sub GeometryGet
{
    my $self = shift;
    return Etk::etk_window_geometry_get($self->{WIDGET});
}

sub CenterOnWindow
{
    my $self = shift;
    my $window = shift;
    Etk::etk_window_center_on_window($self->{WIDGET}, $window->{WIDGET});
    return $self;
}

sub MoveToMouse
{
    my $self = shift;
    Etk::etk_window_move_to_mouse($self->{WIDGET});
    return $self;
}

sub Iconify
{
    my $self = shift;
    Etk::etk_window_iconify($self->{WIDGET});
    return $self;
}

sub Deiconify
{
    my $self = shift;
    Etk::etk_window_deiconify($self->{WIDGET});
    return $self;
}

sub isIconified
{
    my $self = shift;
    return Etk::etk_window_is_iconified($self->{WIDGET});
}

sub Maximize
{
    my $self = shift;
    Etk::etk_window_maximize($self->{WIDGET});
    return $self;
}

sub Unmaximize
{
    my $self = shift;
    Etk::etk_window_unmaximize($self->{WIDGET});
    return $self;
}

sub isMaximized
{
    my $self = shift;
    return Etk::etk_window_is_maximized($self->{WIDGET});
}

sub Fullscreen
{
    my $self = shift;
    Etk::etk_window_fullscreen($self->{WIDGET});
    return $self;
}

sub Unfullscreen
{
    my $self = shift;
    Etk::etk_window_unfullscreen($self->{WIDGET});
    return $self;
}

sub isFullscreen
{
    my $self = shift;
    return Etk::etk_window_is_fullscreen($self->{WIDGET});
}

sub Stick
{
    my $self = shift;
    Etk::etk_window_stick($self->{WIDGET});
    return $self;
}

sub Unstick
{
    my $self = shift;
    Etk::etk_window_unstick($self->{WIDGET});
    return $self;
}

sub isSticky
{
    my $self = shift;
    return Etk::etk_window_is_sticky($self->{WIDGET});
}

sub Focus
{
    my $self = shift;
    Etk::etk_window_focus($self->{WIDGET});
    return $self;
}

sub Unfocus
{
    my $self = shift;
    Etk::etk_window_unfocus($self->{WIDGET});
    return $self;
}

sub isFocused
{
    my $self = shift;
    return Etk::etk_window_is_focused($self->{WIDGET});
}

sub DecoratedSet
{
    my $self = shift;
    Etk::etk_window_decorated_set($self->{WIDGET}, shift);
    return $self;
}

sub DecoratedGet
{
    my $self = shift;
    return Etk::etk_window_decorated_get($self->{WIDGET});
}

sub ShapedSet
{
    my $self = shift;
    Etk::etk_window_shaped_set($self->{WIDGET}, shift);
    return $self;
}

sub ShapedGet
{
    my $self = shift;
    return Etk::etk_window_shaped_get($self->{WIDGET});
}

sub SkipTaskbarHintSet
{
    my $self = shift;
    Etk::etk_window_skip_taskbar_hint_set($self->{WIDGET}, shift);
    return $self;
}

sub SkipTaskbarHintGet
{
    my $self = shift;
    return Etk::etk_window_skip_taskbar_hint_get($self->{WIDGET});
}

sub SkipPagerHintSet
{
    my $self = shift;
    Etk::etk_window_skip_pager_hint_set($self->{WIDGET}, shift);
    return $self;
}

sub SkipPagerHintGet
{
    my $self = shift;
    return Etk::etk_window_skip_pager_hint_get($self->{WIDGET});
}

sub DndAwareSet
{
    my $self = shift;
    Etk::etk_window_dnd_aware_set($self->{WIDGET}, shift);
    return $self;
}

sub HideOnDelete
{
	# TODO
	# what is this?
}

1;

