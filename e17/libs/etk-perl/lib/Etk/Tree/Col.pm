package Etk::Tree::Col;
use strict;
use vars qw(@ISA);
require Etk::Object;
@ISA = ("Etk::Object");

sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    my $tree = shift;
    my $title = shift;
    my $model = shift;
    my $width = shift;
    $self->{WIDGET} = Etk::etk_tree_col_new($tree->{WIDGET}, $title,
	$model->{WIDGET}, $width);
    bless($self, $class);
    return $self;
}

sub new_nocreate
{
    my $class = shift;
    my $self = $class->SUPER::new();
    bless($self, $class);
    return $self;
}

sub TitleSet
{
   my $self = shift;
   my $title = shift;
   Etk::etk_tree_col_title_set($self->{WIDGET}, $title);
}

sub TitleGet
{
    my $self = shift;
    return Etk::etk_tree_col_title_get($self->{WIDGET});
}

sub WidthSet
{
    my $self = shift;
    my $width = shift;
    Etk::etk_tree_col_width_set($self->{WIDGET}, $width);
}

sub WidthGet
{
    my $self = shift;
    return Etk::etk_tree_col_width_get($self->{WIDGET});
}

sub MinWidthSet
{
    my $self = shift;
    my $min_width = shift;
    Etk::etk_tree_col_min_width_set($self->{WIDGET}, $min_width);
}

sub MinWidthGet
{
    my $self = shift;
    return Etk::etk_tree_col_min_width_get($self->{WIDGET});
}

sub ResizableSet
{
    my $self = shift;
    my $resizable = shift;
    Etk::etk_tree_col_resizable_set($self->{WIDGET}, $resizable);
}

sub ResizableGet
{
    my $self = shift;
    return Etk::etk_tree_col_resizable_get($self->{WIDGET});
}

sub ExpandSet
{
    my $self = shift;
    my $expand = shift;
    Etk::etk_tree_col_expand_set($self->{WIDGET}, $expand);
}

sub ExpandGet
{
    my $self = shift;
    return Etk::etk_tree_col_expand_get($self->{WIDGET});
}

sub VisibleSet
{
    my $self = shift;
    my $visible = shift;
    Etk::etk_tree_col_visible_set($self->{WIDGET}, $visible);
}

sub VisibleGet
{
    my $self = shift;
    return Etk::etk_tree_col_visible_get($self->{WIDGET});
}

sub Reorder
{
    my $self = shift;
    my $new_place = shift;
    Etk::etk_tree_col_redorder($self->{WIDGET}, $new_place);
}

sub SortFuncSet
{
    my $self = shift;
    my $callback = shift;
    my $data = undef;
    $data = shift if (@_ > 0);
    # shift in callback and data
    Etk::etk_tree_col_sort_func_set($self->{WIDGET}, $callback, $data);
}


1;
