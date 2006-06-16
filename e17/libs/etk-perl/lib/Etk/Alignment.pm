package Etk::Alignment;
use strict;
use vars qw(@ISA);
require Etk::Bin;
@ISA = ("Etk::Bin");
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    $self->{WIDGET} = Etk::etk_alignment_new(shift, shift, shift, shift);
    bless($self, $class);
    return $self;
}

sub Set
{
    my $self = shift;
    Etk::etk_alignment_set($self->{WIDGET}, shift, shift, shift, shift);
}

sub Get
{
    my $self = shift;
    # returns xalign, yalign, xscale, yscale
    return Etk::etk_alignment_get($self->{WIDGET});
}

1;

