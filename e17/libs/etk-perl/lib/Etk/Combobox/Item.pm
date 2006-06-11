package Etk::Combobox::Item;
use strict;
use vars qw(@ISA);
require Etk::Widget;
@ISA = ("Etk::Widget");
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    bless($self, $class);
    return $self;    
}

sub DataSet
{
    my $self = shift;
    my $data = shift;
    Etk::etk_combobox_item_data_set($self->{WIDGET}, $data);
}

sub DataGet
{
    my $self = shift;
    return Etk::etk_combobox_item_data_get($self->{WIDGET});
}

1;
