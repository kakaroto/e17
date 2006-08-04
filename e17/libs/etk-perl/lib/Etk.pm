package Etk;

use 5.008004;
use strict;
use warnings;
use Carp;

our $VERSION = '0.02';

require XSLoader;
XSLoader::load('Etk', $VERSION);

use Etk::Object;

# initialize Etk
Etk::init();

END {
	Etk::shutdown();
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

Hisham Mardam Bey, E<lt>hisham.mardambey@gmail.comE<gt> -  
Chady 'Leviathan' Kassouf, E<lt>chady.kassouf@gmail.comE<gt>

=head1 COPYRIGHT AND LICENSE

Copyright (C) 2006 by Hisham Mardam Bey

This library is free software; you can redistribute it and/or modify
it under the same terms as Perl itself, either Perl version 5.8.7 or,
at your option, any later version of Perl 5 you may have available.


=cut


