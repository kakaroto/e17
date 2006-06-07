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
    # confess "usage: \$object->SignalConnect(\"sig_name\", \\&callback)" unless (@_ == 2);
    Etk::etk_signal_connect(shift, $self->{WIDGET}, shift);
}

1;

