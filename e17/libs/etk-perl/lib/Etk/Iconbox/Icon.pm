package Etk::Iconbox::Icon;
use strict;
require Etk;
sub new
{
    my $class = shift;
    my $self = {};
    $self->{WIDGET} = undef;
    bless($self, $class);
    return $self;
}

sub FileSet
{
    my $self = shift;
    my $filename = shift;
    my $edje_group = shift;
    Etk::etk_iconbox_icon_file_set($self->{WIDGET}, $filename, $edje_group);
}

sub FileGet
{
    my $self = shift;
    # RETURNS
    # filename
    # edje_group
    return Etk::etk_iconbox_icon_file_get($self->{WIDGET});
}

sub LabelSet
{
    my $self = shift;
    my $label = shift;
    Etk::etk_iconbox_icon_label_set($self->{WIDGET}, $label);
}

sub LabelGet
{
    my $self = shift;
    return Etk::etk_iconbox_icon_label_get($self->{WIDGET});
}

sub DataSet
{
    my $self = shift;
    my $data = shift;
    Etk::etk_iconbox_icon_data_set($self->{WIDGET}, $data);
}

sub DataGet
{
    my $self = shift;
    return Etk::etk_iconbox_icon_data_get($self->{WIDGET});
}

sub Select
{
    my $self = shift;
    Etk::etk_iconbox_icon_select($self->{WIDGET});
}

sub Unselect
{
    my $self = shift;
    Etk::etk_iconbox_icon_unselect($self->{WIDGET});
}

sub IsSelected
{
    my $self = shift;
    return Etk::etk_iconbox_icon_is_selected($self->{WIDGET});
}

1;
