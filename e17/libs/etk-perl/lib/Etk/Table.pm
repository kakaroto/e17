package Etk::Table;
use strict;
use vars qw(@ISA);
require Etk::Container;
@ISA = ("Etk::Container");
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    my $num_cols = shift;
    my $num_rows = shift;
    my $homogenous = shift;
    $self->{WIDGET} = Etk::etk_table_new($num_cols, $num_rows, $homogenous);
    bless($self, $class);
    return $self;
}

sub TypeGet
{
    return Etk::etk_table_type_get();
}

sub CellClear
{
    my $self = shift;
    my $col = shift;
    my $row = shift;
    Etk::etk_table_cell_clear($self->{WIDGET}, $col, $row);
    return $self;
}

sub Resize
{
    my $self = shift;
    my $num_cols = shift;
    my $num_rows = shift;
    Etk::etk_table_resize($self->{WIDGET}, $num_cols, $num_rows);
    return $self;
}

sub Attach
{
    my $self = shift;
    my $child = shift;
    my $left_attach = shift;
    my $right_attach = shift;
    my $top_attach = shift;
    my $bottom_attach = shift;
    my $x_padding = shift;
    my $y_padding = shift;
    my $fill_policy = shift;
    Etk::etk_table_attach($self->{WIDGET}, $child->{WIDGET}, $left_attach,
	$right_attach, $top_attach, $bottom_attach, $x_padding, $y_padding,
	$fill_policy);
    return $self;
}

sub AttachDefaults
{
    my $self = shift;
    my $child = shift;
    my $left_attach = shift;
    my $right_attach = shift;
    my $top_attach = shift;
    my $bottom_attach = shift;
    Etk::etk_table_attach_defaults($self->{WIDGET}, $child->{WIDGET}, 
	$left_attach, $right_attach, $top_attach, $bottom_attach);
    return $self;
}   

sub HomogenousSet
{
    my $self = shift;
    my $homogenous = shift;
    Etk::etk_table_homogeneous_set($self->{WIDGET}, $homogenous);
    return $self;
}

sub HomogenousGet
{
    my $self = shift;
    return Etk::etk_table_homogeneous_get($self->{WIDGET});
}

1;
