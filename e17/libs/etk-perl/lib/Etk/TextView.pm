package Etk::TextView;
use strict;
use vars qw(@ISA);
require Etk::Widget;
require Etk::TextBlock;
@ISA = ("Etk::Widget");

sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    $self->{WIDGET} = Etk::etk_text_view_new();
    bless($self, $class);
    return $self;
}

sub TextBlockGet
{
    my $self = shift;
    my $textblock = Etk::TextBlock->new_nocreate();
    $textblock->{WIDGET} = Etk::etk_text_view_textblock_get($self->{WIDGET});
    return $textblock;
}

1;
