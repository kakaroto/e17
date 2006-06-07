package Etk::Tree::Model;
use strict;
use vars qw(@ISA);
require Etk;

sub new
{
    my $class = shift;
    my $self = {};
    $self->{WIDGET} = unref;
    bless($self, $class);
    return $self;
}

sub Free
{
    my $self = shift;
    Etk::etk_tree_model_free($self->{WIDGET});
}

sub AlignmentSet
{
    my $self = shift;
    my $xalign = shift;
    my $yalign = shift;
    Etk::etk_tree_model_alignment_set($self->{WIDGET}, $xalign, $yalign);
}

sub AlignmentGet
{
    my $self = shift;
    Etk::etk_tree_model_alignment_get($self->{WIDGE}, shift, shift);
}

1;
