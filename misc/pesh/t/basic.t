#!/usr/bin/perl -w

use strict;
use Test;
use X11::Esh;

# Test for the XXXX method

plan test => 8, todo => [3..6];

my $e = new X11::Esh;
ok($e->Class,'X11::Esh',"Class constructor failed, not runing E");

# copyright
ok($e->copyright(),'/^Copyright/', 'copyright() failed!');

# cursor
my $status = $e->cursor();
ok($status,'/^on$|^off$/', 'cursor query failed!');
if ($status =~ /^on$|^off$/) {
  $e->cursor('on');
  ok($e->cursor(),'/^on$/', 'cursor("on") failed!');
  $e->cursor('off');
  ok($e->cursor(), '/^off$/', 'cursor("off") failed!');
  $e->cursor($status);
} else {
    ok(1, 0, 'cursor("on") failed!');
    ok(1, 0, 'cursor("off") failed!');
}

# exit
ok(0);

# help
ok($e->help(),'/^Enlightenment IPC/', 'help() failed!');
ok($e->help("version"),'/displays the/', 'help("version") failed!');

# Done!
