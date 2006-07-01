package Etk::Theme;
use strict;
require Etk;

sub IconThemeGet
{
    return Etk::etk_theme_icon_theme_get();
}

sub IconThemeSet
{
    my $theme = shift;
    return Etk::etk_theme_icon_theme_set($theme);
}

sub DefaultIconThemeGet
{
    return Etk::etk_theme_default_icon_theme_get();
}

sub WidgetThemeGet
{
    return Etk::etk_theme_widget_theme_get();
}

sub WidgetThemeSet
{
    my $theme = shift;
    return Etk::etk_theme_widget_theme_set($theme);
}

sub WidgetDefaultThemeGet
{
    return Etk::etk_theme_default_widget_theme_get();
}

Etk::etk_theme_init();

1;
