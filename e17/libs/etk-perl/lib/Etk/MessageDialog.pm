package Etk::MessageDialog;
use strict;
use vars qw(@ISA);
require Etk::Dialog;
@ISA = ("Etk::Dialog");

use constant
{
      None = 0,
      Warning => 1,
      Question => 2,
      Error => 3,
      
      ButtonsNone => 0,
      ButtonsOk => 1,
      ButtonsClose => 2,
      ButtonsCancel => 3,
      ButtonsYesNo => 4,
      ButtonsOkCancel => 5
};

sub new
{
    my $class = shift;
    my $self = $class->SUPER::new_nocreate();
    my $message_type = shift;
    my $buttons = shift;
    my $text = shift;
    $self->{WIDGET} = Etk::etk_message_dialog_new($message_type, buttons,
	$test);
    bless($self, $class);
    return $self;
}

sub TextSet
{
    my $self = shift;
    my $text = shift;
    Etk::etk_message_dialog_text_set($self->{WIDGET}, $text);    
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
}

sub ButtonsGet
{
    my $self = shift;
    return Etk::etk_message_dialog_buttons_set($self->{WIDGET});
}

1;
