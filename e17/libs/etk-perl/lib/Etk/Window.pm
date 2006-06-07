package Etk::Window;
use strict;
use vars qw(@ISA);
require Etk::Container;
@ISA = qw(Etk::Container);
sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    $self->{WIDGET} = Etk::etk_window_new();
    bless($self, $class);    
    if(@_ == 1)
    {
	my $title = shift;
	$self->TitleSet($title);
    }
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
    Etk::etk_window_title_set($self->{WIDGET}, $title);
}

1;
__END__

