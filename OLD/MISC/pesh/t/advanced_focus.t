#!/usr/bin/perl -w

use strict;
use Test;
use X11::Esh;

# Test for the advanced_focus method

my @options = qw(
		 focus_list
		 manual_placement
		 new_popup_of_owner_focus
		 new_popup_window_focus
		 new_window_focus
		 pointer_after_keyboard_focus_window
		 pointer_to_keyboard_focus_window
		 raise_after_keyboard_focus_switch
		 raise_on_keyboard_focus_switch
		 switch_to_popup_location
		 transients_follow_leader
		);

plan test => (scalar(@options) * 3) + 1;

my $e = new X11::Esh;
ok($e->Class,'X11::Esh',"Class constructor failed, not runing E");

for my $option (@options) {
  my $status = $e->advanced_focus($option);
  ok($status, '/^on$|^off$/', 'advanced_focus: $option query failed!');
  if ($status =~ /^on$|^off$/) {
    $e->advanced_focus($option, 'on');
    ok($e->advanced_focus($option), '/^on$/', 
       'advanced_focus($option, "on") failed!');
    $e->advanced_focus($option, 'off');
    ok($e->advanced_focus($option), '/^off$/', 
       'advanced_focus($option, "off") failed!');
    $e->advanced_focus($option, $status);
  } else {
    ok(1, 0, 'advanced_focus($option, "on") failed!');
    ok(1, 0, 'advanced_focus($option, "off") failed!');
  }
}

# Done!
