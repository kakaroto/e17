#!/usr/bin/perl -w

use strict;
use Test;
use X11::Esh;

# Test for the autosave method

plan test => 4;

my $e = new X11::Esh;
ok($e->Class,'X11::Esh',"Class constructor failed, not runing E");

my $status = $e->autosave();
ok($status,'/^on$|^off$/', 'autosave query failed!');
if ($status =~ /^on$|^off$/) {
  $e->autosave('on');
  ok($e->autosave(),'/^on$/', 'autosave("on") failed!');
  $e->autosave('off');
  ok($e->autosave(), '/^off$/', 'autosave("off") failed!');
  $e->autosave($status);
} else {
    ok(1, 0, 'autosave("on") failed!');
    ok(1, 0, 'autosave("off") failed!');
}

# Done!
