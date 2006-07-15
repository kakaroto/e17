package Etk::Image;
use strict;
use vars qw(@ISA);
require Etk::Widget;
@ISA = ("Etk::Widget");
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    if(@_ == 1)
    {
	$self->{WIDGET} = Etk::etk_image_new_from_file(shift);
    }
    else
    {
	$self->{WIDGET} = Etk::etk_image_new();
    }
    bless($self, $class);
    return $self;
}

sub new_no_create
{
    my $class = shift;
    my $self = $class->SUPER::new();
    $self->{WIDGET} = undef;
    bless($self, $class);
    return $self;
}

sub new_from_edje
{
    my $class = shift;
    my $self = $class->SUPER::new();
    my $edje_filename = shift;
    my $edje_group = shift;
    $self->{WIDGET} = Etk::etk_image_new_from_edje($edje_filename, 
	$edje_group);
    bless($self, $class);
    return $self;
}

sub new_from_stock
{
    my $class = shift;
    my $self = $class->SUPER::new();
    my $stock_id = shift;
    my $stock_size = shift;
    $self->{WIDGET} = Etk::etk_image_new_from_stock($stock_id, $stock_size);
    bless($self, $class);
    return $self;
}

sub SetFromFile
{
    my $self = shift;
    my $filename = shift;
    Etk::etk_image_set_from_file($self->{WIDGET}, $filename);
}

sub FileGet
{
    my $self = shift;
    return Etk::etk_image_file_get($self->{WIDGET});
}

sub SetFromEdje
{
    my $self = shift;
    my $edje_filename = shift;
    my $edje_group = shift;
    Etk::etk_image_set_from_edje($self->{WIDGET}, $edje_filename, $edje_group);
}

sub EdjeGet
{
    my $self = shift;
    # returns
    # edje_filename
    # edje_group
    return Etk::etk_image_edje_get($self->{WIDGET});
}

sub SetFromStock
{
    my $self = shift;
    my $stock_id = shift;
    my $stock_size = shift;
    Etk::etk_image_set_from_stock($self->{WIDGET}, $stock_id, $stock_size);
}

sub StockGet
{
    my $self = shift;
    Etk::etk_image_stock_get($self->{WIDGET}, shift, shift);
}

sub CopyFrom
{
    my $self = shift;
    my $src_image = shift;
    Etk::etk_image_copy($self->{WIDGET}, $src_image->{WIDGET});
}

sub SizeGet
{
    my $self = shift;
    return Etk::etk_image_size_get($self->{WIDGET});
}

sub KeepAspectSet
{
    my $self = shift;
    my $keep_aspect = shift;
    Etk::etk_image_keep_aspect_set($self->{WIDGET}, $keep_aspect);
}

sub KeepAspectGet
{
    my $self = shift;
    return Etk::etk_image_keep_aspect_get($self->{WIDGET});
}

1;
