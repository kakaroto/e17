#!/usr/bin/perl -w

use strict;
use Test;
use X11::Esh;

# Test for the XXXX method

plan test => 4, todo => [2,3,4];

my $e = new X11::Esh;
ok($e->Class,'X11::Esh',"Class constructor failed, not runing E");

my $status = $e->XXXX();
ok($status,'/^on$|^off$/', 'XXXX query failed!');
if ($status =~ /^on$|^off$/) {
  $e->XXXX('on');
  ok($e->XXXX(),'/^on$/', 'XXXX("on") failed!');
  $e->XXXX('off');
  ok($e->XXXX(), '/^off$/', 'XXXX("off") failed!');
  $e->XXXX($status);
} else {
    ok(1, 0, 'XXXX("on") failed!');
    ok(1, 0, 'XXXX("off") failed!');
}

# Done!
