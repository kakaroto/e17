package Etk::Object;
use strict;
require Etk;
sub new
{
    my $class = shift;
    my $self = {};
    bless($self, $class);
    return $self;
}

sub SignalConnect
{
    my $self = shift;
    my $signal_name = shift;
    my $callback = shift;
    my $data = shift;
    Etk::etk_signal_connect($signal_name, $self, $callback, $data);
    return $self;
}

sub SignalConnectAfter
{
    my $self = shift;
    my $signal_name = shift;
    my $callback = shift;
    my $data = shift;
    Etk::etk_signal_connect_after($signal_name, $self, $callback, $data);
    return $self;
}

sub SignalConnectSwapped
{
    my $self = shift;
    my $signal_name = shift;
    my $callback = shift;
    my $data = shift;
    Etk::etk_signal_connect_swapped($signal_name, $self, $callback, $data);
    return $self;
}

sub SignalConnectFull
{
    my $self = shift;
    my $signal_name = shift;
    my $callback = shift;
    my $data = shift;
    my $swapped = shift;
    my $after = shift;
    Etk::etk_signal_connect_full($signal_name, $self, $callback, $data, $swapped, $after);
    return $self;
}

sub SignalDisconnect
{
    my $self = shift;
    my $signal_name = shift;
    my $callback = shift;
    Etk::etk_signal_disconnect($signal_name, $self, $callback);
    return $self;
}

sub NotificationCallbackAdd
{
    my $self = shift;
    my $property_name = shift;
    my $callback = shift;
    my $data = shift;
    Etk::etk_object_notification_callback_add($self->{WIDGET}, $property_name, $callback, $data);
    return $self;
}

sub NotificationCallbackRemove
{
    my $self = shift;
    my $property_name = shift;
    Etk::etk_object_notification_callback_remove($self->{WIDGET}, $property_name);
    return $self;
}

sub Notify
{
    my $self = shift;
    my $property_name = shift;
    Etk::etk_object_notify($self->{WIDGET}, $property_name);
    return $self;
}

sub DataSet
{
    my $self = shift;
    my $key = shift;
    my $data = shift;
    Etk::etk_object_data_set($self->{WIDGET}, $key, $data);
    return $self;
}

sub DataGet
{
    my $self = shift;
    my $key = shift;
    return undef unless $key;
    return Etk::etk_object_data_get($self->{WIDGET}, $key);
}

1;

