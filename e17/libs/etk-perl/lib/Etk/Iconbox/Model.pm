package Etk::Iconbox::Model;
use strict;
require Etk;  
sub new
{
    my $class = shift;
    my $self = {};
    my $model = shift;
    $self->{WIDGET} = Etk::etk_iconbox_model_new($model);
    bless($self, $class);
    return $self;
}

sub new_nocreate
{
    my $class = shift;
    my $self = {};
    $self->{WIDGET} = undef;
    bless($self, $class);
    return $self;
}

sub GeometrySet
{
    my $self = shift;
    my $width = shift;
    my $height = shift;
    my $xpadding = shift;
    my $ypadding = shift;
    Etk::etk_iconbox_model_geometry_set($self->{WIDGET}, $width, $height, 
	$xpadding, $ypadding);
}

sub GeometryGet
{
    my $self = shift;
    # RETURNS:
    # width
    # height
    # xpadding
    # ypadding
    return Etk::etk_iconbox_model_geometry_get($self->{WIDGET});    
}

sub IconGeometrySet
{
    my $self = shift;
    my $x = shift;
    my $y = shift;
    my $width = shift;
    my $height = shift;
    my $fill = shift;
    my $keep_aspect_ratio = shift;
    Etk::etk_iconbox_model_icon_geometry_set($self->{WIDGET}, $x, $y, $width,
	$height, $fill, $keep_aspect_ratio);
}

sub IconGeometryGet
{
    my $self = shift;
    # RETURNS
    # x 
    # y
    # width
    # height
    # fill
    # keep_aspect_ratio
    return Etk::etk_iconbox_model_icon_geometry_get($self->{WIDGET});
}

sub LabelGeometrySet
{
    my $self = shift;
    my $x = shift;
    my $y = shift;
    my $width = shift;
    my $height = shift;
    my $xalign = shift;
    my $yalign = shift;
    Etk::etk_iconbox_model_label_geometry_set($self->{WIDGET}, $x, $y, $width,
	$height, $xalign, $yalign);
}

1;
