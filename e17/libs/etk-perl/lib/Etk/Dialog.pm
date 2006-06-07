package Etk::Dialog;
use strict;
use vars qw(@ISA);
require Etk::Window;
@ISA = ("Etk::Window");
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new_nocreate();
    $self->{WIDGET} = Etk::etk_dialog_new();
    bless($self, $class);
    return $self;
}

sub new_nocreate
{
    my $class = shift;
    my $self = $class->SUPER::new_nocreate();
    bless($self, $class);
    return $self;
}

sub PackInMainArea
{
    my $self = shift;
    my $widget = shift;
    my $expand = shift;
    my $fill = shift;
    my $padding = shift;
    my $pack_at_end = shift;
    Etk::etk_dialog_pack_in_main_area($self->{WIDGET}, $widget->{WIDGET},
	$expand, $fill, $padding, $pack_at_end);
}

sub PackWidgetInActionArea
{
    my $self = shift;
    my $widget = shift;
    my $expand = shift;
    my $fill = shift;
    my $padding = shift;
    my $pack_at_end = shift;
    Etk::etk_dialog_pack_widget_in_action_area($self->{WIDGET}, 
	$widget->{WIDGET}, $expand, $fill, $padding, $pack_at_end);
}

sub PackButtonInActionArea
{
    my $self = shift;
    my $button = shift;
    my $response_id = shift;
    my $expand = shift;
    my $fill = shift;
    my $padding = shift;
    my $pack_at_end = shift;
    Etk::etk_dialog_pack_button_in_action_area($self->{WIDGET}, 
	$button->{WIDGET}, $response_id, $expand, $fill, $padding, $pack_at_end);
}

sub ButtonAdd
{
    my $self = shift;
    my $label = shift;
    my $response_id = shift;
    Etk::etk_dialog_button_add($self->{WIDGET}, $label, $response_id);
}

sub ButtonAddFromStock
{
    my $self = shift;
    my $stock_id = shift;
    my $response_id = shift;
    Etk::etk_dialog_button_add_from_stock($self->{WIDGET}, $stock_id, $response_id);
}

sub HasSeparatorSet
{
    my $self = shift;
    my $has_separator = shift;
    Etk::etk_dialog_has_separator_set($self->{WIDGET}, $has_separator);
}

sub HasSeparatorGet
{
    my $self = shift;
    return Etk::etk_dialog_has_separator_get($self->{WIDGET});
}

1;
