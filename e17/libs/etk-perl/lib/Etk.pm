package Etk;

use 5.008004;
use strict;
use warnings;
use Carp;

our $VERSION = '0.02';

require XSLoader;
XSLoader::load('Etk', $VERSION);

use Etk::Alignment;
use Etk::Button;
use Etk::Canvas;
use Etk::CheckButton;
use Etk::Clipboard;
use Etk::Colorpicker;
use Etk::Combobox;
use Etk::Dialog;
use Etk::Dnd;
use Etk::Drag;
use Etk::Entry;
use Etk::Filechooser;
use Etk::Frame;
use Etk::HBox;
use Etk::HPaned;
use Etk::HSeparator;
use Etk::HSlider;
use Etk::Iconbox;
use Etk::Image;
use Etk::Label;
use Etk::Menu;
use Etk::MessageDialog;
use Etk::Notebook;
use Etk::Paned;
use Etk::PopupWindow;
use Etk::ProgressBar;
use Etk::RadioButton;
use Etk::Range;
use Etk::ScrolledView;
use Etk::Selection;
use Etk::Separator;
use Etk::StatusBar;
use Etk::Table;
use Etk::TextView;
use Etk::Timer;
use Etk::ToggleButton;
use Etk::ToplevelWidget;
use Etk::Tree;
use Etk::VBox;
use Etk::VPaned;
use Etk::VSeparator;
use Etk::VSlider;
use Etk::Viewport;
use Etk::Window;
use Etk::Main;

sub Init
{
    Etk::etk_init();
}

sub Shutdown
{
    Etk::etk_shutdown();
}

# initialize Etk
Etk::Init();

END {
	Etk::Shutdown();
}


1;
__END__

=head1 NAME

Etk - Perl bindings for the Enlightened ToolKit (Etk)

=head1 SYNOPSIS

  use Etk;

  my $win = Etk::Window->new();
  my $button = Etk::Button->new("Click me!");
  $win->Add($button);

  # or

  my $win = Etk::Window->new();
  my $button = $win->AddButton("Click me!");
 
  #

  $win->ShowAll();

  $button->SignalConnect("clicked", \&clicked_cb);

  sub clicked_cb
  {
     print "button clicked!\n";
  }


=head1 DESCRIPTION

This module allows the use of Etk from within Perl. You can use them in one
of two ways, either by using the object oriented approach or directly by
calling the functions (although this is not recommended).

=head2 EXPORT

None by default.

=head2 Exportable functions



=head1 SEE ALSO

Etk documentation is available as Doxygen or in the Etk Explained book:
http://hisham.cc/etk_explained.pdf

http://www.enlightenment.org

=head1 AUTHOR

Hisham Mardam Bey, E<lt>hisham.mardambey@gmail.comE<gt>

=head1 COPYRIGHT AND LICENSE

Copyright (C) 2006 by Hisham Mardam Bey

This library is free software; you can redistribute it and/or modify
it under the same terms as Perl itself, either Perl version 5.8.7 or,
at your option, any later version of Perl 5 you may have available.


=cut


