package Etk::Tree::Model::IconText;
use strict;
use vars qw(@ISA);
require Etk;

sub new
{
    my $class = shift;
    my $self = {};
    my $tree = shift;
    my $icon_type = shift;
    $self->{WIDGET} = Etk::etk_tree_model_icon_text_new($tree->{WIDGET},
	$icon_type);
    bless($self, $class);
    return $self;
}

sub IconWidthSet
{
    my $self = shift;
    my $icon_width = shift;
    Etk::etk_tree_model_icon_text_icon_width_set($self->{WIDGET}, $icon_width);
    return $self;
}

sub IconWidthGet
{
    my $self = shift;
    return Etk::etk_tree_model_icon_text_icon_width_set($self->{WIDGET});
}

1;
