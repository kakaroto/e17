package Etk::Button;
use strict;
use vars qw(@ISA);
require Etk::Bin;
@ISA = ("Etk::Bin");
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    if(@_ == 1)
    {
	# TODO: check if its a string or num and use label / stock respectively
	$self->{WIDGET} = Etk::etk_button_new_with_label(shift);
    }
    else
    {
	$self->{WIDGET} = Etk::etk_button_new();
    }
    bless($self, $class);
    return $self;
}

sub Press
{
    my $self = shift;
    Etk::etk_button_press($self->{WIDGET});
}

sub Release
{
    my $self = shift;
    Etk::etk_button_release($self->{WIDGET});
}

sub Click
{
    my $self = shift;
    Etk::etk_button_click($self->{WIDGET});
}

sub LabelSet
{
    my $self = shift;
    my $label = shift;
    Etk::etk_button_label_set($self->{WIDGET}, $label);
}

sub LabelGet
{
    my $self = shift;
    return Etk::etk_button_label_get($self->{WIDGET});
}

sub ImageSet
{
    my $self = shift;
    my $image = shift;
    Etk::etk_button_image_set($self->{WIDGET}, $image->{WIDGET});
}

sub ImageGet
{
    my $self = shift;
    return Etk::etk_button_image_get($self->{WIDGET});
}

sub SetFromStock
{
    my $self = shift;
    my $stock_id = shift;
    Etk::etk_button_set_from_stock($self->{WIDGET}, $stock_id);
}

sub AlignmentSet
{
    my $self = shift;
    my $alignment = shift;
    Etk::etk_button_alignment_set($self->{WIDGET}, $alignment);
}

sub AlignmentGet
{
    my $self = shift;
    # RETURNS
    # xalign
    # yalign
    return Etk::etk_button_alignment_get($self->{WIDGET});
}

1;
