package Etk::Simple;

use strict;
use Etk;

package Etk::Button;

my $_new = \&new;
*new = sub {
	my $class = shift;
	my $arg = shift;
	return $_new->(__PACKAGE__) unless defined $arg;
	return NewFromStock($arg)
		if $arg + 0 eq $arg;
	return NewWithLabel($arg);
};

package Etk::CheckButton;

my $_new = \&new;
*new = sub {
	my $class = shift;
	my $arg = shift;
	return NewWithLabel($arg) if $arg;
	return $_new->(__PACKAGE__);
};

package Etk::Image;

my $_new = \&new;
*new = sub {
	my $class = shift;
	my $arg1 = shift;
	my $arg2 = shift;
	
	if (defined $arg1) {
		if ($arg1 + 0 eq $arg1) {
			if ($arg2 + 0 ne $arg2) {
				warn "no StockSize defined, using: SizeSmall";
				$arg2 = 0;
			}
			return NewFromStock($arg1, $arg2);
		} else {
			if (defined $arg2) {
				return NewFromEdje($arg1, $arg2);
			} else {
				return NewFromFile($arg1);
			}
		}
	}
			
	return $_new->(__PACKAGE__);
};

package Etk::Menu::Item;

my $_new = \&new;
*new = sub {
	my $class = shift;
	my $arg = shift;

	if (defined $arg) {
		if ($arg+0 eq $arg) {
			return NewFromStock($arg);
		} else {
			return NewWithLabel($arg);
		}
	}
	return $_new->(__PACKAGE__);
};

package Etk::Menu::Item::Check;

my $_new = \&new;
*new = sub {
	my $class = shift;
	my $arg = shift;

	if (defined $arg) {
		return NewWithLabel($arg);
	}
	return $_new->(__PACKAGE__);
};

package Etk::Menu::Item::Image;

my $_new = \&new;
*new = sub {
	my $class = shift;
	my $arg = shift;

	if (defined $arg) {
		if ($arg+0 eq $arg) {
			return NewFromStock($arg);
		} else {
			return NewWithLabel($arg);
		}
	}
	return $_new->(__PACKAGE__);
};

package Etk::Menu::Item::Radio;

my $_new = \&new;
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
	return $_new->(__PACKAGE__);
};


package Etk::ProgressBar;

my $_new = \&new;
*new = sub {
	my $class = shift;
	my $arg = shift;
	if (defined $arg) {
		return NewWithText($arg);
	} 

	return $_new->(__PACKAGE__);
};

package Etk::RadioButton;

my $_new = \&new;
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
	return $_new->(__PACKAGE__);
};

package Etk::ToolButton;

my $_new = \&new;
*new = sub {
	my $class = shift;
	my $arg = shift;

	if (defined $arg) {
		if ($arg+0 eq $arg) {
			return NewFromStock($arg);
		} else {
			return NewWithLabel($arg);
		}
	}
	return $_new->(__PACKAGE__);
};

package Etk::ToolToggleButton;

my $_new = \&new;
*new = sub {
	my $class = shift;
	my $arg = shift;

	if (defined $arg) {
		if ($arg+0 eq $arg) {
			return NewFromStock($arg);
		} else {
			return NewWithLabel($arg);
		}
	}
	return $_new->(__PACKAGE__);
};


package Etk::ToggleButton;

my $_new = \&new;
*new = sub {
	my $class = shift;
	my $arg = shift;

	if (defined $arg) {
		return NewWithLabel($arg);
	}
	return $_new->(__PACKAGE__);
};


1;

