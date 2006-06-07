package Etk::Label;
use strict;
use vars qw(@ISA);
require Etk::Widget;
@ISA = ("Etk::Widget");
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    my $label = shift;
    $self->{WIDGET} = Etk::etk_label_new($label);
    bless($self, $class);
    return $self;
}

sub Set
{
    my $self = shift;
    my $text = shift;
    Etk::etk_label_set($self->{WIDGET}, $text);
}

sub Get
{
    my $self = shift;
    return Etk::etk_label_get($self->{WIDGET});
}

sub AlignmentSet
{
    my $self = shift;
    my $xalign = shift;
    my $yalign = shift;
    Etk::etk_label_alignment_set($self->{WIDGET}, $xalign, $yalign);
}

sub AlignmentGet
{
    my $self = shift;
    Etk::etk_label_alignment_get($self->{WIDGET}, shift, shift);
}

1;
