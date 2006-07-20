package Etk::Frame;
use strict;
use vars qw(@ISA);
require Etk::Bin;
@ISA = ("Etk::Bin");
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    my $label = shift;
    $self->{WIDGET} = Etk::etk_frame_new($label);
    bless($self, $class);    
    return $self;
}

sub LabelSet
{
    my $self = shift;
    my $label = shift;
    Etk::etk_frame_label_set($self->{WIDGET}, $label);
    return $self;
}

sub LabelGet
{
    my $self = shift;
    return Etk::etk_frame_label_get($self->{WIDGET});
}

1;
