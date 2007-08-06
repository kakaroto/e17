package Etk::Simple;

no warnings 'redefine';
use strict;
use Etk;

package Etk::Button;

my $bnew = \&new;
*new = sub {
	my $class = shift;
	my $arg = shift;
	return $bnew->(__PACKAGE__) unless defined $arg;
	return NewFromStock($arg)
		if $arg =~ /^\d+$/;
	return NewWithLabel($arg);
};

package Etk::CheckButton;

my $cbnew = \&new;
*new = sub {
	my $class = shift;
	my $arg = shift;
	return NewWithLabel($arg) if $arg;
	return $cbnew->(__PACKAGE__);
};

package Etk::Image;

my $inew = \&new;
*new = sub {
	my $class = shift;
	my $arg1 = shift;
	my $arg2 = shift;
	
	if (defined $arg1) {
		if ($arg1 =~ /^\d+$/) {
			if ($arg2 !~ /^\d+$/) {
				warn "no StockSize defined, using: SizeSmall";
				$arg2 = 0;
			}
			return NewFromStock($arg1, $arg2);
		} else {
			if (defined $arg2) {
				if ($arg1 =~ /\.edj$/) {
					return NewFromEdje($arg1, $arg2);
				} else {
					return NewFromFile($arg1, $arg2); 
				}
			}
		}
	}
			
	return $inew->(__PACKAGE__);
};

my $iset = \&SetFromFile;
*SetFromFile = sub {
	my $class = shift;
	my $arg1 = shift;
	my $arg2 = shift || "";
	
	return $iset->($class, $arg1, $arg2);
};


package Etk::Menu::Item;

my $minew = \&new;
*new = sub {
	my $class = shift;
	my $arg = shift;

	if (defined $arg) {
		if ($arg =~ /^\d+$/) {
			return NewFromStock($arg);
		} else {
			return NewWithLabel($arg);
		}
	}
	return $minew->(__PACKAGE__);
};

package Etk::Menu::Item::Check;

my $micnew = \&new;
*new = sub {
	my $class = shift;
	my $arg = shift;

	if (defined $arg) {
		return NewWithLabel($arg);
	}
	return $micnew->(__PACKAGE__);
};

package Etk::Menu::Item::Image;

my $miinew = \&new;
*new = sub {
	my $class = shift;
	my $arg = shift;

	if (defined $arg) {
		if ($arg =~ /^\d+$/) {
			return NewFromStock($arg);
		} else {
			return NewWithLabel($arg);
		}
	}
	return $miinew->(__PACKAGE__);
};

package Etk::Menu::Item::Radio;

my $mirnew = \&new;
*new = sub {
	my $class = shift;
	my $arg1 = shift;
	my $arg2 = shift;

	if (defined $arg1) {
		if (ref $arg1 && $arg1->isa("Etk::Widget")) {
			return NewFromWidget($arg1);
		} else {
			if (ref $arg2 && $arg2->isa("Etk::Widget")) {
				return NewWithLabelFromWidget($arg1, $arg2);
			} else {
				return NewWithLabel($arg1);
			}
		}
	}
	return $mirnew->(__PACKAGE__);
};


package Etk::ProgressBar;

my $pnew = \&new;
*new = sub {
	my $class = shift;
	my $arg = shift;
	if (defined $arg) {
		return NewWithText($arg);
	} 

	return $pnew->(__PACKAGE__);
};

package Etk::RadioButton;

my $rbnew = \&new;
*new = sub {
	my $class = shift;
	my $arg1 = shift;
	my $arg2 = shift;

	if (defined $arg1) {
		if (ref $arg1 && $arg1->isa("Etk::Widget")) {
			return NewFromWidget($arg1);
		} else {
			if (ref $arg2 && $arg2->isa("Etk::Widget")) {
				return NewWithLabelFromWidget($arg1, $arg2);
			} else {
				return NewWithLabel($arg1);
			}
		}
	}
	return $rbnew->(__PACKAGE__);
};

