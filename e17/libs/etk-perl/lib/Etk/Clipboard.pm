package Etk::Clipboard;
use strict;
require Etk;
sub TextRequest
{
    my $widget = shift;
    Etk::etk_clipboard_text_request($widget->{WIDGET});
}

sub TextSet
{
    my $widget = shift;
    my $text = shift;
    Etk::etk_clipboard_text_set($widget->{WIDGET}, $text, length($text));
    return $widget;
}

1;
