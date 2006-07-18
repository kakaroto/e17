package Etk::Tree;
use strict;
use vars qw(@ISA);
require Etk::Widget;
@ISA = ("Etk::Widget");

use Etk::Tree::Col;
use Etk::Tree::Row;
use Etk::Tree::Model;

use Etk::Tree::Model::Checkbox;
use Etk::Tree::Model::Double;
use Etk::Tree::Model::IconText;
use Etk::Tree::Model::Image;
use Etk::Tree::Model::Int;
use Etk::Tree::Model::ProgressBar;
use Etk::Tree::Model::Text;

use constant
{
   ModeList => 0,
   ModeTree => 1
};

sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    $self->{WIDGET} = Etk::etk_tree_new();
    bless($self, $class);
    return $self;
}

sub TypeGet
{
    return Etk::etk_tree_type_get();
}

sub ModeSet
{
   my $self = shift;
   my $mode = shift;
   Etk::etk_tree_mode_set($self->{WIDGET}, $mode);
}

sub ModeGet
{
   my $self = shift;
   return Etk::etk_tree_mode_get($self->{WIDGET});
}

sub NumColsGet
{
   my $self = shift;
   return Etk::etk_tree_num_cols_get($self->{WIDGET});
}

sub NthColGet
{
   my $self = shift;
   my $nth = shift;
   my $col = Etk::Tree::Col->new_nocreate();
   $col->{WIDGET} = Etk::etk_tree_nth_col_get($self->{WIDGET}, $nth);
   return $col;
}

sub HeadersVisibleSet
{
   my $self = shift;
   my $headers_visible = shift;
   Etk::etk_tree_headers_visible_set($self->{WIDGET}, $headers_visible);
}

sub HeadersVisibleGet
{
   my $self = shift;
   return Etk::etk_tree_headers_visible_get($self->{WIDGET});
}

sub Build
{
   my $self = shift;
   Etk::etk_tree_build($self->{WIDGET});
}

sub Freeze
{
   my $self = shift;
   Etk::etk_tree_freeze($self->{WIDGET});
}

sub Thaw
{
   my $self = shift;
   Etk::etk_tree_thaw($self->{WIDGET});
}

sub RowHeightSet
{
   my $self = shift;
   my $height = shift;
   Etk::etk_tree_row_height_set($self->{WIDGET}, $height);
}

sub RowHeightGet
{
   my $self = shift;
   return Etk::etk_tree_row_height_get($self->{WIDGET});
}

sub MultipleSelectSet
{
   my $self = shift;
   my $multiple_select = shift;
   Etk::etk_tree_multiple_select_set($self->{WIDGET}, $multiple_select);
}

sub MultipleSelectGet
{
   my $self = shift;
   return Etk::etk_tree_multiple_select_get($self->{WIDGET});
}

sub SelectAll
{
   my $self = shift;
   Etk::etk_tree_select_all($self->{WIDGET});
}

sub UnselectAll
{
   my $self = shift;
   Etk::etk_tree_unselect_all($self->{WIDGET});
}

# NOTE: since we cant transparently pass a variable number of arguments
# from perl to xs to c, we have to do things differently. This function
# will append an empty row, then we use perl model functions to set the
# data of the row's columns.
sub Append
{
    my $self = shift;
    my $row = Etk::Tree::Row->new();
    $row->{WIDGET} = Etk::etk_tree_append($self->{WIDGET});
    return $row;
}

# NOTE: this isnt working because of variable argument lists
sub Append2
{
    my $self = shift;
    my $row = Etk::Tree::Row->new();
    my @args;
    for my $arg (@_)
    {
	if($arg->isa("Etk::Object"))
	{
	    push @args, $arg->{WIDGET};
	}
	else
	{
	    push @args, $arg;
	}
    }
    $row->{WIDGET} = Etk::etk_tree_append($self->{WIDGET}, @args, undef);
    return $row;
}

sub AppendToRow
{
   my $self = shift;
   # TODO: figure out how to implement this
}

sub Clear
{
   my $self = shift;
   Etk::etk_tree_clear($self->{WIDGET});
}

sub Sort
{
    my $self = shift;
    my $callback = shift;
    my $asc = shift;
    my $col = shift;
    my $data = undef;
    $data = shift if (@_ > 0);
    Etk::etk_tree_sort($self->{WIDGET}, $callback, $asc, $col->{WIDGET}, 
	$data);
}

sub FirstRowGet
{
   my $self = shift;
   my $row = Etk::Tree::Row->new();
   $row->{WIDGET} = Etk::etk_tree_first_row_get($self->{WIDGET});
   return $row;
}

sub LastRowGet
{
   my $self = shift;
   my $row = Etk::Tree::Row->new();
   $row->{WIDGET} = Etk::etk_tree_last_row_get($self->{WIDGET});
   return $row;
}

sub SelectedRowGet
{
   my $self = shift;
   my $row = Etk::Tree::Row->new();
   $row->{WIDGET} = etk_tree_selected_row_get($self->{WIDGET});
   return $row;
}

sub SelectedRowsGet
{
   my $self = shift;
   # TODO: pending list implementation
}

sub AddCol
{
    my $self = shift;
    my ($title, $model, $width) = @_;

    my $model_widget;
    if ($model eq "Text") { 
	    $model_widget = Etk::Tree::Model::Text->new($self);
    } elsif ($model eq "ProgressBar") {
	    $model_widget = Etk::Tree::Model::ProgressBar->new($self);
    } # etc...

    my $widget = Etk::Tree::Col->new($self, $title, $model_widget, $width);
    $widget->{MODEL} = $model;
    
    push @{$self->{COLS}}, $widget;
    return $widget;

}

sub AddCols
{
    my $self = shift;
    my @cols = @_;
    foreach (@cols) {
	    $self->AddCol(@$_);
    }
    return $self;
}

sub AddRow
{
    my $self = shift;
    my @data = @_;
    my @cols = @{$self->{COLS}};
    my $row = $self->Append();
    foreach my $col (@cols) {
	if ($col->{MODEL} eq "Text") {
		my $text = shift @data;
		$row->FieldTextSet($col, $text);
	} elsif ($col->{MODEL} eq "ProgressBar") {
		my $prog = shift @data;
		$row->FieldProgressBarSet($col, $prog->[0], $prog->[1]);
	}
    }
    return $row;
}
   
sub AddRows
{
    my $self = shift;
    my @rows = @_;
    foreach (@rows) {
	    $self->AddRow(@$_);
    }
    return $self;
}
   
1;
