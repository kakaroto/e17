package Etk::Box;
use strict;
use vars qw(@ISA);
require Etk::Container;
@ISA = ("Etk::Container");

use AutoLoader;

sub new
{
    my $class = shift;
    my $self = $class->SUPER::new();
    bless($self, $class);
    return $self;
}

sub PackStart
{
    my $self = shift;
    my $child = shift;
    if (!$child && ref $self->{PARENT} && $self->{PARENT}->isa("Etk::Box")) {
	    $self->{PARENT}->PackStart($self, @_);
	    return $self;
    }
    my $expand = 1;
    $expand = shift if(@_ > 0);
    my $fill = 1;
    $fill = shift if(@_ > 0);
    my $padding = 0;
    $padding = shift if(@_ > 0);
    Etk::etk_box_pack_start($self->{WIDGET}, $child->{WIDGET}, $expand,
	$fill, $padding);
    return $self;
}

sub PackEnd
{
    my $self = shift;
    my $child = shift;
    my $expand = 1;
    $expand = shift if(@_ > 0);
    my $fill = 1;
    $fill = shift if(@_ > 0);
    my $padding = 0;
    $padding = shift if(@_ > 0);
    Etk::etk_box_pack_end($self->{WIDGET}, $child->{WIDGET}, $expand,
	$fill, $padding);
}

sub ChildPackingSet
{
    my $self = shift;
    my $child = shift;
    my $padding = shift;
    my $expand = shift;
    my $fill = shift;
    my $pack_end = shift;
    Etk::etk_box_child_packing_set($self->{WIDGET}, $child->{WIDGET}, $padding,
	$expand, $fill, $pack_end);
}

=item ChildPackingGet($child)

Get packing information about the child.
If $child is numerical then it is the nth child added, otherwise it's the widget itself.

Returns: ($padding, $expand, $fill, $pack_end) or undef if child is not found.

=cut

sub ChildPackingGet
{
    my $self = shift;
    my $child = shift;
    my $child_widget;
    if ($child + 0 eq $child) {
        $child_widget = $self->children()->[$child];
    } else {
	$child_widget = $child;
    }
    if ($child_widget->isa("Etk::Widget")) {
	    return Etk::etk_box_child_packing_get($self->{WIDGET}, $child_widget->{WIDGET});
    }
    return undef;
}

sub SpacingSet
{
    my $self = shift;
    my $spacing = shift;
    Etk::etk_box_spacing_set($self->{WIDGET}, $spacing);
}

sub SpacingGet
{
    my $self = shift;
    return Etk::etk_box_spacing_get($self->{WIDGET});
}

sub HomogenousSet
{
    my $self = shift;
    my $homogenous = shift;
    Etk::etk_box_homogenous_set($self->{WIDGET}, $homogenous);
}

sub HomogenousGet
{
    my $self = shift;
    return Etk::etk_box_homogenous_Get($self->{WIDGET});
}

sub children
{
    my $self = shift;
    return $self->{CHILDREN};
}

sub AUTOLOAD
{
    our $AUTOLOAD;

    my $package;
    ($package = $AUTOLOAD) =~ s/.*:://;

    if ($package =~ /^Add(.+)/) 
    {
	my $self = shift;
	my $p = $1;
    	my $return;

    	eval("use Etk::$p");
	die("Cannot load package Etk::$p - $@") if $@;
    	eval("\$return = Etk::${p}->new(\@_);");

	push @{$self->{CHILDREN}}, $return;
	$return->{PARENT} = $self;

    	return $return;
    }
}

1;

__END__

