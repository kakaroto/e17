package Etk::Timer;
use strict;
require Etk;
sub new
{
    my $class = shift;
    my $self = {};
    my $interval = shift;
    $self->{WIDGET} = Etk::etkpl_timer_add($interval, shift);
    bless($self, $class);
    return $self;
}

sub Delete
{
    my $self = shift;
    Etk::etkpl_timer_del($self->{WIDGET});
}

1;
