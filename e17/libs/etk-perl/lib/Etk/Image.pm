package Etk::Image;
use strict;
use vars qw(@ISA);
require Etk::Widget;
@ISA = ("Etk::Widget");
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    if(@_ == 2) {
	my ($arg1, $arg2) = @_;
	$self->{WIDGET} = -e $arg1 ? Etk::etk_image_new_from_edje($arg1, $arg2):
	    Etk::etk_image_new_from_stock($arg1, $arg2);
    } elsif (@_ == 1) {
	$self->{WIDGET} = Etk::etk_image_new_from_file(shift);
    } else {
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

sub SetFromFile
{
    my $self = shift;
    my $filename = shift;
    Etk::etk_image_set_from_file($self->{WIDGET}, $filename);
    return $self;
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
    return $self;
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
    return $self;
}

sub StockGet
{
    my $self = shift;
    # returns stock_id, stock_size
    return Etk::etk_image_stock_get($self->{WIDGET});
}

sub CopyFrom
{
    my $self = shift;
    my $src_image = shift;
    Etk::etk_image_copy($self->{WIDGET}, $src_image->{WIDGET});
    return $self;
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
    return $self;
}

sub KeepAspectGet
{
    my $self = shift;
    return Etk::etk_image_keep_aspect_get($self->{WIDGET});
}

1;
