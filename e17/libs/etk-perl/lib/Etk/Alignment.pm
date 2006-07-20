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

=item Set($xalign, $yalign, $xscale, $yscale)

Set alignment info. 

=cut

sub Set
{
    my $self = shift;
    Etk::etk_alignment_set($self->{WIDGET}, shift, shift, shift, shift);
    return $self;
}

=item Get()

Returns a list containing alignment info.
xalign, yalign, xscale, yscale

=cut

sub Get
{
    my $self = shift;
    return Etk::etk_alignment_get($self->{WIDGET});
}

1;

