package Etk::Combobox;
use strict;
use vars qw(@ISA);
require Etk::Widget;
@ISA = ("Etk::Widget");
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    $self->{WIDGET} = Etk::etk_combobox_new();
    bless($self, $class);
    return $self;
}

sub HeightSet
{
    my $self = shift;
    my $height = shift;
    Etk::etk_combobox_height_set($self->{WIDGET}, $height);
}

sub HeightGet
{
    my $self = shift;
    return Etk::etk_combobox_height_get($self->{WIDGET});
}

sub ColumnAdd
{
    my $self = shift;
    my $col_type = shift;
    my $size = shift;
    my $expand = shift;
    my $vfill = shift;
    my $xalign = shift;
    my $yalign = shift;
    Etk::etk_combobox_column_add($self->{WIDGET}, $col_type, $size, $expand,
	$vfill, $xalign, $yalign);
}

sub Build
{
    my $self = shift;
    Etk::etk_combobox_build($self->{WIDGET});
}

sub ActiveItemSet
{
    my $self = shift;
    my $item = shift;
    Etk::etk_combobox_active_item_set($self->{WIDGET}, $item->{WIDGET});
}

sub ActiveItemGet
{
    my $self = shift;
    return Etk::etk_combobox_active_item_get($self->{WIDGET});
}

sub NthItemGet
{
    my $self = shift;
    my $item = shift;
    return Etk::etk_combobox_nth_item_get($self->{WIDGET}, $item);
}

sub ItemPrepend
{
    # TODO: make this work with as many items as we want
    my $self = shift;
    my $item = shift;
    
    Etk::etk_combobox_prepend($self->{WIDGET}, $item->{WIDGET});
}

sub ItemAppend
{
    # TODO: make this work with as many items as we want
    my $self = shift;
    my $item = shift;
    
    Etk::etk_combobox_append($self->{WIDGET}, $item->{WIDGET});
}

sub ItemPrependRelative
{
    # TODO: make this work with as many items as we want
    my $self = shift;
    my $relative = shift;
    my $item = shift;
    
    Etk::etk_combobox_prepend_relative($self->{WIDGET}, $relative->{WIDGET}, $item->{WIDGET});
}

sub ItemAppendRelative
{
    # TODO: make this work with as many items as we want
    my $self = shift;
    my $relative = shift;
    my $item = shift;
    
    Etk::etk_combobox_append_relative($self->{WIDGET}, $relative->{WIDGET}, $item->{WIDGET});
}

sub ItemRemove
{
    my $self = shift;
    my $item = shift;
    Etk::etk_combobox_item_remove($self->{WIDGET}, $item->{WIDGET});
}

sub Clear
{
    my $self = shift;
    Etk::etk_combobox_clear($self->{WIDGET});
}

sub ItemDataSet
{
    # TODO
}

1;
