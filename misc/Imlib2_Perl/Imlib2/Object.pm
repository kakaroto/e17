package Imlib2::Object;

use strict;
use vars qw($VERSION $AUTOLOAD);
use Carp;
use Imlib2;

# This is incremented every time there is a change to the API
$VERSION = '0.01';

sub new {
  my $proto = shift;
  my $class = ref($proto) || $proto;
  my $self = {};

  my $image;

  if (scalar(@_) == 0) {
    $image = Imlib2::create_image(256, 256);
  } elsif (scalar(@_) == 1) {
    $image = Imlib2::load_image(shift);
  } elsif (scalar(@_) == 2) {
    $image = Imlib2::create_image(shift, shift);
  } else {
    carp("Wrong number of arguments to constructor!");
  }
  
  $self->{IMAGE} = $image;
  
  bless($self, $class);
  return $self;
}


my %functions = (
		 'add_font_path' => 'add_path_to_font_path',
		 'blend' => 'blend_image_onto_image',
		 'crop' => 'create_cropped_image',
		 'DESTROY' => 'free_image',
		 'draw_text' => 'text_draw',
		 'save' => 'save_image',
		 'set_color' => 'context_set_color',
	   );

sub AUTOLOAD {
  my $self = @_[0];
  my $type = ref($self)
    or croak "$self is not an object";
  
  my $name = $AUTOLOAD;
  $name =~ s/.*://;   # strip fully-qualified portion

  $name =~ s|colour|color|; # british spellings too! ;-)

  # allow easier-to-remember names for functions
  $name = $functions{$name} if exists $functions{$name};

  my $sub = 'Imlib2::' . $name;

#  warn "$sub(@_)\n";

  no strict 'refs';

  # generate a new subroutine
  *$AUTOLOAD = sub {
    my $self = shift;
    Imlib2::set_context($self->{IMAGE});
    $sub->(@_);
  };

  # and jump to it
  goto &$AUTOLOAD;
}

sub draw_point {
  my($self, $x, $y) = @_;
  $self->draw_line($x, $y, $x, $y);
}

sub load_font {
  my($self, $fontname) = @_;
  my $font = Imlib2::load_font($fontname);
  $self->context_set_font($font);
}

1;
