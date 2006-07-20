package Etk::Menu::Item::Radio;
use strict;
use vars qw(@ISA);
require Etk::Menu::Item;
@ISA = ("Etk::Menu::Item");
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    my $widget;
    
    if(@_ >= 1) {
        my $data = shift;
        if (ref $data) {
            $self->{WIDGET} = Etk::etk_menu_item_radio_new_from_widget($data->{WIDGET});
            $widget = $data;
        } else {
            if (@_) {
                my $data2 = shift;
                $self->{WIDGET} = Etk::etk_menu_item_radio_new_with_label_from_widget(
                    $data, $data2->{WIDGET});
                $widget = $data2;
            } else {
                $self->{WIDGET} = Etk::etk_menu_item_radio_new_with_label($data);
            }
        }
    }
    bless($self, $class);
    if ($widget) {
        push @{$widget->{GROUP}}, $self;
        $self->{GROUP} = $widget->{GROUP};
    } else {
        push @{$self->{GROUP}}, $self;
    }
    return $self;
}

sub GroupGet
{
    my $self = shift;
    return @{$self->{GROUP}};
}

1;
