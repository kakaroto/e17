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
    my ($type, $mem) = split /=/, "$self->{WIDGET}";
    my $signal_name = shift;
    my $callback = shift;
    my $data = undef;
    $data = shift if (@_ > 0);
    Etk::etk_signal_connect($signal_name, 
	bless($self->{WIDGET}, "Etk_WidgetPtr"), $callback, $data);
    bless($self->{WIDGET}, $type);
}

sub DataGet
{
    my $self = shift;
    my $key = shift;
    return undef unless $key;
    return Etk::etk_object_data_get($self->{WIDGET}, $key);
}

1;

