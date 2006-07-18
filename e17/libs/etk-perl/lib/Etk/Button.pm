package Etk::Button;
use strict;
use vars qw(@ISA);
require Etk::Bin;
@ISA = ("Etk::Bin");
use Etk::Image;

sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    if(@_ == 1)
    {
	my $thing = shift;
	$self->{WIDGET} = $thing + 0 eq $thing ?
	    Etk::etk_button_new_from_stock($thing) :
	    Etk::etk_button_new_with_label($thing); 
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
	
=item ImageSet($image)

Set the image of a button.
If $image is a string, a new Etk::Image object is created.

=cut 

sub ImageSet
{
    my $self = shift;
    my $im = shift;
    my $image;
    if (ref $im && $im->isa("Etk::Image")) {
	    $image = $im;
    } else {
	    $image = Etk::Image->new($im);
    }
    Etk::etk_button_image_set($self->{WIDGET}, $image->{WIDGET});
    return $self;
}

sub ImageGet
{
    my $self = shift;
    my $image = Etk::Image->new_no_create();
    $image->{WIDGET} = Etk::etk_button_image_get($self->{WIDGET});
    return $image;
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
