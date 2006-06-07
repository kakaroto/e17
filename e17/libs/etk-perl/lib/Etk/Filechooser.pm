package Etk::Filechooser;
use strict;
use vars qw(@ISA);
require Etk::Widget;
@ISA = ("Etk::Widget");
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    $self->{WIDGET} = Etk::etk_filechooser_widget_new();
    bless($self, $class);
    return $self;
}

sub SelectMultipleSet
{
    my $self = shift;
    my $select_multiple = shift;
    Etk::etk_filechooser_widget_select_multiple_set($self->{WIDGET},
	$select_multiple);
}

sub SelectMultipleGet
{
    my $self = shift;
    return Etk::etk_filechooser_widget_select_multiple_get($self->{WIDGET});
}

sub ShowHiddenSet
{
    my $self = shift;
    my $show_hidden = shift;
    Etk::etk_filechooser_widget_show_hidden_set($self->{WIDGET}, $show_hidden);
}

sub ShowHiddenGet
{
    my $self = shift;
    return Etk::etk_filechooser_widget_show_hidden_get($self->{WIDGET});
}

sub CurrentFolderSet
{
    my $self = shift;
    my $folder = shift;
    Etk::etk_filechooser_widget_current_folder_set($self->{WIDGET}, $folder);
}

sub CurrentFolderGet
{
    my $self = shift;
    return Etk::etk_filechooser_widget_current_folder_get($self->{WIDGET});
}

sub SelectedFileGet
{
    my $self = shift;
    return Etk::etk_filechooser_widget_selected_file_get($self->{WIDGET});
}

sub SelectedFilesGet
{
    # TODO: pending todo to convert lists to perl lists
}

1;
