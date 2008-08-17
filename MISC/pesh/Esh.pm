#

# $Id$
#
# 

package X11::Esh;
use vars qw($VERSION @ISA);
use strict;
use Carp;
use X11::Protocol;
use Tie::IxHash;
use POSIX qw(ceil);
use String::Escape qw(printable);
use AutoLoader qw(AUTOLOAD);
use Exporter;

@ISA = qw(Exporter);

$VERSION = sprintf("%d.%02d", q$Revision$ =~ /(\d+)\.(\d+)/);

=head1 NAME

X11::Esh - Perl module to interface with the Enlightment Window Manager

=head1 SYNOPSIS

  use X11::Esh;
  $e = new X11::Esh;
  %windows = $e->windows();
  $resistance = $e->fx_edge_snap_resistance();
  $e->fx_raindrops("off");
  ...

=head1 DESCRIPTION

X11::Esh is a client-side IPC interface to the wonderful X11 
B<Enlightenment> window manager, I<E>. It allows perl programs to query 
and set I<E> parameters in the same vein as the Englightment shell L<eesh>.

=head1 BASIC METHODS

=head2 new

  $e = X11::Esh->new([optionpair, ...]);

The recongized options are:

=over 4

=item debug => $debug

Set the debug flag value to C<$debug>. 

=item display => $display

Use C<$display> as the display to connect to.

=head2 Version

  $e->Vesion;

Return the version of this package.

=head2 Class

  $e->Class;

Return the this packages class name.

=head2 debug

  $status = $e->debug([$value]);

Get/set the debug flag value.

=head2 send_message

  $e->send_message($message);

Package and send value of C<$message> to the window manager. This function
should only be used by advanced users. You are responsible for handling
any response is any is expected.

=head2 send_messages

  $e->send_messages(@messages);

Package and send all the messages on the C<@messages> array to the 
window manager. Restrictions similar to the C<send_message> method apply.

=head2 request_print

  ($response, $source_window) = $e->request_print($message);

Package and send C<$message> to the window manager and wait for the the
synchronous reponse which will be printed and retured. This method is exist
purely to support debugging of your scripts.

=head1 ENLIGHTENMENT IPC COMMANDS

The following is a list of implimented IPC commands callable. In
general where ever the command expects a boolean argument the values
1, "on", "true" maybe used for "on" and the values 0, "off", "true"
may be used for "off". Commands with sub command options may also
specified with the minium unique characters. All commands with
extensive sub-command features are callable directly as methods.

=head2 help

  $text = $e->help([$str]);

Request general help or specific help, if C<$str> is defined, from I<E>.
The resulting help string is returned by the method.

=head2 version

  $ver = $e->version;

Request and return the version string of the window manager.

=head2 copyright

  $str = $e->copyright;

Request and return the copyright string from the window manager.

=head2 autosave

  $status = $e->autosave(["on"|"off"]);

Get/Set the I<autosave> parameter in the window manager.

=head2 default_theme|theme

  $themepath = $e->default_theme(["path/to/theme"]);
  $themepath = $e->theme(["path/to/theme"]);

Get/Set the default theme value.

=head2 restart

  $e->restart();

Retart the window manager.

=head2 restart_wm

  $e->restart_wm("wm_prog");

Exit the window manager and launch the specified alternative window manager.

=head2 restart_theme

  $e->restart_theme("theme");

Restart I<E> with the another specified theme.

=head2 exit

  $e->exit();

Instruct I<E> to exit.

=head2 save_config

  $e->save_config()

Force I<E> to save it's current settings immediately.

=head2 sm_file

  $sm_file = $e->sm_file(["path/to/sm_file_prefix"]);

Get/Set the prefix used for session saves. It's suggested that most users 
leave this allow.

=head2 list_themes|themes

  @themes = $e->list_themes();
  @themes = $e->themes();

List all the currently availabe themes. The themes are returned a list
of paths.

