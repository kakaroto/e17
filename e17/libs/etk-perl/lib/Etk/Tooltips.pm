package Etk::Tooltips;
use strict;
require Etk;


sub unimport
{
    Etk::etk_tooltips_shutdown();
}

sub Enable
{
    Etk::etk_tooltips_enable();
}

sub Disable
{
    Etk::etk_tooltips_disable();
}

sub TipSet
{
    my $widget = shift;
    my $text = shift;
    Etk::etk_tooltips_tip_set($widget->{WIDGET}, $text);
    return $widget;
}

sub TipGet
{
    my $widget = shift;
    return Etk::etk_tooltip_tip_get($widget->{WIDGET});
}

sub Popup
{
    my $widget = shift;
    Etk::etk_tooltips_pop_up($widget->{WIDGET});
    return $widget;
}

sub Popdown
{
    my $widget = shift;
    Etk::etk_tooltips_pop_down($widget->{WIDGET});
    return $widget;
}

sub TipVisible
{
    return Etk::etk_tooltips_tip_visible();
}

Etk::etk_tooltips_init();

1;
