#!/usr/bin/perl -w

use strict;
use Test;
use X11::Esh;

# Test for the button_show method

plan test => 5, todo => [2,3,4,5];

my $e = new X11::Esh;
ok($e->Class,'X11::Esh',"Class constructor failed, not runing E");

ok(1, 0, 'button_show("button", str) failed!');
ok(1, 0, 'button_show("buttons", str) failed!');
ok(1, 0, 'button_show("all_buttons_except", str) failed!');
ok(1, 0, 'button_show("all", str) failed!');

# Done!
