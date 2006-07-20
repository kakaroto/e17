package Etk::Tree::Model::Image;
use strict;
use vars qw(@ISA);
require Etk;

sub new
{
    my $class = shift;
    my $self = {};
    my $tree = shift;
    my $image_type = shift;
    $self->{WIDGET} = Etk::etk_tree_model_image_new($tree->{WIDGET},
	$image_type);
    bless($self, $class);
    return $self;
}

1;
