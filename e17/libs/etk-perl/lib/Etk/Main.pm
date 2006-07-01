package Etk::Main;
use strict;
use vars qw(@ISA);
require Etk;

sub Run
{
    Etk::etk_main();
}

sub Quit
{
    Etk::etk_main_quit();
}

sub ToplevelWidgetAdd
{
    my $widget = shift;
    Etk::etk_main_toplevel_widget_add($widget->{WIDGET});
}

sub ToplevelWidgetRemove
{
    my $widget = shift;
    Etk::etk_main_toplevel_widget_remove($widget->{WIDGET});
}

sub ToplevelWidgetsGet
{
    return map {
	my $widget = Etk::ToplevelWidget->new();
	$widget->{WIDGET} = $_;
	$_ = $widget;
    } Etk::etk_main_toplevel_widgets_get();
}

1;
