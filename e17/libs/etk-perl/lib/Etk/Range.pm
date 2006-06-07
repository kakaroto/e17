package Etk::Range;
use strict;
use vars qw(@ISA);
require Etk::Widget;
@ISA = ("Etk::Widget");
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    my $label = shift;
    bless($self, $class);
    return $self;
}

sub ValueSet
{
    my $self = shift;
    my $value = shift;
    Etk::etk_range_value_set($self->{WIDGET}, $value);
}

sub ValueGet
{
    my $self = shift;
    return Etk::etk_range_value_get($self->{WIDGET});
}

sub RangeSet
{
    my $self = shift;
    my $lower = shift;
    my $upper = shift;
    Etk::etk_range_range_set($self->{WIDGET}, $lower, $upper);
}

sub RangeGet
{
    my $self = shift;
    Etk::etk_range_range_get($self->{WIDGET}, shift, shift);
}

sub IncrementsSet
{
    my $self = shift;
    my $step = shift;
    my $page = shift;
    Etk::etk_range_increments_set($self->{WIDGET}, $step, $page);
}

sub IncrementstGet
{
    my $self = shift;
    Etk::etk_range_increments_get($self->{WIDGET}, shift, shift);
}

sub PageSizeSet
{
    my $self = shift;
    my $page_size = shift;
    Etk::etk_range_page_size_set($self->{WIDGET}, $page_size);
}

sub PageSizeGet
{
    my $self = shift;
    return Etk::etk_range_page_size_get($self->{WIDGET});
}

1;
