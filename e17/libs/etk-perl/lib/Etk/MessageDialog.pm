package Etk::MessageDialog;
use strict;
use vars qw(@ISA);
require Etk::Dialog;
@ISA = ("Etk::Dialog");

sub new
{
    my $class = shift;
    my $self = $class->SUPER::new_nocreate();
    my $message_type = shift;
    my $buttons = shift;
    my $text = shift;
    $self->{WIDGET} = Etk::etk_message_dialog_new($message_type, $buttons,
	$text);
    bless($self, $class);
    return $self;
}

sub TextSet
{
    my $self = shift;
    my $text = shift;
    Etk::etk_message_dialog_text_set($self->{WIDGET}, $text);    
    return $self;
}

sub TextGet
{
    my $self = shift;
    return Etk::etk_message_dialog_text_get($self->{WIDGET});
}

sub MessageTypeSet
{
    my $self = shift;
    my $type = shift;
    Etk::etk_message_dialog_message_type_set($self->{WIDGET}, $type);
    return $self;
}

sub MessageTypeGet
{
    my $self = shift;
    return Etk::etk_message_dialog_message_type_get($self->{WIDGET});
}

sub ButtonsSet
{
    my $self = shift;
    my $buttons = shift;
    Etk::etk_message_dialog_buttons_set($self->{WIDGET}, $buttons);
    return $self;
}

sub ButtonsGet
{
    my $self = shift;
    return Etk::etk_message_dialog_buttons_set($self->{WIDGET});
}

1;
