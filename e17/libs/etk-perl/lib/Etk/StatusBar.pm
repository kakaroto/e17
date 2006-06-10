package Etk::StatusBar;
use strict;
use vars qw(@ISA);
require Etk::Widget;
@ISA = ("Etk::Widget");
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    $self->{WIDGET} = Etk::etk_statusbar_new();
    bless($self, $class);
    return $self;
}

sub ContextIdGet
{
   my $self = shift;
   my $context = shift;

   return Etk::etk_statusbar_context_id_get($self->{WIDGET}, $context);
}


sub Push
{
    my $self = shift;
    my $message = shift;
    my $context_id = shift;

    return Etk::etk_statusbar_push($self->{WIDGET}, $message, $context_id);

}

sub Pop
{
    my $self = shift;
    my $context_id = shift;
    
    return Etk::etk_statusbar_pop($self->{WIDGET}, $context_id);
}

sub Remove
{
    my $self = shift;
    my $message_id = shift;

    return Etk::etk_statusbar_remove($self->{WIDGET}, $message_id);

}

sub HasResizeGripSet
{
    my $self = shift;
    my $has_resize_grip = shift;
    
    return Etk::etk_statusbar_has_resize_grip_set($self->{WIDGET}, $has_resize_grip);

}

sub HasResizeGripGet
{
    my $self = shift;
    
    return Etk::etk_statusbar_has_resize_grip_get($self->{WIDGET});

}


1;
