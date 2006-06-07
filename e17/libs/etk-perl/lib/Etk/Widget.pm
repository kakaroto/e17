package Etk::Widget;
use strict;
use vars qw(@ISA);
require Etk::Object;
@ISA = ("Etk::Object");
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    $self->{WIDGET} = undef;
    bless($self, $class);
    return $self;
}

sub SizeRequestSet
{
    my $self = shift;
    my $width = shift;
    my $height = shift;
    Etk::etk_widget_size_request_set($self->{WIDGET}, $width, $height);
}

sub ShowAll
{
    my $self = shift;
    Etk::etk_widget_show_all($self->{WIDGET}) if ($self->{WIDGET});
}

1;
