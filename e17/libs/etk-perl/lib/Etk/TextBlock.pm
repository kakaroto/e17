package Etk::TextBlock;
use strict;
use vars qw(@ISA);
require Etk::Object;
@ISA = ("Etk::Object");

sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    $self->{WIDGET} = Etk::etk_textblock_new();
    bless($self, $class);
    return $self;
}

sub new_nocreate
{
    my $class = shift;
    my $self = $class->SUPER::new();
    bless($self, $class);
    return $self;
}

sub TextSet
{
    my $self = shift;
    my $text = shift;
    my $markup = shift;
    Etk::etk_textblock_text_set($self->{WIDGET}, $text, $markup);
}

sub TextGet
{
    my $self = shift;
    my $markup = shift;
    return Etk::etk_textblock_text_get($self->{WIDGET}, $markup);
}

1;
