package Etk::Menu::Shell;
use strict;
use vars qw(@ISA);
require Etk::Widget;
@ISA = ("Etk::Widget");
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    bless($self, $class);
    return $self;
}

sub Prepend
{
    my $self = shift;
    my $item = shift;
    Etk::etk_menu_shell_prepend($self->{WIDGET}, $item->{WIDGET});
    return $self;
}

sub Append
{
    my $self = shift;
    my $item = shift;
    Etk::etk_menu_shell_append($self->{WIDGET}, $item->{WIDGET});
    return $self;
}    

sub PrependRelative
{
    my $self = shift;
    my $item = shift;
    my $relative = shift;
    Etk::etk_menu_shell_prepend_relative($self->{WIDGET}, $item->{WIDGET}, 
	$relative->{WIDGET});
    return $self;
}

sub AppendRelative
{
    my $self = shift;
    my $item = shift;
    my $relative = shift;
    Etk::etk_menu_shell_append_relative($self->{WIDGET}, $item->{WIDGET}, 
	$relative->{WIDGET});
    return $self;
}

sub Insert
{
    my $self = shift;
    my $item = shift;
    my $position = shift;
    Etk::etk_menu_shell_insert($self->{WIDGET}, $item->{WIDGET}, $position);
    return $self;
}

sub Remove
{
    my $self = shift;
    my $item = shift;
    Etk::etk_menu_shell_remove($self->{WIDGET}, $item->{WIDGET});
    return $self;
}

sub ItemsGet
{
    my $self = shift;
    return map {
	my $widget = Etk::Widget->new();
	$widget->{WIDGET} = $_;
	$_ = $widget;
    } Etk::etk_menu_shell_items_get($self->{WIDGET});
}

1;