package Etk::ToolButton;

my $tbnew = \&new;
*new = sub {
	my $class = shift;
	my $arg = shift;

	if (defined $arg) {
		if ($arg =~ /^\d+$/) {
			return NewFromStock($arg);
		} else {
			return NewWithLabel($arg);
		}
	}
	return $tbnew->(__PACKAGE__);
};

package Etk::ToolToggleButton;

my $ttbnew = \&new;
*new = sub {
	my $class = shift;
	my $arg = shift;

	if (defined $arg) {
		if ($arg =~ /^\d+$/) {
			return NewFromStock($arg);
		} else {
			return NewWithLabel($arg);
		}
	}
	return $ttbnew->(__PACKAGE__);
};


package Etk::ToggleButton;

my $tnew = \&new;
*new = sub {
	my $class = shift;
	my $arg = shift;

	if (defined $arg) {
		return NewWithLabel($arg);
	}
	return $tnew->(__PACKAGE__);
};


1;

__END__

=head1 NAME

Etk::Simple - Simplified function calls for Etk

=head1 SYNOPSIS

  use Etk::Simple;
  use Etk::Stock qw/:all/;

  my $button = Etk::Button->new();
  my $button_with_label = Etk::Button->new("This is a label");
  my $button_from_stock = Etk::Button->new(DocumentNew);
  
=head1 DESCRIPTION

Many widgets in Etk have different constructors and require different
calls for each type of constructor.

This module tries to simplify this process by providing a single "new"
constructor that, depending on the number and type of arguments, calls
the required C constructor.

=head1 OVERLOADED WIDGETS

The following are the widgets that have had their constructors overloaded.
For each widget we list the type/number of arguments it accepts

=head2 Etk::Button
	
	undef - creates an empty button
	string - creates a button with a label
	number - creates a button from stock

=head2 Etk::CheckButton
	
	undef - creates an empty checkbutton
	string - creates a checkbutton with a label

=head2 Etk::Image
	
	undef - creates an empty image
	number,number - creates an image from stock with a stock size
	string,string - creates an image from edje with key
	string - creates an image from file

=head2 Etk::Menu::Item

	undef - creates empty menu item
	number - creates menu item from stock
	string - creates menu item with label

=head2 Etk::Menu::Item::Check

	undef - creates empty menu item
	string - creates menu item with label

=head2 Etk::Menu::Item::Image

	undef - creates empty menu item
	number - creates menu item from stock
	string - creates menu item with label

=head2 Etk::Menu::Item::Radio

	undef - creates empty menu item
	object - creates menu item from the specified object
	string - creates menu item with label
	string,object - creates menu item with label from the specified object

=head2 Etk::ProgressBar

	undef - creates empty progressbar
	string - creates a progressbar with text

=head2 Etk::RadioButton

	undef - creates empty radio button
	object - creates radio button from the specified object
	string - creates radio button with label
	string,object - creates radio button with label from the specified object

=head2 Etk::ToolButton

	undef - creates empty tool button
	number - creates tool button from stock
	string - creates tool button with label

=head2 Etk::ToolToggleButton

	undef - creates empty tool togglebutton
	number - creates tool togglebutton from stock
	string - creates tool togglebutton with label

=head2 Etk::ToggleButton

	undef - creates empty togglebutton
	number - creates togglebutton from stock
	string - creates togglebutton with label

=head1 NOTES

Since this module redefines the new() subroutines, it includes a 
`no warnings 'redefine'` to remove warnings.

=head1 AUTHOR

Chady 'Leviathan' Kassouf, E<lt>chady.kassouf@gmail.comE<gt>

=head1 COPYRIGHT AND LICENSE

Copyright (C) 2007 by Chady Kassouf

This library is free software; you can redistribute it and/or modify
it under the same terms as Perl itself, either Perl version 5.8.7 or,
at your option, any later version of Perl 5 you may have available.

=cut

