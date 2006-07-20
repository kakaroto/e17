package Etk::Entry;
use strict;
use vars qw(@ISA);
require Etk::Widget;
@ISA = ("Etk::Widget");
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    $self->{WIDGET} = Etk::etk_entry_new();
    bless($self, $class);
    return $self;
}

sub TextSet
{
    my $self = shift;
    my $text = shift;
    Etk::etk_entry_text_set($self->{WIDGET}, $text);
    return $self;
}

sub TextGet
{
    my $self = shift;
    return Etk::etk_entry_text_get($self->{WIDGET});
}

sub PasswordSet
{
    my $self = shift;
    my $on = shift;
    return Etk::etk_entry_password_set($self->{WIDGET}, $on);
    return $self;
}

sub PasswordGet
{
    my $self = shift;
    return Etk::etk_entry_password_get($self->{WIDGET});
}

1;
