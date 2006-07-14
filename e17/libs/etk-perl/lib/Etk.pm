package Etk;

use 5.008004;
use strict;
use warnings;
use Carp;

require Exporter;
use AutoLoader;

our @ISA = qw(Exporter);

use Etk::Main;
use Etk::Window;

# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.

# This allows declaration	use Etk ':all';
# If you do not need this, moving things directly into @EXPORT or @EXPORT_OK
# will save memory.
our %EXPORT_TAGS = ( 'all' => [ qw() ] );

our @EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );

our @EXPORT = qw( );

our $VERSION = '0.01';

sub AUTOLOAD {
    # This AUTOLOAD is used to 'autoload' constants from the constant()
    # XS function.

    my $constname;
    our $AUTOLOAD;
    ($constname = $AUTOLOAD) =~ s/.*:://;
    croak "&Etk::constant not defined" if $constname eq 'constant';
    my ($error, $val) = constant($constname);
    if ($error) { croak $error; }
    {
	no strict 'refs';
	# Fixed between 5.005_53 and 5.005_61
#XXX	if ($] >= 5.00561) {
#XXX	    *$AUTOLOAD = sub () { $val };
#XXX	}
#XXX	else {
	    *$AUTOLOAD = sub { $val };
#XXX	}
    }
    goto &$AUTOLOAD;
}

require XSLoader;
XSLoader::load('Etk', $VERSION);


# initialize Etk
Etk::Init();

END {
	Etk::Shutdown();
}

# Preloaded methods go here.

# Autoload methods go after =cut, and are processed by the autosplit program.

1;
__END__
# Below is stub documentation for your module. You'd better edit it!

=head1 NAME

Etk - Perl bindings for the Enlightened ToolKit (Etk)

=head1 SYNOPSIS

use Etk;

Etk::etk_init();

my $win = Etk::Window->new();
my $button = Etk::Button->new("Click me!");

$win->Add($button);
$win->ShowAll();

$button->SignalAttach("clicked", \&clicked_cb);

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
