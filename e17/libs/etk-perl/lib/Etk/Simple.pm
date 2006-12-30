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
				return NewFromEdje($arg1, $arg2);
			} else {
				return NewFromFile($arg1);
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

