#!/usr/bin/perl -w

use strict;
use Test;
use X11::Esh;

# Test for the win_op methods

plan test => 23, todo => [2];

my $x = new X11::Protocol;
my $win = $x->new_rsrc;
$x->CreateWindow($win, $x->root, 'InputOutput', $x->root_depth,
                 'CopyFromParent', (0, 0), 350, 200, 1,
#                'backing_store' => 'Always',
                 'background_pixel' => $x->white_pixel);
$x->ChangeProperty($win, $x->atom('WM_NAME'), $x->atom('STRING'), 8,
                   'Replace', sprintf("XID: %8x, PID: %d (win_op test)",
				     $win, $$));
$x->MapWindow($win);

my $pause = 0.7;

my $e = new X11::Esh;
ok($e->Class,'X11::Esh',"Class constructor failed, not runing E");

ok($e->win_op_annihilate($win), 0, 'win_op annihilate failed!');

ok($e->win_op_area($win), 2, 'win_op area failed!');

ok($e->win_op_border($win), '/^PAGER$|^MENU_BORDER$|^SIDE$|^SHAPED$|^TRANSIENT$|^FIXEDSIZE$|^DEFAULT$|^BORDERLESS$|^__FALLBACK_BORDER$/',
   'win_op border failed!');

ok($e->win_op_desk($win), '/^\d+$/', 'win_op desk failed!');

ok($e->win_op_focus($win), '/^yes$|^no$/', 'win_op focus failed!');
ok($e->win_op_focus($win, "on"), '/^yes$|^no$/', 'win_op focus failed!');

ok($e->win_op_iconify($win), '/^no$/', 'win_op iconify failed!');
ok($e->win_op_iconify($win, "toggle"), '/^yes$/', 'win_op iconify failed!');
$e->win_op_iconify($win, "toggle");

ok($e->win_op_lower($win), 0, 'win_op lower failed!');

{ 
  my @coords = $e->win_op_move($win);
  ok(@coords, 2, 'win_op move failed!');
  $e->win_op_move($win, map {$_ + 10} @coords);
  my @new_coords = $e->win_op_move($win);
  ok(($new_coords[0] - $coords[0]) == 10 && 
     ($new_coords[1] - $coords[1]) == 10, 1, 'win_op move failed!');
}

ok($e->win_op_raise($win), 0, 'win_op raise failed!');

ok(sprintf("%04dx%04d",$e->win_op_resize($win)), sprintf("%04dx%04d",350,200),
   'win_op resize failed!');
ok(sprintf("%04dx%04d",$e->win_op_resize($win,175,400)),
   sprintf("%04dx%04d",175,400), 'win_op resize failed!');

ok($e->win_op_shade($win), '/^no$/', 'win_op shade failed!');
ok($e->win_op_shade($win, "toggle"), '/^yes$/', 'win_op shade failed!');
$e->win_op_shade($win, "toggle");

ok($e->win_op_stick($win), '/^no$/', 'win_op stick failed!');
ok($e->win_op_stick($win, "toggle"), '/^yes$/', 'win_op stick failed!');
$e->win_op_stick($win, "toggle");

ok($e->win_op_toggle_height($win), 0, 'win_op toggle_height failed!');
select(undef,undef,undef, $pause);
$e->win_op_toggle_height($win);

ok($e->win_op_toggle_size($win), 0, 'win_op toggle_size failed!');
select(undef,undef,undef, $pause);
$e->win_op_toggle_size($win);

ok($e->win_op_toggle_width($win), 0, 'win_op toggle_width failed!');
select(undef,undef,undef, $pause);
$e->win_op_toggle_width($win);

ok($e->win_op_close($win), 0, 'win_op close failed!');

# Done!
