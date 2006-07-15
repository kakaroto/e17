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
    my ($type, $mem) = split /=/, $self->{WIDGET};
    my $signal_name = shift;
    my $callback = shift;
    my $data = undef;
    $data = shift if (@_ > 0);
    Etk::etk_signal_connect($signal_name, 
    	bless($self->{WIDGET}, "Etk_WidgetPtr"), $callback, $data);
    bless($self->{WIDGET}, $type);
}

sub SignalConnectAfter
{
    my $self = shift;
    my ($type, $mem) = split /=/, $self->{WIDGET};
    my $signal_name = shift;
    my $callback = shift;
    my $data = undef;
    $data = shift if (@_ > 0);
    Etk::etk_signal_connect_after($signal_name, 
    	bless($self->{WIDGET}, "Etk_WidgetPtr"), $callback, $data);
    bless($self->{WIDGET}, $type);
}

sub SignalConnectSwapped
{
    my $self = shift;
    my ($type, $mem) = split /=/, $self->{WIDGET};
    my $signal_name = shift;
    my $callback = shift;
    my $data = undef;
    $data = shift if (@_ > 0);
    Etk::etk_signal_connect_swapped($signal_name, 
    	bless($self->{WIDGET}, "Etk_WidgetPtr"), $callback, $data);
    bless($self->{WIDGET}, $type);
}

sub SignalConnectFull
{
    my $self = shift;
    my ($type, $mem) = split /=/, $self->{WIDGET};
    my $signal_name = shift;
    my $callback = shift;
    my $data = shift;
    my $swapped = shift;
    my $after = shift;
    Etk::etk_signal_connect_full($signal_name, 
    	bless($self->{WIDGET}, "Etk_WidgetPtr"), $callback, $data, $swapped, $after);
    bless($self->{WIDGET}, $type);
}

sub SignalDisconnect
{
    my $self = shift;
    my ($type, $mem) = split /=/, $self->{WIDGET};
    my $signal_name = shift;
    my $callback = shift;
    Etk::etk_signal_disconnect($signal_name, 
    	bless($self->{WIDGET}, "Etk_WidgetPtr"), $callback);
    bless($self->{WIDGET}, $type);
}

sub NotificationCallbackAdd
{
    my $self = shift;
    my $property_name = shift;
    my $callback = shift;
    my $data = shift;
    Etk::etk_object_notification_callback_add($self->{WIDGET}, $property_name, $callback, $data);
}

sub NotificationCallbackRemove
{
    my $self = shift;
    my $property_name = shift;
    Etk::etk_object_notification_callback_remove($self->{WIDGET}, $property_name);
}

sub Notify
{
    my $self = shift;
    my $property_name = shift;
    Etk::etk_object_notify($self->{WIDGET}, $property_name);
}

sub DataSet
{
    my $self = shift;
    my $key = shift;
    my $data = shift;
    Etk::etk_object_data_set($self->{WIDGET}, $key, $data);
}

sub DataGet
{
    my $self = shift;
    my $key = shift;
    return undef unless $key;
    return Etk::etk_object_data_get($self->{WIDGET}, $key);
}

1;

