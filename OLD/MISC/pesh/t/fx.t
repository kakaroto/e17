#!/usr/bin/perl -w

use strict;
use Test;
use X11::Esh;

# Test for the fx methods

plan test => 13, todo => [7];

my $e = new X11::Esh;
ok($e->Class,'X11::Esh',"Class constructor failed, not runing E");

my $audio = $e->fx_audio;
ok($audio, '/^on$|^off$/', 'fx audio failed!');

ok($e->fx_autoraise, '/^[\d.]+$/', 'fx autoraise failed!');

ok($e->fx_deskslide, '/^on$|^off$/', 'fx deskslide failed!');

ok($e->fx_dragbar, '/^on$|^off$|^left$|^right$|^top$|^bottom$/', 
   'fx dragbar failed!');

ok($e->fx_edge_resistance, '/^\d+$/', 'fx edge_resistance failed!');

ok($e->fx_edge_snap_resistance, '/^\d+$/', 'fx edge_snap_resistance failed!');

ok($e->fx_mapslide, '/^on$|^off$/', 'fx mapslide failed!');

ok($e->fx_menu_animate, '/^on$|^off$/', 'fx menu_animate failed!');

ok($e->fx_raindrops, '/^on$|^off$/', 'fx raindrops failed!');

ok($e->fx_ripples, '/^on$|^off$/', 'fx ripples failed!');

ok($e->fx_tooltips, '/^[\d.]+$/', 'fx tooltips failed!');

ok($e->fx_window_shade_speed, '/^\d+$/', 'fx window_shade_speed failed!');
