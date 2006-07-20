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
    return $self;
}

sub PageAppend
{
    my $self = shift;
    my $tab_label = shift;
    my $page_child = shift;
    Etk::etk_notebook_page_append($self->{WIDGET}, $tab_label, 
	$page_child->{WIDGET});
    return $self;
}

sub Insert
{
    my $self = shift;
    my $tab_label = shift;
    my $page_child = shift;
    my $position = shift;
    Etk::etk_notebook_page_insert($self->{WIDGET}, $tab_label, 
	$page_child->{WIDGET}, $position);
    return $self;
}

sub Remove
{
    my $self = shift;
    my $page_num = shift;
    Etk::etk_notebook_remove($self->{WIDGET}, $page_num);
    return $self;
}

sub NumPagesGet
{
    my $self = shift;
    return Etk::etk_notebook_num_pages_get($self->{WIDGET});
}

sub CurrentPageSet
{
    my $self = shift;
    my $page = shift;
    Etk::etk_notebook_current_page_set($self->{WIDGET}, $page);
    return $self;
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
    return $self;
}

sub PageNext
{
    my $self = shift;
    Etk::etk_notebook_page_next($self->{WIDGET});
    return $self;
}

sub PageTabLabelSet
{
    my $self = shift;
    my $page_num = shift;
    my $tab_label = shift;
    Etk::etk_notebook_page_tab_label_set($self->{WIDGET}, $page_num,
	$tab_label);
    return $self;
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
    return $self;
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
    return $self;
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
