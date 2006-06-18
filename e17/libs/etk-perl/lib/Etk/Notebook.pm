package Etk::Notebook;
use strict;
use vars qw(@ISA);
require Etk::Container;
@ISA = ("Etk::Container");

sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    $self->{WIDGET} = Etk::etk_notebook_new();
    bless($self, $class);
    return $self;
}

sub PagePrepend
{
    my $self = shift;
    my $tab_label = shift;
    my $page_child = shift;
    Etk::etk_notebook_page_prepend($self->{WIDGET}, $tab_label,
	$page_child->{WIDGET});
}

sub PageAppend
{
    my $self = shift;
    my $tab_label = shift;
    my $page_child = shift;
    Etk::etk_notebook_page_append($self->{WIDGET}, $tab_label, 
	$page_child->{WIDGET});
}

sub Insert
{
    my $self = shift;
    my $tab_label = shift;
    my $page_child = shift;
    my $position = shift;
    Etk::etk_notebook_page_insert($self->{WIDGET}, $tab_label, 
	$page_child->{WIDGET}, $position);
}

sub Remove
{
    my $self = shift;
    my $page_num = shift;
    Etk::etk_notebook_remove($self->{WIDGET}, $page_num);
}

sub NumPagesGet
{
    my $self = shift;
    return Etk::etk_notebook_num_pages_get($self->{WIDGET});
}

sub CurrentPageGet
{
    my $self = shift;
    return Etk::etk_notebook_current_page_get($self->{WIDGET});
}

sub PageIndexGet
{
    my $self = shift;
    my $child = shift;
    return Etk::etk_notebook_page_index_get($self->{WIDGET}, $child->{WIDGET});
}

sub PagePrev
{
    my $self = shift;
    Etk::etk_notebook_page_prev($self->{WIDGET});
}

sub PageNext
{
    my $self = shift;
    Etk::etk_notebook_page_next($self->{WIDGET});
}

sub PageTabLabelSet
{
    my $self = shift;
    my $page_num = shift;
    my $tab_label = shift;
    Etk::etk_notebook_page_tab_label_set($self->{WIDGET}, $page_num,
	$tab_label);
}

sub PageTabLabelGet
{
    my $self = shift;
    my $page_num = shift;
    return Etk::etk_notebook_page_tab_label_set($self->{WIDGET}, $page_num);
}

sub PageTabWidgetSet
{
    my $self = shift;
    my $page_num = shift;
    my $tab_widget = shift;
    Etk::etk_notebook_page_tab_label_set($self->{WIDGET}, $page_num,
	$tab_widget->{WIDGET});
}

sub PageTabWidgetGet
{
    my $self = shift;
    my $page_num = shift;
    my $tab_widget = Etk::Widget->new();
    $tab_widget->{WIDGET} = Etk::etk_notebook_page_tab_label_set(
	$self->{WIDGET}, $page_num);
    return $tab_widget;
}

sub PageChildSet
{
    my $self = shift;
    my $page_num = shift;
    my $child = shift;
    Etk::etk_notebook_page_child_set($self->{WIDGET}, $page_num,
	$child->{WIDET});
}

sub PageChildGet
{
    my $self = shift;
    my $page_num = shift;
    my $child = Etk::Widget->new();
    $child->{WIDET} = Etk::etk_notebook_page_child_get($self->{WIDGET}, 
	$page_num);
    return $child;
}

1;
