package Etk::Selection;
use strict;
require Etk;
sub TextRequest
{
    my $widget = shift;
    Etk::etk_selection_text_request($widget->{WIDGET});
    return $widget;
}

sub TextSet
{
    my $widget = shift;
    my $text = shift;
    Etk::etk_selection_text_set($widget->{WIDGET}, $text, length($text));
    return $widget;
}

1;
