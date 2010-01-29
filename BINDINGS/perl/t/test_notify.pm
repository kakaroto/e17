package elmt;

use strict;
use warnings;

sub _bt
{
    my ($data) = @_;
    evas_object_show($$data);
}

sub _bt_close
{
    my ($data) = @_;
    evas_object_hide($$data);
}

sub test_notify
{
   my ($win, $bg, $bx, $tb, $notify, $bt, $lb);

   $win = elm_win_add(undef, "Notify", ELM_WIN_BASIC);
   elm_win_title_set($win, "Notify");
   elm_win_autodel_set($win, 1);

   $bg = elm_bg_add($win);
   elm_win_resize_object_add($win, $bg);
   evas_object_size_hint_weight_set($bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show($bg);

   $tb = elm_table_add($win);
   elm_win_resize_object_add($win, $tb);
   evas_object_size_hint_weight_set($tb, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show($tb);

   $notify = elm_notify_add($win);
   evas_object_size_hint_weight_set($notify, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

   $bx = elm_box_add($win);
   elm_notify_content_set($notify, $bx);
   elm_box_horizontal_set($bx, 1);
   evas_object_show($bx);

   $lb = elm_label_add($win);
   elm_label_label_set($lb, "This position is the default.");
   elm_box_pack_end($bx, $lb);
   evas_object_show($lb);

   $bt = elm_button_add($win);
   elm_button_label_set($bt, "Close");
   evas_object_smart_callback_add($bt, "clicked", \&_bt_close, \$notify);
   elm_box_pack_end($bx, $bt);
   evas_object_show($bt);

   $bt = elm_button_add($win);
   elm_button_label_set($bt, "Top");
   evas_object_smart_callback_add($bt, "clicked", \&_bt, \$notify);
   elm_table_pack($tb, $bt, 1, 0, 1, 1);
   evas_object_show($bt);

   my $notify2 = elm_notify_add($win);
   elm_notify_repeat_events_set($notify2, EINA_FALSE);
   evas_object_size_hint_weight_set($notify2, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_notify_orient_set($notify2, ELM_NOTIFY_ORIENT_BOTTOM);
   elm_notify_timeout_set($notify2, 5);

   $bx = elm_box_add($win);
   elm_notify_content_set($notify2, $bx);
   elm_box_horizontal_set($bx, 1);
   evas_object_show($bx);

   $lb = elm_label_add($win);
   elm_label_label_set($lb, "Bottom position. This notify use a timeout of 5 sec.<br>" .
	 "<b>The events outside the window are blocked.</b>");
   elm_box_pack_end($bx, $lb);
   evas_object_show($lb);

   $bt = elm_button_add($win);
   elm_button_label_set($bt, "Close");
   evas_object_smart_callback_add($bt, "clicked", \&_bt_close, \$notify2);
   elm_box_pack_end($bx, $bt);
   evas_object_show($bt);

   $bt = elm_button_add($win);
   elm_button_label_set($bt, "Bottom");
   evas_object_smart_callback_add($bt, "clicked", \&_bt, \$notify2);
   elm_table_pack($tb, $bt, 1, 2, 1, 1);
   evas_object_show($bt);

   my $notify3 = elm_notify_add($win);
   evas_object_size_hint_weight_set($notify3, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_notify_orient_set($notify3, ELM_NOTIFY_ORIENT_LEFT);
   elm_notify_timeout_set($notify3, 10);

   $bx = elm_box_add($win);
   elm_notify_content_set($notify3, $bx);
   elm_box_horizontal_set($bx, 1);
   evas_object_show($bx);

   $lb = elm_label_add($win);
   elm_label_label_set($lb, "Left position. This notify use a timeout of 10 sec.");
   elm_box_pack_end($bx, $lb);
   evas_object_show($lb);

   $bt = elm_button_add($win);
   elm_button_label_set($bt, "Close");
   evas_object_smart_callback_add($bt, "clicked", \&_bt_close, \$notify3);
   elm_box_pack_end($bx, $bt);
   evas_object_show($bt);

   $bt = elm_button_add($win);
   elm_button_label_set($bt, "Left");
   evas_object_smart_callback_add($bt, "clicked", \&_bt, \$notify3);
   elm_table_pack($tb, $bt, 0, 1, 1, 1);
   evas_object_show($bt);

   my $notify4 = elm_notify_add($win);
   evas_object_size_hint_weight_set($notify4, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_notify_orient_set($notify4, ELM_NOTIFY_ORIENT_RIGHT);

   $bx = elm_box_add($win);
   elm_notify_content_set($notify4, $bx);
   elm_box_horizontal_set($bx, 1);
   evas_object_show($bx);

   $lb = elm_label_add($win);
   elm_label_label_set($lb, "Right position.");
   elm_box_pack_end($bx, $lb);
   evas_object_show($lb);

   $bt = elm_button_add($win);
   elm_button_label_set($bt, "Close");
   evas_object_smart_callback_add($bt, "clicked", \&_bt_close, \$notify4);
   elm_box_pack_end($bx, $bt);
   evas_object_show($bt);

   $bt = elm_button_add($win);
   elm_button_label_set($bt, "Right");
   evas_object_smart_callback_add($bt, "clicked", \&_bt, \$notify4);
   elm_table_pack($tb, $bt, 2, 1, 1, 1);
   evas_object_show($bt);

   my $notify5 = elm_notify_add($win);
   evas_object_size_hint_weight_set($notify5, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_notify_orient_set($notify5, ELM_NOTIFY_ORIENT_TOP_LEFT);

   $bx = elm_box_add($win);
   elm_notify_content_set($notify5, $bx);
   elm_box_horizontal_set($bx, 1);
   evas_object_show($bx);

   $lb = elm_label_add($win);
   elm_label_label_set($lb, "Top Left position.");
   elm_box_pack_end($bx, $lb);
   evas_object_show($lb);

   $bt = elm_button_add($win);
   elm_button_label_set($bt, "Close");
   evas_object_smart_callback_add($bt, "clicked", \&_bt_close, \$notify5);
   elm_box_pack_end($bx, $bt);
   evas_object_show($bt);

   $bt = elm_button_add($win);
   elm_button_label_set($bt, "Top Left");
   evas_object_smart_callback_add($bt, "clicked", \&_bt, \$notify5);
   elm_table_pack($tb, $bt, 0, 0, 1, 1);
   evas_object_show($bt);

   my  $notify6 = elm_notify_add($win);
   evas_object_size_hint_weight_set($notify6, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_notify_orient_set($notify6, ELM_NOTIFY_ORIENT_TOP_RIGHT);

   $bx = elm_box_add($win);
   elm_notify_content_set($notify6, $bx);
   elm_box_horizontal_set($bx, 1);
   evas_object_show($bx);

   $lb = elm_label_add($win);
   elm_label_label_set($lb, "Top Right position.");
   elm_box_pack_end($bx, $lb);
   evas_object_show($lb);

   $bt = elm_button_add($win);
   elm_button_label_set($bt, "Close");
   evas_object_smart_callback_add($bt, "clicked", \&_bt_close, \$notify6);
   elm_box_pack_end($bx, $bt);
   evas_object_show($bt);

   $bt = elm_button_add($win);
   elm_button_label_set($bt, "Top Right");
   evas_object_smart_callback_add($bt, "clicked", \&_bt, \$notify6);
   elm_table_pack($tb, $bt, 2, 0, 1, 1);
   evas_object_show($bt);

   my $notify7 = elm_notify_add($win);
   evas_object_size_hint_weight_set($notify7, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_notify_orient_set($notify7, ELM_NOTIFY_ORIENT_BOTTOM_LEFT);

   $bx = elm_box_add($win);
   elm_notify_content_set($notify7, $bx);
   elm_box_horizontal_set($bx, 1);
   evas_object_show($bx);

   $lb = elm_label_add($win);
   elm_label_label_set($lb, "Bottom Left position.");
   elm_box_pack_end($bx, $lb);
   evas_object_show($lb);

   $bt = elm_button_add($win);
   elm_button_label_set($bt, "Close");
   evas_object_smart_callback_add($bt, "clicked", \&_bt_close, \$notify7);
   elm_box_pack_end($bx, $bt);
   evas_object_show($bt);

   $bt = elm_button_add($win);
   elm_button_label_set($bt, "Bottom Left");
   evas_object_smart_callback_add($bt, "clicked", \&_bt, \$notify7);
   elm_table_pack($tb, $bt, 0, 2, 1, 1);
   evas_object_show($bt);

   my $notify8 = elm_notify_add($win);
   evas_object_size_hint_weight_set($notify8, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_notify_orient_set($notify7, ELM_NOTIFY_ORIENT_BOTTOM_RIGHT);

   $bx = elm_box_add($win);
   elm_notify_content_set($notify8, $bx);
   elm_box_horizontal_set($bx, 1);
   evas_object_show($bx);

   $lb = elm_label_add($win);
   elm_label_label_set($lb, "Bottom Right position.");
   elm_box_pack_end($bx, $lb);
   evas_object_show($lb);

   $bt = elm_button_add($win);
   elm_button_label_set($bt, "Close");
   evas_object_smart_callback_add($bt, "clicked", \&_bt_close, \$notify8);
   elm_box_pack_end($bx, $bt);
   evas_object_show($bt);

   $bt = elm_button_add($win);
   elm_button_label_set($bt, "Bottom Right");
   evas_object_smart_callback_add($bt, "clicked", \&_bt, \$notify8);
   elm_table_pack($tb, $bt, 2, 2, 1, 1);
   evas_object_show($bt);

   evas_object_show($win);
   evas_object_resize($win, 300, 350);
}

1;
