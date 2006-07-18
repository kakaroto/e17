package Etk::Iconbox;
use strict;
use vars qw(@ISA);
require Etk::Widget;
@ISA = ("Etk::Widget");

use Etk::Iconbox::Icon;
use Etk::Iconbox::Model;

sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    $self->{WIDGET} = Etk::etk_iconbox_new();
    bless($self, $class);
    return $self;
}

sub CurrentModelSet
{
    my $self = shift;
    my $model = shift;
    Etk::etk_iconbox_current_model_set($self->{WIDGET}, $model->{WIDGET});
}

sub CurrentModelGet
{
    my $self = shift;
    my $model = Etk::Iconbox::Model->new_nocreate();
    $model->{WIDGET} = Etk::etk_combobox_current_model_get($self->{WIDGET});
    return $model;
}

sub Freeze
{
    my $self = shift;
    Etk::etk_iconbox_freeze($self->{WIDGET});
}

sub Thaw
{
    my $self = shift;
    Etk::etk_iconbox_thaw($self->{WIDGET});
}

sub Append
{
    my $self = shift;
    my $filename = shift;
    my $edje_group = shift;
    my $label = shift;
    my $icon = Etk::Iconbox::Icon->new();
    $icon->{WIDGET} =  Etk::etk_iconbox_append($self->{WIDGET}, $filename,
	$edje_group, $label);
    return $icon;
}

sub Clear
{
    my $self = shift;
    Etk::etk_iconbox_clear($self->{WIDGET});
}

sub IconGetAtXY
{
    my $self = shift;
    my $x = shift;
    my $y = shift;
    my $over_cell = shift;
    my $over_icon = shift;
    my $over_label = shift;
    my $icon = Etk::Iconbox::Icon->new();
    $icon->{WIDGET} =  Etk::etk_iconbox_icon_get_at_xy($self->{WIDGET}, $x, $y,
	$over_cell, $over_icon, $over_label);
    return undef if($icon->{WIDGET} == undef);
    return $icon;
}

sub SelectAll
{
    my $self = shift;
    Etk::etk_iconbox_select_all($self->{WIDGET});
}

sub UnselectAll
{
    my $self = shift;
    Etk::etk_iconbox_unselect_all($self->{WIDGET});
}

1;
