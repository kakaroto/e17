package Etk::Combobox;
use strict;
use vars qw(@ISA);
require Etk::Widget;
@ISA = ("Etk::Widget");

use Etk::Combobox::Item;

sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    $self->{WIDGET} = Etk::etk_combobox_new();
    bless($self, $class);
    return $self;
}

sub new_default
{
    my $class = shift;
    my $self = $class->SUPER::new();
    $self->{WIDGET} = Etk::etk_combobox_new_default();
    bless($self, $class);
    return $self;
}

sub HeightSet
{
    my $self = shift;
    my $height = shift;
    Etk::etk_combobox_height_set($self->{WIDGET}, $height);
    return $self;
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
    my $hfill = shift;    
    my $vfill = shift;
    my $xalign = shift;
    my $yalign = shift;
    Etk::etk_combobox_column_add($self->{WIDGET}, $col_type, $size, $expand,
	$hfill, $vfill, $xalign, $yalign);
    return $self;
}

sub Build
{
    my $self = shift;
    Etk::etk_combobox_build($self->{WIDGET});
    return $self;
}

sub ActiveItemSet
{
    my $self = shift;
    my $item = shift;
    Etk::etk_combobox_active_item_set($self->{WIDGET}, $item->{WIDGET});
    return $self;
}

sub ActiveItemGet
{
    my $self = shift;
    my $item = Etk::Combobox::Item->new();
    $item->{WIDGET} = Etk::etk_combobox_active_item_get($self->{WIDGET});
    return $item;
}

sub NthItemGet
{
    my $self = shift;
    my $n = shift;
    my $item = Etk::Combobox::Item->new();
    $item->{WIDGET} = Etk::etk_combobox_nth_item_get($self->{WIDGET}, $n);
    return $item;
}

sub ItemPrepend
{
    my $self = shift;
    my $item = Etk::Combobox::Item->new();
    my @args;
    
    for my $arg (@_)
    {
	if($arg->isa("Etk::Widget")) {
	    push @args, $arg->{WIDGET};
	} else {
	    push @args, $arg;
	}
    }    
    $item->{WIDGET} = Etk::etk_combobox_prepend_complex($self->{WIDGET},
	@args);
    return $item;
}

sub ItemAppend
{
    my $self = shift;
    my $item = Etk::Combobox::Item->new();
    my @args;
    
    for my $arg (@_)
    {
	if($arg->isa("Etk::Widget")) {
	    push @args, $arg->{WIDGET};
	} else {
	    push @args, $arg;
	}
    }
    
    $item->{WIDGET} = Etk::etk_combobox_item_append_complex($self->{WIDGET},
	@args);
    return $item;
}

sub ItemPrependRelative
{
    my $self = shift;
    my $relative = shift;
    my $item = Etk::Combobox::Item->new();
    my @args;
    
    for my $arg (@_)
    {
	if($arg->isa("Etk::Widget")) {
	    push @args, $arg->{WIDGET};
	} else {
	    push @args, $arg;
	}
    }
        
    $item->{WIDGET} = Etk::etk_combobox_prepend_relative_complex(
	$self->{WIDGET}, $relative->{WIDGET}, @args);
    return $item;
}

sub ItemAppendRelative
{
    my $self = shift;
    my $relative = shift;
    my $item = Etk::Combobox::Item->new();
    my @args;
    
    for my $arg (@_)
    {
	if($arg->isa("Etk::Widget")) {
	    push @args, $arg->{WIDGET};
	} else {
	    push @args, $arg;
	}
    }
        
    $item->{WIDGET} = Etk::etk_combobox_append_relative_complex(
	$self->{WIDGET}, $relative->{WIDGET}, @args);
    return $item;
}

sub ItemRemove
{
    my $self = shift;
    my $item = shift;
    Etk::etk_combobox_item_remove($self->{WIDGET}, $item->{WIDGET});
    return $self;
}

sub Clear
{
    my $self = shift;
    Etk::etk_combobox_clear($self->{WIDGET});
    return $self;
}

1;
