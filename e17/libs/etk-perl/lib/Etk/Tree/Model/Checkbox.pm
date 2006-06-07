package Etk::Tree::Model::Checkbox;
use strict;
use vars qw(@ISA);
require Etk;

sub new
{
    my $class = shift;
    my $self = {};
    my $tree = shift;
    $self->{WIDGET} = Etk::etk_tree_model_checkbox_new($tree->{WIDGET});
    bless($self, $class);
    return $self;
}

1;