=head2 goto_desktopn

  $desktop = $e->goto_desktop([#|"prev", "next"]);

If no arguments are listed, this method returns the current desktop,
otherwise the the current desktop is changed to that of the argument.
Valid values of the desktop argument is a number or the stings "prev"
and "next".

=head2 goto_area

  ($x, $y) = $e->goto_area([$horiz, $vert]);

If no arguments are listed, this method returns the current area,
otherwise the the current area is changed to that of C<$horiz>,
C<$vert>. The method always return the current area as a list with two
values.

=head2 show_icons

  $status = $e->show_icon("on"|"off");

Get/Set the I<E> parameter that controls the display of icons on the
desktop.

=head2 focus_mode

  $mode = $e->focus_mode(["click"|"clicknograb"|"pointer"|"sloppy"]);

If no argument is specified, this method returns the current focus mode,
otherwise the current focus mode is changed to the value of the argument.
Recognized values for the focus mode are: "click", "clicknograb", "pointer"
and "sloppy". The method always returns the current focus mode. See the
B<Enlightenment> documentation of the meanings of the focus mode types.

=head2 advanced_focus

  $status = $e->advanced_focus($option, [$bool]);

This method gets/sets/toggles the advanced focus settings in the window
manager. C<$option> must be one of the following:

=over 4

=item new_window_focus

All new windows get the keyboard focus.

=item new_popup_window_focus

All new transient windows get focus.

=item new_popup_of_owner_focus

Transient windows from an application that already has the focus
willautomatically receive the focus.

=item raise_on_keyboard_focus_switch 

Raise windows when switching focus with the keyboard.

=item raise_after_keyboard_focus_switch

Raise windows after switching focus with the keyboard.

=item pointer_to_keyboard_focus_window

Warp the pointer to the focused window when changing focus with the
keyboard.

=item pointer_after_keyboard_focus_window

Warp the pointer to the focused window after changing focus with the
keyboard.

=item transients_follow_leader

Allow popup windows appear together with the window that created them.

=item switch_to_popup_location

Warp desktop/area to where a popup window appears.

=item focus_list

Display and use focus list (requires XKB).

=item manual_placement

Allow manual placecment of all new windows by hand.

=back

If argument C<$bool> is not specified, the method returns the current value
for the specified advanced focus sub-feature. C<$bool> must be "on", "off"
or "toggle".

=head2 num_desks|desks

  $desks = $e->num_desks([num]);
  $desks = $e->num_desks([num]);

Get/Set the number of available virtual desktops.

=head2 num_areas|areas

  @areas = $e->num_areas([width, height]);
  @areas = $e->areas([width, height]);

Get/Set the dimensions of all virtual desktops. The size is returned
as a list of x, y values.

=head2 win_op

  $win_op($cmd, $window_id, [@args]);

Perform a window operation. The C<win_op> family of commands all take
a mandatory window argument which must be a valid window id
number. The window operation commands may also be called directly as
listed below:

=over 4

=item annihilate

  $e->win_op_annihilate($window_id);

Annihilate the specified window.

=item area

  @area = $e->win_op_area($window_id, [$x, $y]);

Get/Set an window's display area.

=item border

  $border = $e->win_op_border($window_id, [$bordername]);

Get/Set a window's border type. Recognized border types are may be
retrieved from the I<list_borders_class> method.

=item close

  $e->win_op_close($window_id);

Close the specified window.

=item desk

  $e->win_op_desk($window_id, [#|next|prev]]);

Get/Set the window's display desktop.

=item focus

  $mode = $e->win_op_focus($window_id, ["on"]);

If the second argument is not specified return "on" if the targeted
window has the keyboard focus, else "off". Force the keyboard focus to
the targeted window by specifying the second argument as "on". The
method returns the state of keyboard focus for the targeted window.

=item iconify

  $mode = $e->win_op_iconify($window_id, ["on"|"off"|"toggle"]);

If the second argument is not specified return iconify state of targeted
window. The second argument can be "on", "off", or "toggle".

=item lower

  $e->win_op_lower($window_id);

Change the targeted window's stacking order by lowering it.

=item move

  @coords = $e->win_op_move($window_id, [$x, $y]);

Get/Set the targeted window's coordinated.

=item raise

  $e->win_op_raise($window_id);

Change the targeted window's stacking order by raising it.

=item resize

  @coords = $e->win_op_resize($window_id, [$x, $y]);

Get/Set the targeted window's size.

=item shade

  $mode = $e->win_op_shade($window_id, ["on"|"off"|toggle"]);

Get/Set/Toggle the targeted window's shade value.

=item stick

  $mode = $e->win_op_stick($window_id, ["on"|"off"|toggle"]);

Get/Set/Toggle the targeted window's stickness value.

=item toggle_height

  $e->win_op_toggle_height($window_id, ["available"|"conservative"]);

Toggle the targeted window's height. If the second argument is not
specified the toggle is based on the maximum space, else use the
"available" or "conservative" space rule.

=item toggle_size

  $e->win_op_toggle_size($window_id, ["available"|"conservative"]);

Toggle the targeted window's size. If the second argument is not
specified the toggle is based on the maximum space, else use the
"available" or "conservative" space rule.

=item toggle_width

  $e->win_op_toggle_width($window_id, ["available"|"conservative"]);

Toggle the targeted window's width. If the second argument is not
specified the toggle is based on the maximum space, else use the
"available" or "conservative" space rule.

=head2 window_list|windows

  %hash = $e->window_list();
  %hash = $e->windows();

Get and return a hash of all the currently open windows. The keys in
the hash are the the window ids in "0x08x" format, while the values
are the window titles.

=head2 button_show

  $e->button_show($option, $str);

Show/Hide buttons on the desktop as per the C<$option> and C<$str>
argument. Valid values for C<$option> are: "button", "buttons",
"all_buttons_except" and "all". C<$str> is a button pattern string.

=head2 active_network

  $status = $e->active_network(["on"|"off"]);

Enable/Disable/Query networking.

=head2 fx

The following methods fall under the fx famliy of commands.

=over 4

=item audio

  $mode = $e->fx_audio(["on"|"off"]);

Enable/Disable/Query the audio effects.

=item autoraise

  $value = $e->fx_autoraise(["#|on"|"off"]);

Enable/Disable/Change/Query the auto raise effect. If the specified
argument is a number then the I<autoraise> effect is set that number
of seconds.

=item deskslide

  $mode = $e->fx_deskslide(["on"|"off"]);

Enable/Disable/Query the desktop slide effects.

=item dragbar

  $mode = $e->fx_dragbar(["on"|"off"|"left"|"right"|"top"|"bottom"]);

Get/Set the location of the desktop drag bar.

=item edge_resistance

  $mode = $e->fx_edge_resistance([#|"off"]);

Get/Set/Disable the value of the edge resistance effect. This is the
amount, in 1/100 seconds, of time to push for resistance to give. If
the specified argument is a number then the I<edge_resistance> effect
is set to that value.

=item edge_snap_resistance

  $mode = $e->fx_edge_snap_resistance([#]);

Get/Set the value of the edge snape resistance effect. This is the
number of pixels that a window will resist moving against another
window. If the specified argument is a number then the
I<edge_snap_resistance> effect is set to that value.

=item mapslide

  $mode = $e->fx_mapslide(["on"|"off"]);

Get/Enable/Disable the map slide effect. This the effect that
controls the slide appearances of windows.

=item menu_animate

  $mode = $e->fx_menu_animate(["on"|"off"]);

Get/Enable/Disable the rain drops effect. This the effects that
controls the animation of I<E> menus as they are drawn.

=item raindrops

  $mode = $e->fx_raindrops(["on"|"off"]);

Get/Enable/Disable the menu animate effect. This the effect that draws
raindrops on your desktop root.

=item ripples

  $mode = $e->fx_ripples(["on"|"off"]);

Get/Enable/Disable the water ripples effect. This the effects that
draws water ripples on your desktop root.

=item tooltips

  $mode = $e->fx_tooltips([#|"on"|"off"]);

Get/Set the tool tips effect. This method controls the timing in
seconds for the display of tooltip effects. Tooltips in general can be
enabled or disabled by specfiying a "on" or "off" as the method
argument.

=item window_shade_speed

  $mode = $e->fx_window_shade_speed([#]);

Get/Set the value of the window shade speed effect in pixels/sec. If
the specified argument is a number then the I<window_shade_speed> is
set to that value.

=head2 move_mode

  $mode = $e->move_mode([$mode]);

Get/Set the window move mode. C<$mode> can be one of the following
window move mode values: "opaque", "lined", "box", "shaded",
"semi-solid" and "translucent". If no mode is specified the method
returns the current mode.

=head2 resize_mode

  $mode = $e->resize_mode([$mode]);

Get/Set the window reisze mode. C<$mode> can be one of the following
window move mode values: "opaque", "lined", "box", "shaded" and
"semi-solid". If no mode is specified the method returns the current
mode.

=head2 pager

The following methods fall under the pager famliy of commands.

  $status = $e->pager(["on"|"off"]);

Get/Set the current pager status.

=item desk

  $status = $e->pager_desk($num, ["on"|"off"]);

Get/Set the pager display status for desktop C<$num>'s pager.

=item hiq

  $staus = $e->pager_hiq(["on"|"off"]);

Get/Set the pager high quality display flag.

=item snap

  $status = $e->pager_snap(["on"|"off"])

Get/Set the pager snap shot flag.

=item zoom

  $status = $e->pager_zoom(["on"|"off");

Get/Set the pager zoom flag.

=item title

  $status = $e->pager_title(["on"|"off");

Enable/Disable the displa of titles in the pagers.

=item scanrate

  $status = $e->pager_scanrate(["on"|"off");

Set the nuber of lines updated per second in the displayed pagers.

=head2 internal_list|internals

  @win_ids = $e->internal_list(["pagers"|"menus"|"dialogs"|"internal_ewin"]);
  @win_ids = $e->internals(["pagers"|"menus"|"dialogs"|"internal_ewin"]);

Retreied a list of the internal items. The optional first argument specifies teh classification of I<E> internal item.

=head2 set_focus

  $win_id = $e->set_focus([$win_id]);

Get/Set window keyboard focus. Force the keyboard focus to be handed
to the window specified by the id in the first argument.

=head2 dialog_ok

  $e->dialog_ok("message");

Display  an I<E> dialog box with an Ok button.

=head2 list_class|classes

  @ary = $e->list_class($classname);
  @ary = $e->classes($classname);

This method list all members of a class. C<$classname> may be one of the following class name:

=item actions

  @ary = $e->list_actions_class();

Retreive the list of configured actions.

=item backgrounds

  @ary = $e->list_backgrounds_class();

Retreive the list of configured background.

=item borders

  @ary = $e->list_borders_class();

Retreive the list of configured borders

=item buttons

  @ary = $e->list_buttons_class();

Retreive the list of configured buttons.

=item cursors

  @ary = $e->list_cursors_class();

Retreive the list of configured cursors.

=item images

  @ary = $e->list_images_class();

Retreive the list of configured images.

=item sounds

  @ary = $e->list_sounds_class();

Retreive the list of configured sounds.

=item text

  @ary = $e->list_text_class();

Retreive the list of configured text classes.

=head2 list_classes

  %hash = $e->list_classes();

A convenience method to that returns a hash of lists of all the class
names the window manager knows of. The keys to the hash are the class
names, with the values being anonymous lists of classes.

=head2 play_sound

  $e->play_sound($sound_class);

Ask I<E> to play the sound identified by the C<$sound_class>.

=head2 sound_class|soundclass

  $e->sound_class("create"|"delete", $class_name, [$filename]);
  $e->soundclass("create"|"delete", $class_name, [$filename]);

Create/Delete a sound class with C<$class_name> as the name.

=head2 sound_create_class

  $e->sound_create_class($class_name, $filename);

Create a sound class with C<$class_name> as the name related to file
C<$filename>.

=head2 sound_delete_class

  $e->sound_delete_class($class_name);

Delete the sound class called C<$class_name>.

=head2 image_class|imageclass

  $e->image_class();
  $e->imageclass();

Creat/Delete/Modify an image class. Presently this method is
unimplemented.

=head2 text_class|textclass

  $e->text_class();
  $e->textclass();

Creat/Delete/Modify a text class. Presently this method is
unimplemented.

=head2 cursor

  $e->cursor();

Creat/Delete/Modify a cursor. Presently this method is unimplemented.

=head2 general_info|info

  $response = $e->general_info($info_option);

Retreive some general information for I<E>. The following are the
available information options:

=over 4

=item screen_size

  ($x, $y) = $e->general_info("screen_size");

Retrieve the screen size and return it as a list.

=head2 module

  @modules = $e->module(["load"|"unload", $module]);

Load/Unload/List modules. If no arguments are specified the method
returnes a list of the currently load modules. For the "load" and
"unload" sub-command the method returns the command compleation status
from I<E>.

=head1 INTERNAL METHODS

The following methods are concidered private and subject to
change. Use them at your own risk.

=head2 _fh

  $e->_fh();

Return the the underlying connection file handle. Maybe used in select
calls to determine if there is any responses.

=head2 _get_response

  ($response, $id) = $e->_get_response

Perform a blocking call to check for responses from the window
manager.

=head1 SEE ALSO

L<X11::Protocol>, L<eesh>, L<enlightenment>

=head1 COPYRIGHT

Copyright 1998-1999 Anthony Mutiso.

This script is free software; you can redistribute it and/or
modify it under the same terms as Perl itself.

=head1 AUTHOR

Anthony Mutiso <anm.mlist01@iname.com>

=cut

## methods

sub new {
  my $proto = shift;
  my $class = ref($proto) || $proto;
  my $self = {};
  bless($self,$class);
  $self->_init(@_);
  $self;
}

sub Class {
  my $self = shift;

  ref $self;
}

sub _init {
  my $self = shift;
  my %resp = ('window_id' => 0, 'string' => "", 'complete' => 0);

  $self->{debug} = 0;
  $self->{name} = $self->Class;
  $self->{version} = $VERSION;
  $self->{response} = \%resp;

  # user configuration
  if (@_) {
    my %extra = @_;
    @$self{keys %extra} = values %extra;
  }

  if (defined $self->{display}) {
    $self->{xconn} = X11::Protocol->new($self->{display});
  } else {
    $self->{xconn} = X11::Protocol->new;
  }
  $self->{my_window} = $self->{xconn}->new_rsrc;
  $self->{my_window_xid} = sprintf("%8x\n", $self->{my_window});

  $self->{xconn}->
    CreateWindow($self->{my_window}, $self->{xconn}->root, 'InputOutput',
		 $self->{xconn}->root_depth, 'CopyFromParent', (-100, -100), 
		 5, 5, 0,
		 event_mask => 
		 $self->{xconn}->pack_event_mask('StructureNotify',
						 'SubstructureNotify'),
		);
  {
    my $a = $self->{xconn}->atom("ENLIGHTENMENT_COMMS");
    my @prop = $self->{xconn}->
      GetProperty($self->{xconn}->root, $a, 'AnyPropertyType', 0, 14, 0);
    $prop[0] =~ s/WINID\s+/0x/;
    $self->{wm_window} = oct $prop[0];
    $self->{wm_window_xid} = sprintf("%8x\n", $self->{wm_window});
  }


  $self->{xconn}->{event_handler} = sub {$self->_handle_event(@_);};
  $self->{msg_atom} = $self->{xconn}->atom("ENL_MSG");
  $self->{xconn}->{_esh} = $self;

  printf("%s: my_win[0x%08x] wm_win[0x%08x]: %s\n",
	 $self->{name}, $self->{my_window}, $self->{wm_window}) 
    if $self->{debug};

  $self;
}

sub Version {
  # usage: $version = $obj->Version();
  my ($self) = @_;

  $self->{version};
}  

sub debug {
  # usage: $status = $obj->debug([$debug]);
  my $self = shift;

  $self->{debug} = $_[0] if defined $_[0];
  $self->{debug};
}

sub _fh {
  # usage: $obj->_fh();
  my $self = shift;

  $self->{xconn}->connection->fh;
}

sub _handle_clientmessage_event {
  # usage: $complted = $obj->_handle_clientmessage_event(%ev);
  my ($self, %ev) = @_;
  my ($id, $mesg) = unpack("A8a13", $ev{data});
  $self->{response}->{window_id} = oct($id);
  $mesg =~ s/\000.*$//s;
  $self->{response}->{string} .= $mesg;
#  printf("%s::_handle_clientmessage_event: [%s][%d]/[%s][%s][%d]\n",
#	 $self->{name}, printable($ev{data}), length($ev{data}), $id, 
#	 printable($mesg), length($mesg))
  printf("%s::_handle_clientmessage_event: [%s][%s][%d]\n",
	 $self->{name}, $id, printable($mesg), length($mesg))
    if $self->{debug};
  length($mesg) == 12 ? 0 : 1;
}

sub _handle_event {
  # usage: $obj->_handle_event(%ev);
  my($self, %ev) = @_;

 SWITCH: {
    $ev{'name'} eq "ClientMessage" && do {
      $self->{response}->{complete} = $self->_handle_clientmessage_event(%ev);
      last SWITCH;
    };
    $ev{'name'} eq "DestroyNotify" && do {
      $self->{response}->{complete} = 1;
      last SWITCH;
    };
    $self->_print_event(%ev);
    $self->{response}->{complete} = 1;
  }
}

sub _get_response {
  # usage: ($str, $id) = $obj->_get_response();
  my ($self) = @_;
  my ($id, $res) = (0,"");

  $self->{response}->{complete} = 0;
  $self->{response}->{string} = "";

#  printf("---XDB[%s]: X11::Esh::_get_response: before \"handle_input\"\n",__LINE__);
  $self->{xconn}->handle_input while !$self->{response}->{complete};
  
  $res = $self->{response}->{string};
  $id = $self->{response}->{window_id};
  $self->{response}->{string} = "";
  $self->{response}->{string} = 0;
  printf STDERR ("%s::_get_response: response[%s] id[0x08x]\n", $self->{name}, 
		 $res,$id) if $self->{debug};
  ($res,$id);
}

sub _break_str {
  # usage: break_str $len, $str
  my ($self, $len, $str) = @_;
  my @ary = ();
  for my $index (0..(ceil(length($str)/$len)-1)) {
    push(@ary, substr($str,$index*$len,$len));
  }
  push(@ary, "") if length($ary[$#ary]) == $len;
  @ary;
}

sub _print_event {
  # usage: $obj->_print_event(%ev);
  my($self, %ev) = @_;
  my($i);
#  $last_event_time = $ev{'time'} if $ev{'time'};
#  exit if $ev{'name'} eq "KeyPress" and ($ev{'detail'} == 24 or $done);
  print delete($ev{'name'}), ": ";
  print join(", ", map("$_ $ev{$_}", keys %ev)), "\n";
}

sub _convert_id {
  # usage: $int = $obj->_convert_id($id);
  # convert a string number to int
  my ($self, $id) = @_;

  croak $self->Class . "::_convert_id: missing id argument "
    unless defined $id;

 SWITCH: {
    $id =~ /^0([0-7]*|[xX][a-fA-F\d]*)$/ && do {
      $id = oct $id;
      last SWITCH;
    };
    $id =~ /^[-+]?[1-9]\d*$/ && do {
      $id += 0;
      last SWITCH;
    };
    $id =~ /^[-+]?\d+\.?\d+$/ && do {
      $id += 0.0;
      last SWITCH;
    };
    
    croak $self->Class . "::_convert_id: invalid id \"$id\"";
  }      
   $id;
}

sub send_message {
  # usage: $obj->send_message($message);
  my ($self, $mesg) = @_;

  my %ev = ('name' => 'ClientMessage',
	    'window' => $self->{wm_window},
	    'type' => $self->{msg_atom},
	    'format' => 8,
	   );

  foreach ($self->_break_str(12, $mesg)) {
    $ev{data} = pack("A8a13", $self->{my_window_xid}, $_);
    printf STDERR ("%s::send_message: data[%s]\n", $self->{name}, 
		   printable($ev{data})) if $self->{debug};
    $self->{xconn}->
      SendEvent($self->{wm_window}, 0, 0, $self->{xconn}->pack_event(%ev));
  }
  $self;
}

sub send_messages {
  # usage: $obj->send_messages(@messages);
  my $self = shift;

  map {$self->send_message($_)} @_;
  $self;
}

sub request_print {
  # usage: ($response, $source_window) = $obj->request_print($message);
  my ($self, $str) = @_;

  $self->send_message($str);
  my ($res, $swin) = $self->_get_response;
  printf("Request: \"%s\"\nResponse From: 0x%08x\n%s\n", $str, $swin, $res);

  ($res, $swin);
}

## Method aliases

*X11::Esh::areas      = \&X11::Esh::num_areas;
*X11::Esh::classes    = \&X11::Esh::list_class;
*X11::Esh::desks      = \&X11::Esh::num_desks;
*X11::Esh::info       = \&X11::Esh::general_info;
*X11::Esh::internals  = \&X11::Esh::internal_list;
*X11::Esh::theme      = \&X11::Esh::default_theme;
*X11::Esh::themes     = \&X11::Esh::list_themes;
*X11::Esh::windows    = \&X11::Esh::window_list;
*X11::Esh::soundclass = \&X11::Esh::sound_class;
*X11::Esh::imageclass = \&X11::Esh::image_class;
*X11::Esh::textclass  = \&X11::Esh::text_class;

1;

__END__

## AutoSplit methods

## Enlightenment IPC methods (in alphabetic order)

sub active_network {
  # usage: $status = $obj->active_network(["on"|"off"]);
  my ($self, $arg) = @_;

  if (defined $arg) {
    croak $self->Class . "::active_network: invalid argument \"$arg\""
      unless $arg =~ /^\d+$|^on$|^off$|^true$|^false$/i;
    my $flag = $arg =~ /^0$|^off$|^false$/i ? "off" : "on";
    $self->send_message("active_network $flag");
    $flag;
  } else {
    $self->send_message("active_network ?");
    my ($res, $swin) = $self->_get_response;
    $res =~ s/^.*:\s+//;
    chomp($res);
    $res;
  }
}

sub advanced_focus {
  # usage: $status = $obj->advanced_focus($option, [$bool]);
  # $option is on of:
  #   focus_list
  #   manual_placement
  #   new_popup_of_owner_focus
  #   new_popup_window_focus
  #   new_window_focus
  #   pointer_after_keyboard_focus_window
  #   pointer_to_keyboard_focus_window
  #   raise_after_keyboard_focus_switch
  #   raise_on_keyboard_focus_switch
  #   switch_to_popup_location
  #   transients_follow_leader
  my ($self, $option, $arg) = @_;

  $option = lc $option;
  croak $self->Class . "::advanced_focus: \"\$option, [\$arg]\" "
      . "invalid argument" 
	unless defined $option;
  croak $self->Class . "::advanced_focus: invalid option \"\$option\""
    if $option !~
      /^new_window_focus$|^new_popup_window_focus$|^new_popup_of_owner_focus$|^raise_on_keyboard_focus_switch$|^raise_after_keyboard_focus_switch$|^pointer_to_keyboard_focus_window$|^pointer_after_keyboard_focus_window$|^transients_follow_leader$|^switch_to_popup_location$|^focus_list$|^manual_placement$/;

  if (defined $arg) {
    croak $self->Class . "::advanced_focus: $option: invalid argument \"$arg\""
      unless $arg =~ /^\d+$|^on$|^off$|^true$|^false$/i;
    my $flag = $arg =~ /^0$|^off$|^false$/i ? "off" : "on";
    $self->send_message("advanced_focus $option $flag");
    $flag;
  } else {
    $self->send_message("advanced_focus $option ?");
    my ($res, $swin) = $self->_get_response;
    $res =~ s/^.*:\s+//;
    chomp($res);
    $res;
  }
}

sub autosave {
  # usage: $status = $obj->autosave(["on"|"off"]);
  my $self = shift;

  if (defined $_[0]) {
    my $arg = shift;
    croak $self->Class . "::autosave: invalid argument \"$arg\""
      unless $arg =~ /^\d+$|^on$|^off$|^true$|^false$/i;
    my $flag = $arg =~ /^0$|^off$|^false$/i ? "off" : "on";
    $self->send_message("autosave $flag");
    $flag;
  } else {
    $self->send_message("autosave ?");
    my ($res, $swin) = $self->_get_response;
    $res =~ s/^.*:\s+//;
    chomp($res);
    $res;
  }
}

sub button_show {
  # usage: $obj->button_show("button"|"buttons"|"all_buttons_except"|"all", 
  #			     $str);
  my ($self, $option, $arg) = @_;

  $option = lc $option;
  croak $self->Class . "::button_show: \"\$option, \$arg\" "
      . "invalid argument" 
	unless defined $option and defined $arg;
  croak $self->Class . "::button_show: invalid option \"\$option\""
    if $option !~ /^button$|^buttons$|^all_buttons_except$|^all$/;
  $self->send_message("button_show $option $arg");
}

sub copyright {
  # usage: $str = $obj->copyright;
  my $self = shift;

  $self->send_message("copyright");
  my ($res, $swin) = $self->_get_response;
  chomp($res);
  $res;
}

sub cursor {
  # usage: $obj->cursor();
  my $self = shift;

  # no-op - not implemented yet
  -1;
}

sub default_theme {
  # usage: $themepath = $obj->default_theme(["path/to/theme"]);
  my $self = shift;

  if (defined $_[0]) {
    my $arg = shift;
    $self->send_message("default_theme $arg");
    $arg;
  } else {
    $self->send_message("default_theme ?");
    my ($res, $swin) = $self->_get_response;
    chomp($res);
    $res;
  }
}

sub dialog_ok {
  # usage: $obj->dialog_ok("message");
  my ($self, $arg) = @_;

  croak $self->Class . "::dialog_ok: \"message\" missing argument" 
    unless defined $arg;
    $self->send_message("dialog_ok $arg");
}

sub exit {
  # usage: $obj->exit();
  my $self = shift;

  $self->send_message("exit");
  $self;
}

sub focus_mode {
  # usage: $mode = $obj->focus_mode(["click"|"clicknograb"|"pointer"|"sloppy"]);
  my ($self, $mode) = @_;

  if (defined $mode) {
    $mode = lc $mode;
    croak $self->Class . "::focus_mode: \"click|clicknograb|pointer|sloppy\":"
      . "invalid argument" 
	unless $mode =~ /^click$|^clicknograb$|^pointer$|^sloppy$/;
    $self->send_message("focus_mode $mode");
    $mode;
  } else {
    $self->send_message("focus_mode ?");
    my ($res, $swin) = $self->_get_response;
    $res =~ s/^.*:\s+//;
    chomp($res);
    $res;
  }
}

## fx group:
#    audio <on/off>
#    autoraise <on/off/#>
#    deskslide <on/off>
#    dragbar <on/off/left/right/top/bottom>
#    edge_resistance <#/?/off>
#    edge_snap_resistance <#/?>		# not implemented yet!
#    mapslide <on/off>
#    menu_animate <on/off>
#    raindrops <on/off>
#    ripples <on/off>
#    tooltips <on/off/#>
#    window_shade_speed <#>

sub fx_audio {
  # usage: $mode = $obj->fx_audio(["on"|"off"]);
  my ($self, $arg) = @_;

  if (defined $arg) {
    croak $self->Class . "::fx_audio: invalid argument \"$arg\""
      unless $arg =~ /^\d+$|^on$|^off$|^true$|^false$/i;
    my $flag = $arg =~ /^0$|^off$|^false$/i ? "off" : "on";
    $self->send_message("fx audio $flag");
    $flag;
  } else {
    $self->send_message("fx audio ?");
    my ($res, $swin) = $self->_get_response;
    $res =~ s/^.*:\s+//;
    chomp($res);
    $res;
  }
}

sub fx_autoraise {
  # usage: $value = $obj->fx_autoraise(["#|on"|"off"]);
  my ($self, $arg) = @_;

  if (defined $arg) {
    croak $self->Class . "::fx_autoraise: invalid argument \"$arg\""
      unless $arg =~ /^\d+(\.\d*)?$|^on$|^off$|^true$|^false$/i;
    my $flag = $arg =~ /^\d+(\.\d*)?$/ ? $arg : 
      $arg =~ /^off$|^false$/i ? "off" : "on";
    $self->send_message("fx autoraise $flag");
    $flag;
  } else {
    $self->send_message("fx autoraise ?");
    my ($res, $swin) = $self->_get_response;
    $res =~ s/^.*:\s+//;
    chomp($res);
    $res =~ s/^\s*([\d.]+)[^\d]*$/$1/;
    $res + 0.0;
  }
}

sub fx_deskslide {
  # usage: $mode = $obj->fx_deskslide(["on"|"off"]);
  my ($self, $arg) = @_;

  if (defined $arg) {
    croak $self->Class . "::fx_deskslide: invalid argument \"$arg\""
      unless $arg =~ /^\d+$|^on$|^off$|^true$|^false$/i;
    my $flag = $arg =~ /^0$|^off$|^false$/i ? "off" : "on";
    $self->send_message("fx deskslide $flag");
    $flag;
  } else {
    $self->send_message("fx deskslide ?");
    my ($res, $swin) = $self->_get_response;
    $res =~ s/^.*:\s+//;
    chomp($res);
    $res;
  }
}

sub fx_dragbar {
  # usage: $mode = $obj->fx_dragbar(["on"|"off"|"left"|"right"|"top"|"bottom"]);
  my ($self, $arg) = @_;

  if (defined $arg) {
    croak $self->Class . "::fx_dragbar: invalid argument \"$arg\""
      unless $arg =~ /^\d+$|^on$|^off$|^true$|^false$|^left$|^right$|
	              ^top$|^bottom$/i;
    my $flag = $arg =~ /^left$|^right$|^top$|^bottom$/i ? lc $arg : 
      $arg =~ /^0$|^off$|^false$/i ? "off" : "on";
    $self->send_message("fx dragbar $flag");
    $flag;
  } else {
    $self->send_message("fx dragbar ?");
    my ($res, $swin) = $self->_get_response;
    $res =~ s/^.*:\s+//;
    chomp($res);
    $res;
  }
}

sub fx_edge_resistance {
  # usage: $mode = $obj->fx_edge_resistance([#|"off"]);
  my ($self, $arg) = @_;

  if (defined $arg) {
    croak $self->Class . "::fx_edge_resistance: invalid argument \"$arg\""
      unless $arg =~ /^\d+$|^^off$|^false$/i;
    my $flag = $arg =~ /^0$|^off$|^false$/i ? "off" : $arg;
    $self->send_message("fx edge_resistance $flag");
    $flag;
  } else {
    $self->send_message("fx edge_resistance ?");
    my ($res, $swin) = $self->_get_response;
    $res =~ s/^.*:\s+//;
    chomp($res);
    $res =~ s/^\s*(\d+)\s+.*$/$1/;
    $res + 0;
  }
}

sub fx_edge_snap_resistance {
  # usage: $mode = $obj->fx_edge_snap_resistance([#]);
  my ($self, $arg) = @_;

  if (defined $arg) {
    # no-op - not implemented yet
    -1;
  } else {
    # no-op - not implemented yet
    -1;
  }
}

sub fx_mapslide {
  # usage: $mode = $obj->fx_mapslide(["on"|"off"]);
  my ($self, $arg) = @_;

  if (defined $arg) {
    croak $self->Class . "::fx_mapslide: invalid argument \"$arg\""
      unless $arg =~ /^\d+$|^on$|^off$|^true$|^false$/i;
    my $flag = $arg =~ /^0$|^off$|^false$/i ? "off" : "on";
    $self->send_message("fx mapslide $flag");
    $flag;
  } else {
    $self->send_message("fx mapslide ?");
    my ($res, $swin) = $self->_get_response;
    $res =~ s/^.*:\s+//;
    chomp($res);
    $res;
  }
}

sub fx_menu_animate {
  # usage: $mode = $obj->fx_menu_animate(["on"|"off"]);
  my ($self, $arg) = @_;

  if (defined $arg) {
    croak $self->Class . "::fx_menu_animate: invalid argument \"$arg\""
      unless $arg =~ /^\d+$|^on$|^off$|^true$|^false$/i;
    my $flag = $arg =~ /^0$|^off$|^false$/i ? "off" : "on";
    $self->send_message("fx menu_animate $flag");
    $flag;
  } else {
    $self->send_message("fx menu_animate ?");
    my ($res, $swin) = $self->_get_response;
    $res =~ s/^.*:\s+//;
    chomp($res);
    $res;
  }
}

sub fx_raindrops {
  # usage: $mode = $obj->fx_raindrops(["on"|"off"]);
  my ($self, $arg) = @_;

  if (defined $arg) {
    croak $self->Class . "::fx_raindrops: invalid argument \"$arg\""
      unless $arg =~ /^\d+$|^on$|^off$|^true$|^false$/i;
    my $flag = $arg =~ /^0$|^off$|^false$/i ? "off" : "on";
    $self->send_message("fx raindrops $flag");
    $flag;
  } else {
    $self->send_message("fx raindrops ?");
    my ($res, $swin) = $self->_get_response;
    $res =~ s/^.*:\s+//;
    chomp($res);
    $res;
  }
}

sub fx_ripples {
  # usage: $mode = $obj->fx_ripples(["on"|"off"]);
  my ($self, $arg) = @_;

  if (defined $arg) {
    croak $self->Class . "::fx_ripples: invalid argument \"$arg\""
      unless $arg =~ /^\d+$|^on$|^off$|^true$|^false$/i;
    my $flag = $arg =~ /^0$|^off$|^false$/i ? "off" : "on";
    $self->send_message("fx ripples $flag");
    $flag;
  } else {
    $self->send_message("fx ripples ?");
    my ($res, $swin) = $self->_get_response;
    $res =~ s/^.*:\s+//;
    chomp($res);
    $res;
  }
}

sub fx_tooltips {
  # usage: $mode = $obj->fx_tooltips([#|"on"|"off"]);
  my ($self, $arg) = @_;

  if (defined $arg) {
    croak $self->Class . "::fx_tooltips: invalid argument \"$arg\""
      unless $arg =~ /^\d+(\.\d*)?$|^on$|^off$|^true$|^false$/i;
    my $flag = $arg =~ /^\d+(\.\d*)?$/ ? $arg : 
      $arg =~ /^off$|^false$/i ? "off" : "on";
    $self->send_message("fx tooltips $flag");
    $flag;
  } else {
    $self->send_message("fx tooltips ?");
    my ($res, $swin) = $self->_get_response;
    $res =~ s/^.*:\s+//;
    chomp($res);
    $res =~ s/^\s*([\d.]+)[^\d]*$/$1/;
    $res + 0.0;
  }
}

sub fx_window_shade_speed {
  # usage: $mode = $obj->fx_window_shade_speed([#]);
  my ($self, $arg) = @_;

  if (defined $arg) {
    croak $self->Class . "::fx_window_shade_speed: invalid argument \"$arg\""
      unless $arg =~ /^\d+$|^on$|^off$|^true$|^false$/i;
    my $flag = $arg =~ /^\d+$/ ? $arg : 
      $arg =~ /^0$|^off$|^false$/i ? "off" : "on";
    $self->send_message("fx window_shade_speed $flag");
    $flag;
  } else {
    $self->send_message("fx window_shade_speed ?");
    my ($res, $swin) = $self->_get_response;
    $res =~ s/^.*:\s+//;
    chomp($res);
    $res =~ s/^\s*(\d+)[^\d]*$/$1/;
    $res;
  }
}

sub general_info {
  # usage: $response = $obj->general_info("screen_size");
  my ($self, $arg) = @_;

  croak $self->Class . "::general_info: required argument missing"
      unless defined $arg;

  $arg =~ /^s/i && do {
    $self->send_message("general_info screen_size");
    my ($res, $swin) = $self->_get_response;
    $res =~ s/^.*:\s+//;
    chomp($res);
    return split(/\s+/, $res);
  };

  croak $self->Class . "::general_info: unknown subcommand \"$arg\"";
}

sub goto_area {
  # usage: @area = $obj->goto_area([$horiz, $vert]);
  my ($self, $horiz, $vert) = @_;

  if (defined($horiz) and defined($vert)) {
    $self->send_message("goto_area $horiz $vert");
    ($horiz, $vert);
  } else {
    $self->send_message("goto_area ?");
    my ($res, $swin) = $self->_get_response;
    $res =~ s/^.*:\s+//;
    chomp($res);
    split /\s+/, $res;
  }
}

sub goto_desktop {
  # ussage: $desktop = $obj->goto_desktop([#|"prev"|"next"]);
  my $self = shift;

  if (defined $_[0]) {
    my $arg = shift;
    if ($arg =~ /^\d+$/) {
      $self->send_message("goto_desktop $arg");
    } elsif ($arg =~ /^p/i) {
      $self->send_message("goto_desktop prev");
    } elsif ($arg =~ /^n/i) {
      $self->send_message("goto_desktop next");
    } else {
      croak $self->Class . "::goto_desktop: \"#|next|prev\": " .
	"invalid argument";
    }
  }
  $self->send_message("goto_desktop ?");
  my ($res, $swin) = $self->_get_response;
  $res =~ s/^.*:\s+//;
  chomp($res);
  $res;
}

sub help {
  # usage: $str = $obj->help(["command"]);
  my $self = shift;

  if (defined $_[0]) {
    $self->send_message("help $_[0]");
  } else {
    $self->send_message("help");
  }
  my ($res, $swin) = $self->_get_response;
  chomp($res);
  $res;
}

sub image_class {
  # usage: $obj->image_class();
  my $self = shift;

  # no-op - not implemented yet
  -1;
}

sub internal_list {
  # usage: @win_ids = $obj->internal_list(["pagers"|"menus"|"dialogs"|"internal_ewin"]);
  my ($self, $arg) = @_;

  croak $self->Class . "::internal_list: \"type\" missing argument" 
    unless defined $arg;

  if ($arg =~ /^d/i) {
    $self->send_message("internal_list dialogs");
  } elsif ($arg =~ /^i/i) {
    $self->send_message("internal_list internal_ewin");
  } elsif ($arg =~ /^m/i) {
    $self->send_message("internal_list menus");
  } elsif ($arg =~ /^p/i) {
    $self->send_message("internal_list pagers");
  } else {
    croak $self->Class . "::internal_list: \"$arg\": invalid argument";
  }
  my ($res, $swin) = $self->_get_response;
  $res =~ s/^.*:\s+//;
  chomp($res);
  $res =~ s/ //sg;
  map {oct("0x".$_)} split /\n+/, $res;
}

sub list_actions_class {
  # usage: @ary = $obj->list_actions_class();
  my $self = shift;

  $self->send_message("list_class actions");
  my ($res, $swin) = $self->_get_response;
  chomp($res);
  split /\n+/, $res;
}

sub list_backgrounds_class {
  # usage: @ary = $obj->list_backgrounds_class();
  my $self = shift;

  $self->send_message("list_class backgrounds");
  my ($res, $swin) = $self->_get_response;
  chomp($res);
  split /\n+/, $res;
}

sub list_borders_class {
  # usage: @ary = $obj->list_borders_class();
  my $self = shift;

  $self->send_message("list_class borders");
  my ($res, $swin) = $self->_get_response;
  chomp($res);
  split /\n+/, $res;
}

sub list_buttons_class {
  # usage: @ary = $obj->list_buttons_class();
  my $self = shift;

  $self->send_message("list_class buttons");
  my ($res, $swin) = $self->_get_response;
  chomp($res);
  split /\n+/, $res;
}

sub list_class {
  # usage: @ary = $obj->list_class("classname");
  # classes: 
  #   actions
  #   backgrounds
  #   borders
  #   buttons
  #   cursors
  #   images
  #   sounds
  #   text
  my ($self, $arg) = @_;

  croak $self->Class . "::dialog_ok: \"message\" missing argument" 
    unless defined $arg;

  if ($arg =~ /^a/i) {
    $self->send_message("list_class actions");
  } elsif ($arg =~ /^ba/i) {
    $self->send_message("list_class backgrounds");
  } elsif ($arg =~ /^bo/i) {
    $self->send_message("list_class borders");
  } elsif ($arg =~ /^bu/i) {
    $self->send_message("list_class buttons");
  } elsif ($arg =~ /^c/i) {
    $self->send_message("list_class cursors");
  } elsif ($arg =~ /^i/i) {
    $self->send_message("list_class images");
  } elsif ($arg =~ /^s/i) {
    $self->send_message("list_class sounds");
  } elsif ($arg =~ /^t/i) {
    $self->send_message("list_class text");
  } else {
    croak $self->Class . "::list_class: \"$arg\": invalid argument";
  }
  my ($res, $swin) = $self->_get_response;
  chomp($res);
  split /\n+/, $res;
}

sub list_classes {
  # usage: %hash = $obj->list_classes();
  my $self = shift;

  tie(my %hash, 'Tie::IxHash', 
      actions => [$self->list_actions_class],
      backgrounds => [$self->list_backgrounds_class],
      borders => [$self->list_borders_class],
      buttons => [$self->list_buttons_class],
      cursors => [$self->list_cursors_class],
      images => [$self->list_images_class],
      sounds => [$self->list_sounds_class],
      text => [$self->list_text_class],
     );
  %hash;
}

sub list_cursors_class {
  # usage: @ary = $obj->list_cursors_class();
  my $self = shift;

  $self->send_message("list_class cursors");
  my ($res, $swin) = $self->_get_response;
  chomp($res);
  split /\n+/, $res;
}

sub list_images_class {
  # usage: @ary = $obj->list_images_class();
  my $self = shift;

  $self->send_message("list_class images");
  my ($res, $swin) = $self->_get_response;
  chomp($res);
  split /\n+/, $res;
}

sub list_sounds_class {
  # usage: @ary = $obj->list_sounds_class();
  my $self = shift;

  $self->send_message("list_class sounds");
  my ($res, $swin) = $self->_get_response;
  chomp($res);
  split /\n+/, $res;
}

sub list_text_class {
  # usage: @ary = $obj->list_text_class();
  my $self = shift;

  $self->send_message("list_class text");
  my ($res, $swin) = $self->_get_response;
  chomp($res);
  split /\n+/, $res;
}

sub list_themes {
  # usage: @themes = $obj->list_themes();
  my $self = shift;

  $self->send_message("list_themes");
  my ($res, $swin) = $self->_get_response;
  chomp($res);
  split(/\n+/,$res);
}

sub module {
  # usage: @modules = $obj->module(["load"|"unload", $module]);
  my ($self, $arg, $module) = @_;

  if (defined $arg) {
    croak $self->Class . "::module: \"load|unload module\" " .
      "missing module argument" unless defined $module;
    if ($arg =~ /^l/i) {
      $self->send_message("module load $module");
    } elsif ($arg =~ /^u/i) {
      $self->send_message("module unload $module");
    } else {
      croak $self->Class . "::module: \"$arg\": invalid argument";
    }
    $module;
  } else {
    $self->send_message("module list");
  }
  my ($res, $swin) = $self->_get_response;
  $res =~ s/^.*:\s+//;
  chomp($res);
  $res =~ s/^no modules loaded$//;
  split /\n+/, $res;
}

sub move_mode {
  # usage: $mode = $obj->move_mode([mode]);
  my ($self, $arg) = @_;

  if (defined $arg) {
    if ($arg =~ /^b/i) {
      $self->send_message("move_mode box");
      "box";
    } elsif ($arg =~ /^l/i) {
      $self->send_message("move_mode lined");
      "lined";
    } elsif ($arg =~ /^o/i) {
      $self->send_message("move_mode opaque");
      "opaque";
    } elsif ($arg =~ /^se|^ss/i) {
      $self->send_message("move_mode semi-solid");
      "semi-solid";
    } elsif ($arg =~ /^sh/i) {
      $self->send_message("move_mode shaded");
      "shaded";
    } elsif ($arg =~ /^t/i) {
      $self->send_message("move_mode translucent");
      "translucent";
    } else {
      croak $self->Class . "::move_mode: \"$arg\": invalid argument";
    }
  } else {
    $self->send_message("move_mode ?");
    my ($res, $swin) = $self->_get_response;
    $res =~ s/^.*:\s+//;
    chomp($res);
    $res;
  }
}

sub num_areas {
  # usage: @areas = $obj->num_areas([width, height]);
  my ($self, $width, $height) = @_;

  if (defined($width) and defined($height)) {
    croak $self->Class . "::num_areas: \"width, height\": " .
      "invalid arguments" unless $width =~ /^\d+$/ and $height =~ /^\d+$/;
    $self->send_message("num_areas $width $height");
    ($width, $height);
  } else {
    $self->send_message("num_areas ?");
    my ($res, $swin) = $self->_get_response;
    $res =~ s/^.*:\s+//;
    chomp($res);
    split /\s+/, $res;
  }
}

sub num_desks {
  # usage: $desks = $obj->num_desks([num]);
  my $self = shift;

  if (defined $_[0]) {
    my $arg = shift;
    croak $self->Class . "::num_desks: \"#\": " .
      "invalid argument" unless $arg =~ /^\d+$/;
    $self->send_message("num_desks $arg");
    $arg;
  } else {
    $self->send_message("num_desks ?");
    my ($res, $swin) = $self->_get_response;
    $res =~ s/^.*:\s+//;
    chomp($res);
    $res;
  }
}

sub pager {
  # usage: $status = $obj->pager([on|off]);
  my $self = shift;

  if (defined $_[0]) {
    my $arg = shift;
    croak $self->Class . "::pager: invalid argument \"$arg\""
      unless $arg =~ /^\d+$|^on$|^off$|^true$|^false$/i;
    my $flag = $arg =~ /^0$|^off$|^false$/i ? "off" : "on";
    $self->send_message("pager $flag");
    $flag;
  } else {
    $self->send_message("pager ?");
    my ($res, $swin) = $self->_get_response;
    $res =~ s/^.*:\s+//;
    chomp($res);
    $res;
  }
}

sub pager_desk {
  # usage: $status = $obj->pager_desk(num, ["on"|"off"]);
  my ($self, $desk, $arg) = @_;

  croak $self->Class . "::pager_desk: \"# [on|off]\" missing argument"
      unless $arg =~ /^\d+$|^on$|^off$|^true$|^false$/i;

  if (defined $arg) {
    croak $self->Class . "::pager_desk: invalid argument \"$arg\""
      unless $arg =~ /^\d+$|^on$|^off$|^true$|^false$/i;
    my $flag = $arg =~ /^0$|^off$|^false$/i ? "off" : "on";
    $self->send_message("pager desk $desk $flag");
    $flag;
  } else {
    $self->send_message("pager desk $desk ?");
    my ($res, $swin) = $self->_get_response;
    chomp($res);
    $res =~ s/^.*:\s+//;
    $res =~ s/\s+pages\s*$//;
    $res;
  }
}

sub pager_hiq {
  # usage: $staus = $obj->pager_hiq(["on"|"off"]);
  my $self = shift;

  if (defined $_[0]) {
    my $arg = shift;
    croak $self->Class . "::pager_hiq: invalid argument \"$arg\""
      unless $arg =~ /^\d+$|^on$|^off$|^true$|^false$/i;
    my $flag = $arg =~ /^0$|^off$|^false$/i ? "off" : "on";
    $self->send_message("pager hiq $flag");
    $flag;
  } else {
    $self->send_message("pager hiq ?");
    my ($res, $swin) = $self->_get_response;
    $res =~ s/^.*:\s+//;
    chomp($res);
    $res;
  }
}

sub pager_scanrate {
  # usage: $status = $obj->pager_scanrate(["on"|"off");
  my ($self, $arg) = @_;

  if (defined $arg) {
    croak $self->Class . "::pager_scanrate: invalid argument \"$arg\""
      unless $arg =~ /^\d+$/;
    $self->send_message("pager scanrate $arg");
    $arg;
  } else {
    $self->send_message("pager scanrate ?");
    my ($res, $swin) = $self->_get_response;
    $res =~ s/^.*:\s+//;
    chomp($res);
    $res;
  }
}

sub pager_snap {
  # usage: $status = $obj->pager_snap(["on"|"off"])
  my $self = shift;

  if (defined $_[0]) {
    my $arg = shift;
    croak $self->Class . "::pager_snap: invalid argument \"$arg\""
      unless $arg =~ /^\d+$|^on$|^off$|^true$|^false$/i;
    my $flag = $arg =~ /^0$|^off$|^false$/i ? "off" : "on";
    $self->send_message("pager snap $flag");
    $flag;
  } else {
    $self->send_message("pager snap ?");
    my ($res, $swin) = $self->_get_response;
    $res =~ s/^.*:\s+//;
    chomp($res);
    $res;
  }
}

sub pager_title {
  # usage: $status = $obj->pager_title(["on"|"off");
  my $self = shift;

  if (defined $_[0]) {
    my $arg = shift;
    croak $self->Class . "::pager_title: invalid argument \"$arg\""
      unless $arg =~ /^\d+$|^on$|^off$|^true$|^false$/i;
    my $flag = $arg =~ /^0$|^off$|^false$/i ? "off" : "on";
    $self->send_message("pager title $flag");
    $flag;
  } else {
    $self->send_message("pager title ?");
    my ($res, $swin) = $self->_get_response;
    $res =~ s/^.*:\s+//;
    chomp($res);
    $res;
  }
}

sub pager_zoom {
  # usage: $status = $obj->pager_zoom(["on"|"off");
  my $self = shift;

  if (defined $_[0]) {
    my $arg = shift;
    croak $self->Class . "::pager_zoom: invalid argument \"$arg\""
      unless $arg =~ /^\d+$|^on$|^off$|^true$|^false$/i;
    my $flag = $arg =~ /^0$|^off$|^false$/i ? "off" : "on";
    $self->send_message("pager zoom $flag");
    $flag;
  } else {
    $self->send_message("pager zoom ?");
    my ($res, $swin) = $self->_get_response;
    $res =~ s/^.*:\s+//;
    chomp($res);
    $res;
  }
}

sub play_sound {
  # usage: $obj->play_sound($sound_class);
  my ($self, $arg) = @_;

  croak $self->Class . "::play_sound: \"sound_class\" missing argument" 
    unless defined $arg;

  $self->send_message("play_sound $arg");
}

sub resize_mode {
  # usage: $mode = $obj->resize_mode([mode]);
  my ($self, $arg) = @_;

  if (defined $arg) {
    if ($arg =~ /^b/i) {
      $self->send_message("resize_mode box");
      "box";
    } elsif ($arg =~ /^l/i) {
      $self->send_message("resize_mode lined");
      "lined";
    } elsif ($arg =~ /^o/i) {
      $self->send_message("resize_mode opaque");
      "opaque";
    } elsif ($arg =~ /^se|^ss/i) {
      $self->send_message("resize_mode semi-solid");
      "semi-solid";
    } elsif ($arg =~ /^sh/i) {
      $self->send_message("resize_mode shaded");
      "shaded";
    } else {
      croak $self->Class . "::resize_mode: \"$arg\": invalid argument";
    }
  } else {
    $self->send_message("resize_mode ?");
    my ($res, $swin) = $self->_get_response;
    $res =~ s/^.*:\s+//;
    chomp($res);
    $res;
  }
}

sub restart {
  # usage: $obj->restart();
  my $self = shift;

  $self->send_message("restart");
  $self;
}

sub restart_theme {
  # usage: $obj->restart_theme("theme");
  my ($self, $arg) = @_;

  croak $self->Class . "::restart_theme: missing argument \"$arg\""
      unless defined $arg;
  $self->send_message("restart_theme $arg");
  $self;
}

sub restart_wm {
  # usage: $obj->restart_wm("wm_prog");
  my ($self, $arg) = @_;

  croak $self->Class . "::restart_wm: missing argument \"$arg\""
      unless defined $arg;
  $self->send_message("restart_wm $arg");
  $self;
}

sub save_config {
  # usage: $obj->save_config()
  my $self = shift;

  $self->send_message("save_config");
  $self;
}

sub set_focus {
  # usage: $win_id = $obj->set_focus([win_id]);
  my ($self, $id) = @_;

  if (defined $id) {
    $id = $self->_convert_id($id);
    $self->send_message("set_focus $id");
    $id;
  } else {
    $self->send_message("set_focus ?");
    my ($res, $swin) = $self->_get_response;
    $res =~ s/^.*:\s+//;
    chomp($res);
    oct("0x" . $res);
  }
}

sub show_icons {
  # usage: $status = $obj->show_icon("on"|"off");
  my ($self, $arg) = @_;

  if (defined $arg) {
    croak $self->Class . "::show_icons: invalid argument \"$arg\""
      unless $arg =~ /^\d+$|^on$|^off$|^true$|^false$/i;
    my $flag = $arg =~ /^0$|^off$|^false$/i ? "off" : "on";
    $self->send_message("show_icons $flag");
    $flag;
  } else {
    $self->send_message("show_icons ?");
    my ($res, $swin) = $self->_get_response;
    $res =~ s/^.*:\s+//;
    chomp($res);
    $res;
  }
}

sub sm_file {
  # usage: $sm_file = $obj->sm_file(["path/to/sm_file_prefix"]);
  my $self = shift;

  if (defined $_[0]) {
    my $arg = shift;
    $self->send_message("sm_file $arg");
    $arg;
  } else {
    $self->send_message("sm_file ?");
    my ($res, $swin) = $self->_get_response;
    chomp($res);
    $res;
  }
}

sub sound_class {
  # usage: $obj->sound_class("create"|"delete", "classname", [$filename]);
  my ($self, $op, $classname, $filename) = @_;

  croak $self->Class . "::sound_class: \"create|delete, classname, " .
    "[filename]\" missing argument" unless defined $op and defined $classname;

  if ($op =~ /^c/i) {
    croak $self->Class . "::sound_class: \"create class_name path\" " .
      "missing filename argument" unless defined $filename;
    $self->send_message("soundclass create $classname $filename");
  } elsif ($op =~ /^d/i) {
    $self->send_message("soundclass delete $classname");
  } else {
    croak $self->Class . "::sound_class: \"$op\": invalid argument";
  }
}

sub sound_create_class {
  # usage: $obj->sound_create_class("classname", $filename);
  my ($self, $classname, $filename) = @_;

  croak $self->Class . "::sound_create_class: \"classname, filename\" " .
    "missing argument" unless defined $classname and defined $filename;
  $self->send_message("soundclass create $classname $filename");
}

sub sound_delete_class {
  # usage: $obj->sound_delete_class("classname);
  my ($self, $op, $classname, $filename) = @_;

  croak $self->Class . "::sound_delete_class: \"classname\" " .
    "missing argument" unless defined $classname;
  $self->send_message("soundclass delete $classname");
}

sub text_class {
  # usage: $obj->text_class();
  my $self = shift;

  # no-op - not implemented yet
  -1;
}

sub version {
  # usage: $ver = $obj->version;
  my $self = shift;

  $self->send_message("version");
  my ($res, $swin) = $self->_get_response;
  chomp($res);
  $res =~ s/\s+/ /g;
  $res;
}

## win_op group:
# win_op iconify [?|on|off|toggle]
# win_op lower
# win_op move ?|x, y
# win_op raise
# win_op resize ?|x, y
# win_op shade [?|on|off]
# win_op stick [?|on|off]
# win_op toggle_height conservative|available|absolute|maximum
# win_op toggle_size conservative|available|absolute|maximum
# win_op toggle_width conservative|available|absolute|maximum

sub win_op {
  # usage: $obj->win_op($cmd, $window_id, [@args]);
  my ($self, $cmd, @args) = @_;
  my $ret = undef;

  croak $self->Class . "::win_op: invalid arguments"
    unless scalar @args > 0;

 SWITCH: {
    # annihilate	# not implimented
    $cmd =~ /^an/i && do {
      $ret = $self->win_op_annihilate(@args);
      last SWITCH;
    };
    # area
    $cmd =~ /^ar/i && do {
      $ret = $self->win_op_area(@args);
      last SWITCH;
    };
    # border
    $cmd =~ /^b/i && do {
      $ret = $self->win_op_border(@args);
      last SWITCH;
    };
    # close
    $cmd =~ /^c/i && do {
      $ret = $self->win_op_close(@args);
      last SWITCH;
    };
    # desk
    $cmd =~ /^d/i && do {
      $ret = $self->win_op_desk(@args);
      last SWITCH;
    };
    # focus
    $cmd =~ /^f/i && do {
      $ret = $self->win_op_focus(@args);
      last SWITCH;
    };
    # iconify
    $cmd =~ /^i/i && do {
      $ret = $self->win_op_iconify(@args);
      last SWITCH;
    };
    # lower
    $cmd =~ /^l/i && do {
      $ret = $self->win_op_lower(@args);
      last SWITCH;
    };
    # move
    $cmd =~ /^m/i && do {
      $ret = $self->win_op_move(@args);
      last SWITCH;
    };
    # raise
    $cmd =~ /^ra/i && do {
      $ret = $self->win_op_raise(@args);
      last SWITCH;
    };
    # resize
    $cmd =~ /^re/i && do {
      $ret = $self->win_op_resize(@args);
      last SWITCH;
    };
    # shade
    $cmd =~ /^sh/i && do {
      $ret = $self->win_op_shade(@args);
      last SWITCH;
    };
    # stick
    $cmd =~ /^st/i && do {
      $ret = $self->win_op_stick(@args);
      last SWITCH;
    };
    # toggle_height, toggle_size, toggle_width
    $cmd =~ /^toggle_h/i && do {
      $ret = $self->win_op_toggle_height(@args);
      last SWITCH;
    };
    $cmd =~ /^toggle_s/i && do {
      $ret = $self->win_op_toggle_size(@args);
      last SWITCH;
    };
    $cmd =~ /^toggle_w/i && do {
      $ret = $self->win_op_toggle_width(@args);
      last SWITCH;
    };
    croak $self->Class . "::win_op: invalid command type, \"$cmd\"";
  }
   $ret;
}

sub win_op_annihilate {
  # usage: $obj->win_op_annihilate($window_id);
  my ($self, $wid) = @_;
  my ($wid_str) = (sprintf("%08x",$self->_convert_id($wid)));

  croak $self->Class . "::win_op_annihilate: invalid arguments"
    unless defined $wid;

  # not implimented
  # $self->send_message("win_op $wid_str annihilate");
  -1;
}

sub win_op_area {
  # usage: @area = $obj->win_op_area($window_id, [$x, $y]);
  my ($self, $wid, $x, $y) = @_;
  my ($wid_str) = (sprintf("%08x",$self->_convert_id($wid)));

  croak $self->Class . "::win_op_area: invalid arguments"
    unless defined $wid;

  if (defined $x and defined $y) {
    $self->send_message("win_op $wid_str area $x $y");
    ($x, $y);
  } else {
    $self->send_message("win_op $wid_str area ?");
    my ($res, $swin) = $self->_get_response;
    $res =~ s/^.*:\s+//;
    chomp($res);
    split /\s+/, $res;
  }
}

sub win_op_border {
  # usage: $border = $obj->win_op_border($window_id, [$bordername]);
  my ($self, $wid, $bordername) = @_;
  my ($wid_str) = (sprintf("%08x",$self->_convert_id($wid)));

  croak $self->Class . "::win_op_border: invalid arguments"
    unless defined $wid;

  if (defined $bordername) {
    $self->send_message("win_op $wid_str border $bordername");
    $bordername;
  } else {
    $self->send_message("win_op $wid_str border ?");
    my ($res, $swin) = $self->_get_response;
    $res =~ s/^.*:\s+//;
    chomp($res);
    $res;
  }
}

sub win_op_close {
  # usage: $obj->win_op_close($window_id);
  my ($self, $wid) = @_;
  my ($wid_str) = (sprintf("%08x",$self->_convert_id($wid)));

  croak $self->Class . "::win_op_close: invalid arguments"
    unless defined $wid;

  $self->send_message("win_op $wid_str close");
  0;
}

sub win_op_desk {
  # usage: $desk = $obj->win_op_desk($window_id, [#|next|prev]]);
  my ($self, $wid, $arg) = @_;
  my ($wid_str) = (sprintf("%08x",$self->_convert_id($wid)));

  croak $self->Class . "::win_op_desk: invalid arguments"
    unless defined $wid;

  if (defined $arg) {
    if ($arg =~ /^\d+$/) {
      $self->send_message("win_op $wid_str desk $arg");
    } elsif ($arg =~ /^p/i) {
      $self->send_message("win_op $wid_str desk prev");
    } elsif ($arg =~ /^n/i) {
      $self->send_message("win_op $wid_str desk next");
    } else {
      croak $self->Class . "::win_op_desk: \"desk #|next|prev\": " .
	"invalid argument";
    }
  } else {
    $self->send_message("win_op $wid_str desk ?");
    my ($res, $swin) = $self->_get_response;
    $res =~ s/^.*:\s+//;
    chomp($res);
    $res;
  }
}

sub win_op_focus {
  # usage: $mode = $obj->win_op_focus($window_id, ["on"]);
  my ($self, $wid, $arg) = @_;
  my ($wid_str) = (sprintf("%08x",$self->_convert_id($wid)));

  croak $self->Class . "::win_op_focus: invalid arguments"
    unless defined $wid;

  if (defined $arg) {
    croak $self->Class . "::win_op_focus: invalid argument \"$arg\""
      unless $arg =~ /^[1-9]\d*$|^on$|^true$/i;
    $self->send_message("win_op $wid_str focus on");
    "yes";
  } else {
    $self->send_message("win_op $wid_str focus ?");
    my ($res, $swin) = $self->_get_response;
    $res =~ s/^.*:\s+//;
    chomp($res);
    $res;
  }
}

sub win_op_iconify {
  # usage: $mode = $obj->win_op_iconify($window_id, ["on"|"off"|"toggle"]);
  my ($self, $wid, $arg) = @_;
  my ($wid_str) = (sprintf("%08x",$self->_convert_id($wid)));

  croak $self->Class . "::win_op_iconify: invalid arguments"
    unless defined $wid;

  if (defined $arg) {
    croak $self->Class . "::win_op_iconify: invalid argument \"$arg\""
      unless $arg =~ /^\d+$|^on$|^true$|^off$|^false$|^toggle$/i;
    my $flag = $arg =~ /^toggle$/i ? "" : 
      $arg =~ /^\d+$/ ? ($arg == 0 ? " off" : " on") : 
      $arg =~ /^off$|^false$/i ? " off" : " on";
    $self->send_message("win_op $wid_str iconify$flag");
  }
  $self->send_message("win_op $wid_str iconify ?");
  my ($res, $swin) = $self->_get_response;
  $res =~ s/^.*:\s+//;
  chomp($res);
  $res;
}

sub win_op_lower {
  # usage: $obj->win_op_lower($window_id);
  my ($self, $wid) = @_;
  my ($wid_str) = (sprintf("%08x",$self->_convert_id($wid)));

  croak $self->Class . "::win_op_lower: invalid arguments"
    unless defined $wid;

  $self->send_message("win_op $wid_str lower");
  0;
}

sub win_op_move {
  # usage: @coords = $obj->win_op_move($window_id, [$x, $y]);
  my ($self, $wid, $x, $y) = @_;
  my ($wid_str) = (sprintf("%08x",$self->_convert_id($wid)));

  croak $self->Class . "::win_op_move: invalid arguments"
    unless defined $wid;

  if (defined $x and defined $y) {
    $self->send_message("win_op $wid_str move $x $y");
    ($x, $y);
  } else {
    $self->send_message("win_op $wid_str move ?");
    my ($res, $swin) = $self->_get_response;
    $res =~ s/^.*:\s+//;
    chomp($res);
    split /\s+/, $res;
  }
}

sub win_op_raise {
  # usage: $obj->win_op_raise($window_id);
  my ($self, $wid) = @_;
  my ($wid_str) = (sprintf("%08x",$self->_convert_id($wid)));

  $self->send_message("win_op $wid_str raise");
  0;
}

sub win_op_resize {
  # usage: @coords = $obj->win_op_resize($window_id, [$x, $y]);
  my ($self, $wid, $x, $y) = @_;
  my ($wid_str) = (sprintf("%08x",$self->_convert_id($wid)));

  croak $self->Class . "::win_op_shade: invalid arguments"
    unless defined $wid;

  if (defined $x and defined $y) {
    $self->send_message("win_op $wid_str resize $x $y");
    ($x, $y);
  } else {
    $self->send_message("win_op $wid_str resize ?");
    my ($res, $swin) = $self->_get_response;
    $res =~ s/^.*:\s+//;
    chomp($res);
    split /\s+/, $res;
  }
}

sub win_op_shade {
  # usage: $mode = $obj->win_op_shade($window_id, ["on"|"off"|toggle"]);
  my ($self, $wid, $arg) = @_;
  my ($wid_str) = (sprintf("%08x",$self->_convert_id($wid)));

  croak $self->Class . "::win_op: invalid arguments"
    unless defined $wid;

  if (defined $arg) {
    croak $self->Class . "::win_op_shade: invalid argument \"$arg\""
      unless $arg =~ /^\d+$|^on$|^true$|^off$|^false$|^toggle$/i;
    my $flag = $arg =~ /^toggle$/i ? "" : 
      $arg =~ /^\d+$/ ? ($arg == 0 ? " off" : " on") : 
      $arg =~ /^off$|^false$/i ? " off" : " on";
    $self->send_message("win_op $wid_str shade$flag");
  }
  $self->send_message("win_op $wid_str shade ?");
  my ($res, $swin) = $self->_get_response;
  $res =~ s/^.*:\s+//;
  chomp($res);
  $res;
}

sub win_op_stick {
  # usage: $mode = $obj->win_op_stick($window_id, ["on"|"off"|toggle"]);
  my ($self, $wid, $arg) = @_;
  my ($wid_str) = (sprintf("%08x",$self->_convert_id($wid)));

  croak $self->Class . "::win_op_stick: invalid arguments"
    unless defined $wid;

  if (defined $arg) {
    croak $self->Class . "::win_op_stick: invalid argument \"$arg\""
      unless $arg =~ /^\d+$|^on$|^true$|^off$|^false$|^toggle$/i;
    my $flag = $arg =~ /^toggle$/i ? "" : 
      $arg =~ /^\d+$/ ? ($arg == 0 ? " off" : " on") : 
      $arg =~ /^off$|^false$/i ? " off" : " on";
    $self->send_message("win_op $wid_str stick$flag");
  }
  $self->send_message("win_op $wid_str stick ?");
  my ($res, $swin) = $self->_get_response;
  $res =~ s/^.*:\s+//;
  chomp($res);
  $res;
}

sub win_op_toggle_height {
  # usage: $obj->win_op_toggle_height($window_id, 
  #				      ["available"|"conservative"]);
  my ($self, $wid, $arg) = @_;
  my ($wid_str) = (sprintf("%08x",$self->_convert_id($wid)));

  croak $self->Class . "::win_op_toggle_height: invalid arguments"
    unless defined $wid;

  if (defined $arg) {
    if ($arg =~ /^a/i) {
      $self->send_message("win_op $wid_str toggle_height conservative");
    } elsif ($arg =~ /^c/i) {
      $self->send_message("win_op $wid_str toggle_height available");
    } else {
      croak $self->Class . "::win_op_toggle_height: \"[available|conservative]"
	. "\": invalid argument";
    }
  } else {
    $self->send_message("win_op $wid_str toggle_height");
  }
  0;
}

sub win_op_toggle_size {
  # usage: $obj->win_op_toggle_size($window_id, 
  #				    ["available"|"conservative"]);
  my ($self, $wid, $arg) = @_;
  my ($wid_str) = (sprintf("%08x",$self->_convert_id($wid)));

  croak $self->Class . "::win_op_toggle_size: invalid arguments"
    unless defined $wid;

  if (defined $arg) {
    if ($arg =~ /^a/i) {
      $self->send_message("win_op $wid_str toggle_size conservative");
    } elsif ($arg =~ /^c/i) {
      $self->send_message("win_op $wid_str toggle_size available");
    } else {
      croak $self->Class . "::win_op_toggle_size: \"[available|conservative]"
	. "\": invalid argument";
    }
  } else {
    $self->send_message("win_op $wid_str toggle_size");
  }
  0;
}

sub win_op_toggle_width {
  # usage: $obj->win_op_toggle_width($window_id, 
  #				     ["available"|"conservative"]);
  my ($self, $wid, $arg) = @_;
  my ($wid_str) = (sprintf("%08x",$self->_convert_id($wid)));

  croak $self->Class . "::win_op_toggle_width: invalid arguments"
    unless defined $wid;

  if (defined $arg) {
    if ($arg =~ /^a/i) {
      $self->send_message("win_op $wid_str toggle_width conservative");
    } elsif ($arg =~ /^c/i) {
      $self->send_message("win_op $wid_str toggle_width available");
    } else {
      croak $self->Class . "::win_op_toggle_width: \"[available|conservative]"
	. "\": invalid argument";
    }
  } else {
    $self->send_message("win_op $wid_str toggle_width");
  }
  0;
}

sub window_list {
  # usage: %hash = $obj->window_list();
  my $self = shift;

  $self->send_message("window_list");
  my ($res, $swin) = $self->_get_response;
  chomp($res);
  tie my %winlist, 'Tie::IxHash';
  foreach (split(/\n+/,$res)) {
    s/^\s+/0x/;
    my @ary = split /[\s:]+/;
    $winlist{sprintf("0x%08x", oct($ary[0]))} = $ary[1];
  }
  %winlist;
}

## Compound operations

sub window_info {
  # usage: %hash = %obj->window_info($window_id)
  # returns the windows properties, with the expection of title and internal.
  my ($self, $wid) = @_;
  my ($win) = (sprintf("0x%08x",$self->_convert_id($wid)));

  my %winfo = 
    (
     area => [$self->win_op_area($win)],
     border => $self->win_op_border($win),
     coords => [$self->win_op_move($win),
		$self->win_op_resize($win)],
     desk => $self->win_op_desk($win),
     focus => $self->win_op_focus($win),
     iconify => $self->win_op_iconify($win),
     shade => $self->win_op_shade($win),
     stick => $self->win_op_stick($win),
    );
}

sub windows_info {
  # usage: %hash = %obj->windows_info()
  my $self = shift;
  my %info;

  my %wins  = $self->window_list;
  my @iwins = $self->internal_list("i");
  
  foreach my $win (keys %wins){
    $info{$win} = 
      {
       area => [$self->win_op_area($win)],
       border => $self->win_op_border($win),
       coords => [$self->win_op_move($win),
		  $self->win_op_resize($win)],
       desk => $self->win_op_desk($win),
       focus => $self->win_op_focus($win),
       iconify => $self->win_op_iconify($win),
       internal => scalar(grep {$self->_convert_id($win) == $_} @iwins),
       shade => $self->win_op_shade($win),
       stick => $self->win_op_stick($win),
       title => $wins{$win},
      };
  }
  %info;
}

# Done!

#
# Mode: perl-mode
#
