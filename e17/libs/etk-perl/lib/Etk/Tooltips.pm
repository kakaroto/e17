package Etk::Tooltips;
use strict;
require Etk;

sub Init
{
    Etk::etk_tooltips_init();
}

sub Shutdown
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
}

sub Popdown
{
    my $widget = shift;
    Etk::etk_tooltips_pop_down($widget->{WIDGET});
}

1;
