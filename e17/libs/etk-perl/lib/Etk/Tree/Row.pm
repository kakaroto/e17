package Etk::Tree::Row;
use strict;
use vars qw(@ISA);
require Etk;

sub new
{
    my $class = shift;
    my $self = {};
    $self->{WIDGET} = undef;
    bless($self, $class);
    return $self;
}

sub Delete
{
    my $self = shift;
    Etk::etk_tree_row_delete($self->{WIDGET});
}

sub FirstChildGet
{
    my $self = shift;
    my $row = Etk::Tree::Row->new();
    $row->{WIDGET} = Etk::etk_tree_row_first_child_get($self->{WIDGET});
    return $row;
}

sub LastChildGet
{
    my $self = shift;
    my $row = Etk::Tree::Row->new();
    $row->{WIDGET} = Etk::etk_tree_row_last_child_get($self->{WIDGET});
    return $row;
}

sub PrevRowGet
{
    my $self = shift;
    my $row = Etk::Tree::Row->new();
    my $walking_through_hierarchy = shift;
    my $include_collapsed_children = shift;    
    $row->{WIDGET} = Etk::etk_tree_row_prev_row_get($self->{WIDGET},
    $walking_through_hierarchy, $include_collapsed_children);
    return $row;
}

sub NextRowGet
{
    my $self = shift;
    my $walking_through_hierarchy = shift;
    my $include_collapsed_children = shift;
    my $row = Etk::Tree::Row->new();
    $row->{WIDGET} = Etk::etk_tree_row_next_row_get($self->{WIDGET},
	$walking_through_hierarchy, $include_collapsed_children);    
    return $row;
}

sub FieldIntSet
{
    my $self = shift;
    my $col = shift;
    my $int = shift;
    Etk::etk_tree_row_field_int_set($self->{WIDGET}, $col->{WIDGET}, $int);
    return $self;
}

sub FieldTextSet
{
    my $self = shift;
    my $col = shift;
    my $text = shift;
    Etk::etk_tree_row_field_text_set($self->{WIDGET}, $col->{WIDGET}, $text);
    return $self;
}

sub FieldDoubleSet
{
    my $self = shift;
    my $col = shift;
    my $double = shift;
    Etk::etk_tree_row_field_double_set($self->{WIDGET}, $col->{WIDGET}, $double);
    return $self;
}

sub FieldImageFileSet
{
    my $self = shift;
    my $col = shift;
    my $icon = shift;
    my $text = shift;
    Etk::etk_tree_row_field_image_file_set($self->{WIDGET}, $col->{WIDGET},
	$icon);
    return $self;
}

sub FieldImageEdjeSet
{
    my $self = shift;
    my $col = shift;
    my $edje = shift;
    my $group = shift;
    Etk::etk_tree_row_field_image_edje_set($self->{WIDGET}, $col->{WIDGET},
	$edje, $group);
    return $self;
}

sub FieldIconFileTextSet
{
    my $self = shift;
    my $col = shift;
    my $icon = shift;
    my $text = shift;
    Etk::etk_tree_row_field_icon_file_text_set($self->{WIDGET}, $col->{WIDGET},
	$icon, $text);
    return $self;
}

sub FieldIconEdjeTextSet
{
    my $self = shift;
    my $col = shift;
    my $edje = shift;
    my $group = shift;
    my $text = shift;
    Etk::etk_tree_row_field_icon_edje_text_set($self->{WIDGET}, $col->{WIDGET},
	$edje, $group, $text);
    return $self;
}

sub FieldCheckboxSet
{
    my $self = shift;
    my $col = shift;
    my $checked = shift;
    Etk::etk_tree_row_field_checkbox_set($self->{WIDGET}, $col->{WIDGET},
	$checked);
    return $self;
}

sub FieldProgressBarSet
{
    my $self = shift;
    my $col = shift;
    my $fraction = shift;
    my $text = shift;
    Etk::etk_tree_row_field_progress_bar_set($self->{WIDGET}, $col->{WIDGET},
	$fraction, $text);
    return $self;
}

sub FieldIntGet
{
    my $self = shift;
    my $col = shift;
    # RETURNS
    # int
    return Etk::etk_tree_row_field_int_get($self->{WIDGET}, $col->{WIDGET});
}

sub FieldTextGet
{
    my $self = shift;
    my $col = shift;
    # RETURNS
    # text
    return Etk::etk_tree_row_field_text_get($self->{WIDGET}, $col->{WIDGET});
}

sub FieldDoubleGet
{
    my $self = shift;
    my $col = shift;
    # RETURNS
    # double
    return Etk::etk_tree_row_field_double_get($self->{WIDGET}, $col->{WIDGET});
}

sub FieldImageFileGet
{
    my $self = shift;
    my $col = shift;
    # RETURNS
    # image
    return Etk::etk_tree_row_field_image_file_get($self->{WIDGET}, 
	$col->{WIDGET});
}

sub FieldImageEdjeGet
{
    my $self = shift;
    my $col = shift;
    # RETURN
    # edje
    # group
    return Etk::etk_tree_row_field_image_edje_get($self->{WIDGET},
	$col->{WIDGET});
}

sub FieldIconFileTextGet
{
    my $self = shift;
    my $col = shift;
    # RETURNS
    # icon
    # text
    return Etk::etk_tree_row_field_icon_file_text_get($self->{WIDGET},
	$col->{WIDGET});
}

sub FieldIconEdjeTextGet
{
    my $self = shift;
    my $col = shift;
    # RETURN
    # edje
    # group
    # text
    return Etk::etk_tree_row_field_icon_edje_text_get($self->{WIDGET},
	$col->{WIDGET});
}

sub FieldCheckboxGet
{
    my $self = shift;
    my $col = shift;
    # RETURN
    # checked
    return Etk::etk_tree_row_field_checkbox_get($self->{WIDGET},
	$col->{WIDGET});
}

sub FieldProgressBarGet
{
    my $self = shift;
    my $col = shift;
    # RETURN
    # fraction
    # text
    return Etk::etk_tree_row_field_progress_bar_get($self->{WIDGET},
	$col->{WIDGET});
}

sub FieldsSet
{
    my $self = shift;
    # TODO
}

sub FieldsGet
{
    my $self = shift;
    # TODO
}

sub DataSet
{
    my $self = shift;
    my $data = shift;
    
    Etk::etk_tree_row_data_set($self->{WIDGET}, $data);
    return $self;
}

sub DataGet
{
    my $self = shift;
    return Etk::etk_tree_row_data_get($self->{WIDGET});
}

sub ScrollTo
{
    my $self = shift;
    my $center_the_row = shift;
    Etk::etk_tree_row_scroll_to($self->{WIDGET}, $center_the_row);
    return $self;
}

sub Select
{
    my $self = shift;
    Etk::etk_tree_row_select($self->{WIDGET});
    return $self;
}

sub Unselect
{
    my $self = shift;
    Etk::etk_tree_row_unselect($self->{WIDGET});
    return $self;
}

sub Expand
{
    my $self = shift;
    Etk::etk_tree_row_expand($self->{WIDGET});
    return $self;
}

sub Collapse
{
    my $self = shift;
    Etk::etk_tree_row_collapse($self->{WIDGET});
    return $self;
}

1;
