package elmt;

use strict;
use warnings;

my ($slideshow, $bt_start, $bt_stop);

my $package_data_dir = $ENV{'ELM_PACKAGE_DATA_DIR'} || "/opt/e17/share/elementary/images/";

#static Elm_Slideshow_Item_Class itc;
my $img1 = $package_data_dir . "/images/logo.png";

my $img2 = $package_data_dir . "/images/plant_01.jpg";
my $img3 = $package_data_dir . "/images/rock_01.jpg";
my $img4 = $package_data_dir . "/images/rock_02.jpg";
my $img5 = $package_data_dir . "/images/sky_01.jpg";
my $img6 = $package_data_dir . "/images/sky_04.jpg";
my $img7 = $package_data_dir . "/images/wood_01.jpg";

sub _notify_show {
    my ($data) = @_;
    evas_object_show($$data);
    elm_notify_timer_init($$data);
}

sub _next {
    my ($data) = @_;
    elm_slideshow_next($$data);
}

sub _previous {
    my ($data) = @_;
    elm_slideshow_previous($$data);
}

sub _mouse_in {
    my ($data) = @_;
    elm_notify_timeout_set($$data, 0);
}

sub _mouse_out {
    my ($data) = @_;
    elm_notify_timeout_set($$data, 3);
}

sub _hv_select {
    my ($data, $obj) = @_;
    elm_slideshow_transition_set($slideshow, $$data);
    elm_hoversel_label_set($obj, $$data);
}

sub _start {
    my ($data) = @_;
    elm_slideshow_timeout_set($slideshow, elm_spinner_value_get($$data));

    elm_object_disabled_set($bt_start, 1);
    elm_object_disabled_set($bt_stop,  0);
}

sub _stop {
    elm_slideshow_timeout_set($slideshow, 0);
    elm_object_disabled_set($bt_start, 0);
    elm_object_disabled_set($bt_stop,  1);
}

sub _spin {
    my ($data) = @_;
    if (elm_slideshow_timeout_get($slideshow) > 0) {
        elm_slideshow_timeout_set($slideshow, elm_spinner_value_get($$data));
    }
}

sub _get {
    my ($data, $obj) = @_;
    my $photo = elm_photocam_add($$obj);
    elm_photocam_file_set($photo, $$data);
    elm_photocam_zoom_mode_set($photo, ELM_PHOTOCAM_ZOOM_MODE_AUTO_FIT);
    return $photo;
}

sub test_slideshow {
    my ($win, $bg, $notify, $bx, $bt, $hv, $spin);

    #          const Eina_List * l;
    #          const char * transition;

    $win = elm_win_add(undef, "Slideshow", ELM_WIN_BASIC);
    elm_win_title_set($win, "Slideshow");
    elm_win_autodel_set($win, 1);

    $bg = elm_bg_add($win);
    elm_win_resize_object_add($win, $bg);
    evas_object_size_hint_weight_set($bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_show($bg);

    $slideshow = elm_slideshow_add($win);
    elm_slideshow_loop_set($slideshow, 1);
    elm_win_resize_object_add($win, $slideshow);
    evas_object_size_hint_weight_set($slideshow, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_show($slideshow);

    #   itc.func.get = _get;
    #   itc.func.del = undef;

    #   elm_slideshow_item_add($slideshow, &itc, $img1);
    #   elm_slideshow_item_add($slideshow, &itc, $img2);
    #   elm_slideshow_item_add($slideshow, &itc, $img3);
    #   elm_slideshow_item_add($slideshow, &itc, $img4);
    #   elm_slideshow_item_add($slideshow, &itc, $img5);
    #   elm_slideshow_item_add($slideshow, &itc, $img6);
    #   elm_slideshow_item_add($slideshow, &itc, $img7);

    $notify = elm_notify_add($win);
    elm_notify_orient_set($notify, ELM_NOTIFY_ORIENT_BOTTOM);
    elm_win_resize_object_add($win, $notify);
    elm_notify_timeout_set($notify, 3);

    $bx = elm_box_add($win);
    elm_box_horizontal_set($bx, 1);
    elm_notify_content_set($notify, $bx);
    evas_object_show($bx);

    evas_object_event_callback_add($bx, EVAS_CALLBACK_MOUSE_IN,  \&_mouse_in,  \$notify);
    evas_object_event_callback_add($bx, EVAS_CALLBACK_MOUSE_OUT, \&_mouse_out, \$notify);

    $bt = elm_button_add($win);
    elm_button_label_set($bt, "Previous");
    evas_object_smart_callback_add($bt, "clicked", \&_previous, \$slideshow);
    elm_box_pack_end($bx, $bt);
    evas_object_show($bt);

    $bt = elm_button_add($win);
    elm_button_label_set($bt, "Next");
    evas_object_smart_callback_add($bt, "clicked", \&_next, \$slideshow);
    elm_box_pack_end($bx, $bt);
    evas_object_show($bt);

    $hv = elm_hoversel_add($win);
    elm_box_pack_end($bx, $hv);
    elm_hoversel_hover_parent_set($hv, $win);

    # TODO 
    #       foreach my $l (
    #   EINA_LIST_FOREACH(elm_slideshow_transitions_get($slideshow), l, transition)
    #      elm_hoversel_item_add($hv, transition, undef, 0, _hv_select, transition);

    #elm_hoversel_label_set($hv, eina_list_data_get(elm_slideshow_transitions_get($slideshow)));
    evas_object_show($hv);

    $spin = elm_spinner_add($win);
    elm_spinner_label_format_set($spin, "%2.0f secs.");
    evas_object_smart_callback_add($spin, "changed", \&_spin, \$spin);
    elm_spinner_step_set($spin, 1);
    elm_spinner_min_max_set($spin, 1, 30);
    elm_spinner_value_set($spin, 3);
    elm_box_pack_end($bx, $spin);
    evas_object_show($spin);

    $bt       = elm_button_add($win);
    $bt_start = $bt;
    elm_button_label_set($bt, "Start");
    evas_object_smart_callback_add($bt, "clicked", \&_start, \$spin);
    elm_box_pack_end($bx, $bt);
    evas_object_show($bt);

    $bt      = elm_button_add($win);
    $bt_stop = $bt;
    elm_button_label_set($bt, "Stop");
    evas_object_smart_callback_add($bt, "clicked", \&_stop, \$spin);
    elm_box_pack_end($bx, $bt);
    elm_object_disabled_set($bt, 1);
    evas_object_show($bt);

    evas_object_event_callback_add($slideshow, EVAS_CALLBACK_MOUSE_UP,   \&_notify_show, \$notify);
    evas_object_event_callback_add($slideshow, EVAS_CALLBACK_MOUSE_MOVE, \&_notify_show, \$notify);

    evas_object_resize($win, 350, 200);
    evas_object_show($win);
}

1;
