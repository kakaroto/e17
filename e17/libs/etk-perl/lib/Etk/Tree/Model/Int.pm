package Etk::Tree::Model::Int;
use strict;
use vars qw(@ISA);
require Etk;

sub new
{
    my $class = shift;
    my $self = {};
    my $tree = shift;
    $self->{WIDGET} = Etk::etk_tree_model_int_new($tree->{WIDGET});
    bless($self, $class);
    return $self;
}

1;
