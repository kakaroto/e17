#!/usr/bin/perl -w

use strict;
use Test;
use X11::Esh;

# Test for the active_network method

plan test => 4, todo => [2,3,4];

my $e = new X11::Esh;
ok($e->Class,'X11::Esh',"Class constructor failed, not runing E");

my $status = $e->active_network();
ok($status,'/^on$|^off$/', 'active_network query failed!');
if ($status =~ /^on$|^off$/) {
  $e->active_network('on');
  ok($e->active_network(),'/^on$/', 'active_network("on") failed!');
  $e->active_network('off');
  ok($e->active_network(), '/^off$/', 'active_network("off") failed!');
  $e->active_network($status);
} else {
    ok(1, 0, 'active_network("on") failed!');
    ok(1, 0, 'active_network("off") failed!');
}

# Done!
