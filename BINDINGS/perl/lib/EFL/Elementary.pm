package EFL::Elementary;

# ABSTRACT: Perl bindings for Elementary from the Enlightenment Foundation Libraries

use 5.10.0;

use strict;
use warnings;

our $VERSION    = '0.501';
our $XS_VERSION = $VERSION;

use Carp;

use Sub::Exporter;

sub can {
    my ($class, $name) = @_;

    return \&{$class . '::' . $name} if (defined(&{$name}));

    return if ($name eq 'constant');
    my ($error, $val) = constant($name);

    return if ($error);
    my $sub = sub () { $val };

    {
        no strict 'refs';    ## no critic
        *{$class . '::' . $name} = $sub;
    }

    return $sub;
}

our @__constants = qw(
  ELM_FLIP_ROTATE_X_CENTER_AXIS
  ELM_FLIP_ROTATE_Y_CENTER_AXIS
  ELM_FLIP_ROTATE_XZ_CENTER_AXIS
  ELM_FLIP_ROTATE_YZ_CENTER_AXIS
  ELM_GENLIST_ITEM_NONE
  ELM_GENLIST_ITEM_SUBITEMS
  ELM_HOVER_AXIS_BOTH
  ELM_HOVER_AXIS_HORIZONTAL
  ELM_HOVER_AXIS_NONE
  ELM_HOVER_AXIS_VERTICAL
  ELM_ICON_FILE
  ELM_ICON_NONE
  ELM_ICON_STANDARD
  ELM_IMAGE_FLIP_HORIZONTAL
  ELM_IMAGE_FLIP_TRANSPOSE
  ELM_IMAGE_FLIP_TRANSVERSE
  ELM_IMAGE_FLIP_VERTICAL
  ELM_IMAGE_ORIENT_NONE
  ELM_IMAGE_ROTATE_180_CW
  ELM_IMAGE_ROTATE_90_CCW
  ELM_IMAGE_ROTATE_90_CW
  ELM_LIST_COMPRESS
  ELM_LIST_LIMIT
  ELM_LIST_SCROLL
  ELM_MAP_ZOOM_MODE_AUTO_FILL
  ELM_MAP_ZOOM_MODE_AUTO_FIT
  ELM_MAP_ZOOM_MODE_MANUAL
  ELM_NOTIFY_ORIENT_BOTTOM
  ELM_NOTIFY_ORIENT_BOTTOM_LEFT
  ELM_NOTIFY_ORIENT_BOTTOM_RIGHT
  ELM_NOTIFY_ORIENT_LEFT
  ELM_NOTIFY_ORIENT_RIGHT
  ELM_NOTIFY_ORIENT_TOP
  ELM_NOTIFY_ORIENT_TOP_LEFT
  ELM_NOTIFY_ORIENT_TOP_RIGHT
  ELM_PANEL_ORIENT_BOTTOM
  ELM_PANEL_ORIENT_LEFT
  ELM_PANEL_ORIENT_RIGHT
  ELM_PANEL_ORIENT_TOP
  ELM_PHOTOCAM_ZOOM_MODE_AUTO_FILL
  ELM_PHOTOCAM_ZOOM_MODE_AUTO_FIT
  ELM_PHOTOCAM_ZOOM_MODE_MANUAL
  ELM_PI
  ELM_POLICY_LAST
  ELM_POLICY_QUIT
  ELM_POLICY_QUIT_LAST_WINDOW_CLOSED
  ELM_POLICY_QUIT_NONE
  ELM_SCROLLER_POLICY_AUTO
  ELM_SCROLLER_POLICY_OFF
  ELM_SCROLLER_POLICY_ON
  ELM_TEXT_FORMAT_MARKUP_UTF8
  ELM_TEXT_FORMAT_PLAIN_UTF8
  ELM_WIN_BASIC
  ELM_WIN_DESKTOP
  ELM_WIN_DIALOG_BASIC
  ELM_WIN_DOCK
  ELM_WIN_KEYBOARD_ALPHA
  ELM_WIN_KEYBOARD_HEX
  ELM_WIN_KEYBOARD_NUMERIC
  ELM_WIN_KEYBOARD_OFF
  ELM_WIN_KEYBOARD_ON
  ELM_WIN_KEYBOARD_PASSWORD
  ELM_WIN_KEYBOARD_PHONE_NUMBER
  ELM_WIN_KEYBOARD_PIN
  ELM_WIN_KEYBOARD_TERMINAL
  ELM_WIN_KEYBOARD_UNKNOWN
  ELM_WIN_MENU
  ELM_WIN_SPLASH
  ELM_WIN_TOOLBAR
  ELM_WIN_UTILITY
);

my @elm_general = qw(
  elm_init
  elm_run
  elm_exit
  elm_shutdown

  elm_quicklaunch_init
  elm_quicklaunch_sub_init
  elm_quicklaunch_sub_shutdown
  elm_quicklaunch_seed
  elm_quicklaunch_prepare
  elm_quicklaunch_cleanup
  elm_quicklaunch_fallback
  elm_quicklaunch_exe_path_get

  elm_need_efreet
  elm_need_e_dbus

  elm_policy_set
  elm_policy_get

  elm_object_scale_set
  elm_object_scale_get

  elm_object_style_set
  elm_object_style_get

  elm_object_disabled_set
  elm_object_disabled_get

  elm_scale_get
  elm_scale_set

  elm_finger_size_get
  elm_finger_size_set

  elm_object_focus
  elm_object_focus_allow_set
  elm_object_focus_allow_get

  elm_object_scroll_hold_push
  elm_object_scroll_hold_pop
  elm_object_scroll_freeze_push
  elm_object_scroll_freeze_pop

  elm_coords_finger_size_adjust

  elm_theme_overlay_add
  elm_theme_overlay_del
  elm_theme_extension_add
  elm_theme_extension_del
  elm_theme_flush
);

my @elm_win = qw(
  elm_win_add
  elm_win_resize_object_add
  elm_win_resize_object_del
  elm_win_title_set
  elm_win_autodel_set
  elm_win_activate
  elm_win_lower
  elm_win_raise
  elm_win_borderless_set
  elm_win_borderless_get
  elm_win_shaped_set
  elm_win_shaped_get
  elm_win_alpha_set
  elm_win_alpha_get
  elm_win_override_set
  elm_win_override_get
  elm_win_fullscreen_set
  elm_win_fullscreen_get
  elm_win_maximized_set
  elm_win_maximized_get
  elm_win_iconified_set
  elm_win_iconified_get
  elm_win_layer_set
  elm_win_layer_get
  elm_win_rotation_set
  elm_win_rotation_get
  elm_win_sticky_set
  elm_win_sticky_get
  elm_win_keyboard_mode_set
  elm_win_keyboard_win_set
  elm_win_inwin_add
  elm_win_inwin_activate
  elm_win_inwin_content_set
);

my @elm_bg = qw(
  elm_bg_add
  elm_bg_file_set
);

my @elm_icon = qw(
  elm_icon_add
  elm_icon_file_set
  elm_icon_standard_set
  elm_icon_smooth_set
  elm_icon_no_scale_set
  elm_icon_scale_set
  elm_icon_fill_outside_set
  elm_icon_prescale_set
);

my @elm_image = qw(
  elm_image_add
  elm_image_file_set
  elm_image_smooth_set
  elm_image_no_scale_set
  elm_image_object_size_get
  elm_image_scale_set
  elm_image_fill_outside_set
  elm_image_prescale_set
  elm_image_orient_set
);

my @elm_box = qw(
  elm_box_add
  elm_box_horizontal_set
  elm_box_homogeneous_set
  elm_box_homogenous_set
  elm_box_pack_start
  elm_box_pack_end
  elm_box_pack_before
  elm_box_pack_after
  elm_box_clear
  elm_box_unpack
  elm_box_unpack_all
);

my @elm_button = qw(
  elm_button_add
  elm_button_label_set
  elm_button_label_get
  elm_button_icon_set
  elm_button_icon_get
);

my @elm_scroller = qw(
  elm_scroller_add
  elm_scroller_content_set
  elm_scroller_content_min_limit
  elm_scroller_region_show
  elm_scroller_policy_set
  elm_scroller_region_get
  elm_scroller_child_size_get
  elm_scroller_bounce_set
  elm_scroller_page_relative_set
  elm_scroller_page_size_set
  elm_scroller_region_bring_in
);

my @elm_label = qw(
  elm_label_add
  elm_label_label_set
  elm_label_line_wrap_set
);

my @elm_toggle = qw(
  elm_toggle_add
  elm_toggle_label_set
  elm_toggle_label_get
  elm_toggle_icon_set
  elm_toggle_icon_get
  elm_toggle_states_labels_set
  elm_toggle_state_set
  elm_toggle_state_get
  elm_toggle_state_pointer_set
);

my @elm_frame = qw(
  elm_frame_add
  elm_frame_label_set
  elm_frame_label_get
  elm_frame_content_set
);

my @elm_table = qw(
  elm_table_add
  elm_table_homogeneous_set
  elm_table_homogenous_set
  elm_table_pack
  elm_table_padding_set
);

my @elm_clock = qw(
  elm_clock_add
  elm_clock_time_set
  elm_clock_time_get
  elm_clock_edit_set
  elm_clock_show_am_pm_set
  elm_clock_show_seconds_set
);

my @elm_layout = qw(
  elm_layout_add
  elm_layout_file_set
  elm_layout_content_set
  elm_layout_edje_get
  elm_layout_sizing_eval
);

my @elm_notify = qw(
  elm_notify_add
  elm_notify_content_set
  elm_notify_orient_set
  elm_notify_timeout_set
  elm_notify_timer_init
  elm_notify_repeat_events_set
  elm_notify_parent_set
);

my @elm_hover = qw(
  elm_hover_add
  elm_hover_target_set
  elm_hover_parent_set
  elm_hover_content_set
  elm_hover_best_content_location_get
);

my @elm_entry = qw(
  elm_entry_add
  elm_entry_single_line_set
  elm_entry_password_set
  elm_entry_entry_set
  elm_entry_entry_get
  elm_entry_selection_get
  elm_entry_entry_insert
  elm_entry_line_wrap_set
  elm_entry_line_char_wrap_set
  elm_entry_editable_set
  elm_entry_select_none
  elm_entry_select_all
  elm_entry_cursor_next
  elm_entry_cursor_prev
  elm_entry_cursor_up
  elm_entry_cursor_down
  elm_entry_cursor_begin_set
  elm_entry_cursor_end_set
  elm_entry_cursor_selection_begin
  elm_entry_cursor_selection_end
  elm_entry_cursor_is_format_get
  elm_entry_cursor_is_visible_format_get
  elm_entry_cursor_content_get
  elm_entry_selection_cut
  elm_entry_selection_copy
  elm_entry_selection_paste
  elm_entry_context_menu_clear
  elm_entry_context_menu_item_add
  elm_entry_context_menu_disabled_set
  elm_entry_context_menu_disabled_get
  elm_entry_markup_to_utf8
  elm_entry_utf8_to_markup
);

my @elm_notepad = qw(
  elm_notepad_add
  elm_notepad_file_set
);

my @elm_anchorview = qw(
  elm_anchorview_add
  elm_anchorview_text_set
  elm_anchorview_hover_parent_set
  elm_anchorview_hover_style_set
  elm_anchorview_hover_end
);

my @elm_anchorblock = qw(
  elm_anchorblock_add
  elm_anchorblock_text_set
  elm_anchorblock_hover_parent_set
  elm_anchorblock_hover_style_set
  elm_anchorblock_hover_end
);

my @elm_bubble = qw(
  elm_bubble_add
  elm_bubble_label_set
  elm_bubble_label_get
  elm_bubble_info_set
  elm_bubble_content_set
  elm_bubble_icon_set
  elm_bubble_icon_get
  elm_bubble_corner_set
);

my @elm_photo = qw(
  elm_photo_add
  elm_photo_file_set
  elm_photo_size_set
);

my @elm_hoversel = qw(
  elm_hoversel_add
  elm_hoversel_hover_parent_set
  elm_hoversel_label_set
  elm_hoversel_label_get
  elm_hoversel_icon_set
  elm_hoversel_icon_get
  elm_hoversel_hover_begin
  elm_hoversel_hover_end
  elm_hoversel_clear
  elm_hoversel_item_add
  elm_hoversel_item_del
  elm_hoversel_item_data_get
  elm_hoversel_item_label_get
  elm_hoversel_item_icon_set
  elm_hoversel_item_icon_get
);

my @elm_toolbar = qw(
  elm_toolbar_add
  elm_toolbar_icon_size_set
  elm_toolbar_align_set
  elm_toolbar_icon_size_get
  elm_toolbar_item_unselect_all
  elm_toolbar_item_add
  elm_toolbar_item_icon_get
  elm_toolbar_item_label_get
  elm_toolbar_item_label_set
  elm_toolbar_item_del
  elm_toolbar_item_select
  elm_toolbar_item_disabled_get
  elm_toolbar_item_disabled_set
  elm_toolbar_item_separator_set
  elm_toolbar_item_separator_get
  elm_toolbar_item_menu_set
  elm_toolbar_item_menu_get
  elm_toolbar_menu_parent_set
  elm_toolbar_scrollable_set
  elm_toolbar_homogeneous_set
  elm_toolbar_homogenous_set
);

my @elm_menu = qw(
  elm_menu_add
  elm_menu_move
  elm_menu_parent_set
  elm_menu_item_add
  elm_menu_item_separator_add
  elm_menu_item_label_set
  elm_menu_item_label_get
  elm_menu_item_icon_set
  elm_menu_item_disabled_set
  elm_menu_item_del
  elm_menu_object_get
);

my @elm_list = qw(
  elm_list_add
  elm_list_item_append
  elm_list_selected_item_get
  elm_list_clear
  elm_list_go
  elm_list_multi_select_set
  elm_list_horizontal_mode_set
  elm_list_always_select_mode_set
  elm_list_items_get
  elm_list_item_selected_set
  elm_list_item_show
  elm_list_item_del
  elm_list_item_data_get
  elm_list_item_icon_get
  elm_list_item_icon_set
  elm_list_item_end_get
  elm_list_item_end_set
  elm_list_item_object_get
  elm_list_item_label_get
  elm_list_item_label_set
  elm_list_item_prev
  elm_list_item_next
);

my @elm_slider = qw(
  elm_slider_add
  elm_slider_label_set
  elm_slider_label_get
  elm_slider_icon_set
  elm_slider_icon_get
  elm_slider_span_size_set
  elm_slider_unit_format_set
  elm_slider_indicator_format_set
  elm_slider_horizontal_set
  elm_slider_min_max_set
  elm_slider_value_set
  elm_slider_value_get
  elm_slider_inverted_set
);

my @elm_genlist = qw(
  elm_genlist_add
  elm_genlist_item_append

);

my @elm_check = qw(
  elm_check_add
  elm_check_label_set
  elm_check_label_get
  elm_check_icon_set
  elm_check_icon_get
  elm_check_state_set
  elm_check_state_get
);

my @elm_radio = qw(
  elm_radio_add
  elm_radio_label_set
  elm_radio_label_get
  elm_radio_icon_set
  elm_radio_icon_get
  elm_radio_group_add
  elm_radio_state_value_set
  elm_radio_value_set
  elm_radio_value_get

);

my @elm_pager = qw(
  elm_pager_add
  elm_pager_content_push
  elm_pager_content_pop
  elm_pager_content_promote
  elm_pager_content_bottom_get
  elm_pager_content_top_get
);

my @elm_slideshow = qw(
  elm_slideshow_add
  elm_slideshow_item_add
  elm_slideshow_show
  elm_slideshow_next
  elm_slideshow_previous
  elm_slideshow_transitions_get

  elm_slideshow_timeout_set
  elm_slideshow_timeout_get
  elm_slideshow_loop_set
  elm_slideshow_clear
  elm_slideshow_items_get
  elm_slideshow_item_del
  elm_slideshow_item_current_get
  elm_slideshow_item_object_get
);

my @elm_fileselector = qw(
  elm_fileselector_add
  elm_fileselector_is_save_set
  elm_fileselector_is_save_get
  elm_fileselector_folder_only_set

  elm_fileselector_buttons_ok_cancel_set
  elm_fileselector_expandable_set
  elm_fileselector_path_set
  elm_fileselector_path_get
  elm_fileselector_selected_get
);

my @elm_progressbar = qw(
  elm_progressbar_add
  elm_progressbar_label_set
  elm_progressbar_icon_set
  elm_progressbar_span_size_set
  elm_progressbar_horizontal_set
  elm_progressbar_inverted_set
  elm_progressbar_pulse_set
  elm_progressbar_pulse
  elm_progressbar_unit_format_set
  elm_progressbar_value_set
  elm_progressbar_value_get
);

my @elm_separator = qw(
  elm_separator_add
  elm_separator_horizontal_set
  elm_separator_horizontal_get
);

my @elm_spinner = qw(
  elm_spinner_add
  elm_spinner_label_format_set
  elm_spinner_label_format_get
  elm_spinner_min_max_set
  elm_spinner_step_set
  elm_spinner_value_set
  elm_spinner_value_get
  elm_spinner_wrap_set
);

my @elm_index = qw(
  elm_index_add
  elm_index_active_set
  elm_index_item_level_set
  elm_index_item_level_get
  elm_index_item_selected_get
  elm_index_item_append
  elm_index_item_prepend
  elm_index_item_append_relative
  elm_index_item_prepend_relative
  elm_index_item_del
  elm_index_item_clear
  elm_index_item_go
);

my @elm_photocam = qw(
  elm_photocam_add
  elm_photocam_file_set
  elm_photocam_file_get
  elm_photocam_zoom_set
  elm_photocam_zoom_get
  elm_photocam_zoom_mode_set
  elm_photocam_zoom_mode_get
  elm_photocam_image_size_get
  elm_photocam_region_get
  elm_photocam_image_region_show
  elm_photocam_image_region_bring_in
  elm_photocam_paused_get
  elm_photocam_paused_set
);

my @elm_map = qw(
  elm_map_add
  elm_map_zoom_set
  elm_map_zoom_get
  elm_map_zoom_mode_get
  elm_map_zoom_mode_set
  elm_map_geo_region_get
  elm_map_geo_region_bring_in
  elm_map_geo_region_show
  elm_map_paused_set
  elm_map_paused_get
  elm_map_paused_markers_set
  elm_map_paused_markers_get
  elm_map_utils_convert_coord_into_geo
  elm_map_utils_convert_geo_into_coord
  elm_map_marker_add
  elm_map_max_marker_per_group_set
  elm_map_marker_remove
  elm_map_marker_bring_in
  elm_map_marker_show
  elm_map_markers_list_show
  elm_map_marker_object_get
  elm_map_marker_update
  elm_map_bubbles_close
);

my @elm_panel = qw(
  elm_panel_add
  elm_panel_orient_set
  elm_panel_content_set
);

my @elm_flip = qw(
  elm_flip_add
  elm_flip_content_front_set
  elm_flip_content_back_set
  elm_flip_front_get
  elm_flip_perspective_set
  elm_flip_go
);

#elm_scrolled_entry_context_menu_item_add(Evas_Object *obj, const char *label, const char *icon_file, Elm_Icon_Type icon_type, void (*func) (void *data, Evas_Object *obj, void *event_info), const void *data);
my @elm_scrolled = qw(
  elm_scrolled_entry_add
  elm_scrolled_entry_single_line_set
  elm_scrolled_entry_password_set
  elm_scrolled_entry_entry_set
  elm_scrolled_entry_entry_get
  elm_scrolled_entry_selection_get
  elm_scrolled_entry_entry_insert
  elm_scrolled_entry_line_wrap_set
  elm_scrolled_entry_line_char_wrap_set
  elm_scrolled_entry_editable_set
  elm_scrolled_entry_select_none
  elm_scrolled_entry_select_all
  elm_scrolled_entry_cursor_next
  elm_scrolled_entry_cursor_prev
  elm_scrolled_entry_cursor_up
  elm_scrolled_entry_cursor_down
  elm_scrolled_entry_cursor_begin_set
  elm_scrolled_entry_cursor_end_set
  elm_scrolled_entry_cursor_line_begin_set
  elm_scrolled_entry_cursor_line_end_set
  elm_scrolled_entry_cursor_selection_begin
  elm_scrolled_entry_cursor_selection_end
  elm_scrolled_entry_cursor_is_format_get
  elm_scrolled_entry_cursor_is_visible_format_get
  elm_scrolled_entry_cursor_content_get
  elm_scrolled_entry_selection_cut
  elm_scrolled_entry_selection_copy
  elm_scrolled_entry_selection_paste
  elm_scrolled_entry_context_menu_clear
  elm_scrolled_entry_context_menu_disabled_set
  elm_scrolled_entry_context_menu_disabled_get
  elm_scrolled_entry_scrollbar_policy_set
);

our @__funcs = (
    @elm_general,   @elm_win,          @elm_bg,          @elm_icon,      @elm_box,      @elm_toggle, @elm_frame,    @elm_label,      @elm_list,
    @elm_table,     @elm_button,       @elm_clock,       @elm_layout,    @elm_hover,    @elm_entry,  @elm_scroller, @elm_anchorview, @elm_notepad,
    @elm_notify,    @elm_anchorblock,  @elm_bubble,      @elm_photo,     @elm_toolbar,  @elm_menu,   @elm_slider,   @elm_check,      @elm_radio,
    @elm_slideshow, @elm_fileselector, @elm_progressbar, @elm_separator, @elm_spinner,  @elm_index,  @elm_photocam, @elm_map,        @elm_panel,
    @elm_flip,      @elm_scrolled,     @elm_pager,       @elm_image,     @elm_hoversel, @elm_genlist
);

my @__todo = qw(
  elm_check_state_pointer_set
  elm_fileselector_buttons_ok_cancel_get
  elm_fileselector_folder_only_get
  elm_genlist_always_select_mode_set
  elm_genlist_at_xy_item_get
  elm_genlist_bounce_set
  elm_genlist_clear
  elm_genlist_compress_mode_set
  elm_genlist_first_item_get
  elm_genlist_horizontal_mode_set
  elm_genlist_item_bring_in
  elm_genlist_item_data_get
  elm_genlist_item_data_set
  elm_genlist_item_del
  elm_genlist_item_disabled_get
  elm_genlist_item_disabled_set
  elm_genlist_item_expanded_get
  elm_genlist_item_expanded_set
  elm_genlist_item_genlist_get
  elm_genlist_item_insert_after
  elm_genlist_item_insert_before
  elm_genlist_item_middle_bring_in
  elm_genlist_item_middle_show
  elm_genlist_item_next_get
  elm_genlist_item_object_get
  elm_genlist_item_parent_get
  elm_genlist_item_prepend
  elm_genlist_item_prev_get
  elm_genlist_item_selected_get
  elm_genlist_item_selected_set
  elm_genlist_item_show
  elm_genlist_item_subitems_clear
  elm_genlist_item_top_bring_in
  elm_genlist_item_top_show
  elm_genlist_item_update
  elm_genlist_last_item_get
  elm_genlist_multi_select_set
  elm_genlist_no_select_mode_set
  elm_genlist_selected_item_get
  elm_genlist_selected_items_get
  elm_hoversel_items_get
  elm_list_item_del_cb_set
  elm_list_item_insert_after
  elm_list_item_insert_before
  elm_list_item_prepend
  elm_notify_timer
  elm_quicklaunch_fork
  elm_radio_value_pointer_set
  elm_slideshow_transitions_set
  elm_win_screen_position_get
);

Sub::Exporter::setup_exporter(
    {
        'exports' => [ @__constants, @__funcs ],
        'groups'  => {
            'funcs'        => \@__funcs,
            'constants'    => \@__constants,
            'anchorblock'  => \@elm_anchorblock,
            'anchorview'   => \@elm_anchorview,
            'bg'           => \@elm_bg,
            'box'          => \@elm_box,
            'bubble'       => \@elm_bubble,
            'button'       => \@elm_button,
            'check'        => \@elm_check,
            'clock'        => \@elm_clock,
            'entry'        => \@elm_entry,
            'fileselector' => \@elm_fileselector,
            'flip'         => \@elm_flip,
            'frame'        => \@elm_frame,
            'general'      => \@elm_general,
            'genlist'      => \@elm_genlist,
            'hover'        => \@elm_hover,
            'hoversel'     => \@elm_hoversel,
            'icon'         => \@elm_icon,
            'image'        => \@elm_image,
            'index'        => \@elm_index,
            'label'        => \@elm_label,
            'layout'       => \@elm_layout,
            'list'         => \@elm_list,
            'map'          => \@elm_map,
            'menu'         => \@elm_menu,
            'notepad'      => \@elm_notepad,
            'notify'       => \@elm_notify,
            'pager'        => \@elm_pager,
            'pager'        => \@elm_slideshow,
            'panel'        => \@elm_panel,
            'photo'        => \@elm_photo,
            'photocam'     => \@elm_photocam,
            'progressbar'  => \@elm_progressbar,
            'radio'        => \@elm_radio,
            'scrolled'     => \@elm_scrolled,
            'scroller'     => \@elm_scroller,
            'separator'    => \@elm_separator,
            'slider'       => \@elm_slider,
            'spinner'      => \@elm_spinner,
            'table'        => \@elm_table,
            'toggle'       => \@elm_toggle,
            'toolbar'      => \@elm_toolbar,
            'win'          => \@elm_win,
        },
    }
);

sub elm_init {
    my (@args) = @_;

    my $ptrs = pack('P' x @args, @args);

    return _elm_init(scalar(@args), $ptrs);
}

sub elm_quicklaunch_init {
    my (@args) = @_;

    my $ptrs = pack('P' x @args, @args);

    return _elm_quicklaunch_init(scalar(@args), $ptrs);
}

sub elm_quicklaunch_sub_init {
    my (@args) = @_;

    my $ptrs = pack('P' x @args, @args);

    return _elm_quicklaunch_sub_init(scalar(@args), $ptrs);
}

sub elm_quicklaunch_prepare {
    my (@args) = @_;

    my $ptrs = pack('P' x @args, @args);

    return _elm_quicklaunch_prepare(scalar(@args), $ptrs);
}

sub elm_quicklaunch_fallback {
    my (@args) = @_;

    my $ptrs = pack('P' x @args, @args);

    return _elm_quicklaunch_fallback(scalar(@args), $ptrs);
}

require XSLoader;
XSLoader::load('EFL::Elementary', $XS_VERSION);

1;


__END__
=pod

=head1 NAME

EFL::Elementary - Perl bindings for Elementary from the Enlightenment Foundation Libraries

=head1 VERSION

version 0.501

=head1 SYNOPSIS

    use EFL::Elementary qw(:all);
    use EFL::Evas qw(:all);
    
    elm_init();
    my $win = elm_win_add(undef, "main", ELM_WIN_BASIC);
    
    elm_win_title_set($win, "my first window");
    
    evas_object_smart_callback_add($win, "delete,request", sub { elm_exit() }, undef);
    
    my $bg = elm_bg_add($win);
    evas_object_size_hint_weight_set($bg, 1.0, 1.0);
    elm_win_resize_object_add($win, $bg);
    evas_object_show($bg);
    
    evas_object_resize($win, 100, 100);
    evas_object_show($win);
    
    elm_run();
    
    elm_exit();

=head1 DESCRIPTION

Perl bindings for the Enlightenment Foundation Libraries (EFL) Elementary
library.

Visual examples at L<http://trac.enlightenment.org/e/wiki/Elementary>.

=head1 WARNING

With regards to passing data between Perl and C, the API is not set in stone and
may change in future releases.

=head1 EXPORTED API/CONSTANTS

    Note: this list is autogenerated from F<misc/supported-api.pl>.
    
    ELM_FLIP_ROTATE_X_CENTER_AXIS
    ELM_FLIP_ROTATE_Y_CENTER_AXIS
    ELM_GENLIST_ITEM_NONE
    ELM_GENLIST_ITEM_SUBITEMS
    ELM_HOVER_AXIS_BOTH
    ELM_HOVER_AXIS_HORIZONTAL
    ELM_HOVER_AXIS_NONE
    ELM_HOVER_AXIS_VERTICAL
    ELM_ICON_FILE
    ELM_ICON_NONE
    ELM_ICON_STANDARD
    ELM_IMAGE_FLIP_HORIZONTAL
    ELM_IMAGE_FLIP_TRANSPOSE
    ELM_IMAGE_FLIP_TRANSVERSE
    ELM_IMAGE_FLIP_VERTICAL
    ELM_IMAGE_ORIENT_NONE
    ELM_IMAGE_ROTATE_180_CW
    ELM_IMAGE_ROTATE_90_CCW
    ELM_IMAGE_ROTATE_90_CW
    ELM_LIST_COMPRESS
    ELM_LIST_LIMIT
    ELM_LIST_SCROLL
    ELM_MAP_ZOOM_MODE_AUTO_FILL
    ELM_MAP_ZOOM_MODE_AUTO_FIT
    ELM_MAP_ZOOM_MODE_MANUAL
    ELM_NOTIFY_ORIENT_BOTTOM
    ELM_NOTIFY_ORIENT_BOTTOM_LEFT
    ELM_NOTIFY_ORIENT_BOTTOM_RIGHT
    ELM_NOTIFY_ORIENT_LEFT
    ELM_NOTIFY_ORIENT_RIGHT
    ELM_NOTIFY_ORIENT_TOP
    ELM_NOTIFY_ORIENT_TOP_LEFT
    ELM_NOTIFY_ORIENT_TOP_RIGHT
    ELM_PANEL_ORIENT_BOTTOM
    ELM_PANEL_ORIENT_LEFT
    ELM_PANEL_ORIENT_RIGHT
    ELM_PANEL_ORIENT_TOP
    ELM_PHOTOCAM_ZOOM_MODE_AUTO_FILL
    ELM_PHOTOCAM_ZOOM_MODE_AUTO_FIT
    ELM_PHOTOCAM_ZOOM_MODE_MANUAL
    ELM_POLICY_LAST
    ELM_POLICY_QUIT
    ELM_POLICY_QUIT_LAST_WINDOW_CLOSED
    ELM_POLICY_QUIT_NONE
    ELM_SCROLLER_POLICY_AUTO
    ELM_SCROLLER_POLICY_OFF
    ELM_SCROLLER_POLICY_ON
    ELM_TEXT_FORMAT_MARKUP_UTF8
    ELM_TEXT_FORMAT_PLAIN_UTF8
    ELM_WIN_BASIC
    ELM_WIN_DESKTOP
    ELM_WIN_DIALOG_BASIC
    ELM_WIN_DOCK
    ELM_WIN_KEYBOARD_ALPHA
    ELM_WIN_KEYBOARD_HEX
    ELM_WIN_KEYBOARD_NUMERIC
    ELM_WIN_KEYBOARD_OFF
    ELM_WIN_KEYBOARD_ON
    ELM_WIN_KEYBOARD_PASSWORD
    ELM_WIN_KEYBOARD_PHONE_NUMBER
    ELM_WIN_KEYBOARD_PIN
    ELM_WIN_KEYBOARD_TERMINAL
    ELM_WIN_KEYBOARD_UNKNOWN
    ELM_WIN_MENU
    ELM_WIN_SPLASH
    ELM_WIN_TOOLBAR
    ELM_WIN_UTILITY
    elm_anchorblock_add
    elm_anchorblock_hover_end
    elm_anchorblock_hover_parent_set
    elm_anchorblock_hover_style_set
    elm_anchorblock_text_set
    elm_anchorview_add
    elm_anchorview_hover_end
    elm_anchorview_hover_parent_set
    elm_anchorview_hover_style_set
    elm_anchorview_text_set
    elm_bg_add
    elm_bg_file_set
    elm_box_add
    elm_box_clear
    elm_box_homogeneous_set
    elm_box_homogenous_set
    elm_box_horizontal_set
    elm_box_pack_after
    elm_box_pack_before
    elm_box_pack_end
    elm_box_pack_start
    elm_box_unpack
    elm_box_unpack_all
    elm_bubble_add
    elm_bubble_content_set
    elm_bubble_corner_set
    elm_bubble_icon_get
    elm_bubble_icon_set
    elm_bubble_info_set
    elm_bubble_label_get
    elm_bubble_label_set
    elm_button_add
    elm_button_icon_get
    elm_button_icon_set
    elm_button_label_get
    elm_button_label_set
    elm_check_add
    elm_check_icon_get
    elm_check_icon_set
    elm_check_label_get
    elm_check_label_set
    elm_check_state_get
    elm_check_state_set
    elm_clock_add
    elm_clock_edit_set
    elm_clock_show_am_pm_set
    elm_clock_show_seconds_set
    elm_clock_time_get
    elm_clock_time_set
    elm_coords_finger_size_adjust
    elm_entry_add
    elm_entry_context_menu_clear
    elm_entry_context_menu_disabled_get
    elm_entry_context_menu_disabled_set
    elm_entry_context_menu_item_add
    elm_entry_cursor_begin_set
    elm_entry_cursor_content_get
    elm_entry_cursor_down
    elm_entry_cursor_end_set
    elm_entry_cursor_is_format_get
    elm_entry_cursor_is_visible_format_get
    elm_entry_cursor_next
    elm_entry_cursor_prev
    elm_entry_cursor_selection_begin
    elm_entry_cursor_selection_end
    elm_entry_cursor_up
    elm_entry_editable_set
    elm_entry_entry_get
    elm_entry_entry_insert
    elm_entry_entry_set
    elm_entry_line_char_wrap_set
    elm_entry_line_wrap_set
    elm_entry_markup_to_utf8
    elm_entry_password_set
    elm_entry_select_all
    elm_entry_select_none
    elm_entry_selection_copy
    elm_entry_selection_cut
    elm_entry_selection_get
    elm_entry_selection_paste
    elm_entry_single_line_set
    elm_entry_utf8_to_markup
    elm_exit
    elm_fileselector_add
    elm_fileselector_buttons_ok_cancel_set
    elm_fileselector_expandable_set
    elm_fileselector_folder_only_set
    elm_fileselector_is_save_get
    elm_fileselector_is_save_set
    elm_fileselector_path_get
    elm_fileselector_path_set
    elm_fileselector_selected_get
    elm_finger_size_get
    elm_finger_size_set
    elm_flip_add
    elm_flip_content_back_set
    elm_flip_content_front_set
    elm_flip_front_get
    elm_flip_go
    elm_flip_perspective_set
    elm_frame_add
    elm_frame_content_set
    elm_frame_label_get
    elm_frame_label_set
    elm_genlist_add
    elm_genlist_item_append
    elm_hover_add
    elm_hover_best_content_location_get
    elm_hover_content_set
    elm_hover_parent_set
    elm_hover_target_set
    elm_hoversel_add
    elm_hoversel_clear
    elm_hoversel_hover_begin
    elm_hoversel_hover_end
    elm_hoversel_hover_parent_set
    elm_hoversel_icon_get
    elm_hoversel_icon_set
    elm_hoversel_item_add
    elm_hoversel_item_data_get
    elm_hoversel_item_del
    elm_hoversel_item_icon_get
    elm_hoversel_item_icon_set
    elm_hoversel_item_label_get
    elm_hoversel_label_get
    elm_hoversel_label_set
    elm_icon_add
    elm_icon_file_set
    elm_icon_fill_outside_set
    elm_icon_no_scale_set
    elm_icon_prescale_set
    elm_icon_scale_set
    elm_icon_smooth_set
    elm_icon_standard_set
    elm_image_add
    elm_image_file_set
    elm_image_fill_outside_set
    elm_image_no_scale_set
    elm_image_object_size_get
    elm_image_orient_set
    elm_image_prescale_set
    elm_image_scale_set
    elm_image_smooth_set
    elm_index_active_set
    elm_index_add
    elm_index_item_append
    elm_index_item_append_relative
    elm_index_item_clear
    elm_index_item_del
    elm_index_item_go
    elm_index_item_level_get
    elm_index_item_level_set
    elm_index_item_prepend
    elm_index_item_prepend_relative
    elm_index_item_selected_get
    elm_init
    elm_label_add
    elm_label_label_set
    elm_label_line_wrap_set
    elm_layout_add
    elm_layout_content_set
    elm_layout_edje_get
    elm_layout_file_set
    elm_layout_sizing_eval
    elm_list_add
    elm_list_always_select_mode_set
    elm_list_clear
    elm_list_go
    elm_list_horizontal_mode_set
    elm_list_item_append
    elm_list_item_object_get
    elm_list_item_data_get
    elm_list_item_del
    elm_list_item_end_get
    elm_list_item_end_set
    elm_list_item_icon_get
    elm_list_item_icon_set
    elm_list_item_label_get
    elm_list_item_label_set
    elm_list_item_next
    elm_list_item_prev
    elm_list_item_selected_set
    elm_list_item_show
    elm_list_items_get
    elm_list_multi_select_set
    elm_map_add
    elm_map_bubbles_close
    elm_map_geo_region_bring_in
    elm_map_geo_region_get
    elm_map_geo_region_show
    elm_map_marker_add
    elm_map_marker_bring_in
    elm_map_marker_object_get
    elm_map_marker_remove
    elm_map_marker_show
    elm_map_marker_update
    elm_map_markers_list_show
    elm_map_max_marker_per_group_set
    elm_map_paused_get
    elm_map_paused_markers_get
    elm_map_paused_markers_set
    elm_map_paused_set
    elm_map_utils_convert_coord_into_geo
    elm_map_utils_convert_geo_into_coord
    elm_map_zoom_get
    elm_map_zoom_mode_get
    elm_map_zoom_mode_set
    elm_map_zoom_set
    elm_menu_add
    elm_menu_item_add
    elm_menu_item_del
    elm_menu_item_disabled_set
    elm_menu_item_icon_set
    elm_menu_item_label_get
    elm_menu_item_label_set
    elm_menu_item_separator_add
    elm_menu_move
    elm_menu_object_get
    elm_menu_parent_set
    elm_need_e_dbus
    elm_need_efreet
    elm_notepad_add
    elm_notepad_file_set
    elm_notify_add
    elm_notify_content_set
    elm_notify_orient_set
    elm_notify_parent_set
    elm_notify_repeat_events_set
    elm_notify_timeout_set
    elm_notify_timer_init
    elm_object_disabled_get
    elm_object_disabled_set
    elm_object_focus
    elm_object_focus_allow_get
    elm_object_focus_allow_set
    elm_object_scale_get
    elm_object_scale_set
    elm_object_scroll_freeze_pop
    elm_object_scroll_freeze_push
    elm_object_scroll_hold_pop
    elm_object_scroll_hold_push
    elm_object_style_get
    elm_object_style_set
    elm_pager_add
    elm_pager_content_bottom_get
    elm_pager_content_pop
    elm_pager_content_promote
    elm_pager_content_push
    elm_pager_content_top_get
    elm_panel_add
    elm_panel_content_set
    elm_panel_orient_set
    elm_photo_add
    elm_photo_file_set
    elm_photo_size_set
    elm_photocam_add
    elm_photocam_file_get
    elm_photocam_file_set
    elm_photocam_image_region_bring_in
    elm_photocam_image_region_show
    elm_photocam_image_size_get
    elm_photocam_paused_get
    elm_photocam_paused_set
    elm_photocam_region_get
    elm_photocam_zoom_get
    elm_photocam_zoom_mode_get
    elm_photocam_zoom_mode_set
    elm_photocam_zoom_set
    elm_policy_get
    elm_policy_set
    elm_progressbar_add
    elm_progressbar_horizontal_set
    elm_progressbar_icon_set
    elm_progressbar_inverted_set
    elm_progressbar_label_set
    elm_progressbar_pulse
    elm_progressbar_pulse_set
    elm_progressbar_span_size_set
    elm_progressbar_unit_format_set
    elm_progressbar_value_get
    elm_progressbar_value_set
    elm_quicklaunch_cleanup
    elm_quicklaunch_exe_path_get
    elm_quicklaunch_fallback
    elm_quicklaunch_init
    elm_quicklaunch_prepare
    elm_quicklaunch_seed
    elm_quicklaunch_sub_init
    elm_quicklaunch_sub_shutdown
    elm_radio_add
    elm_radio_group_add
    elm_radio_icon_get
    elm_radio_icon_set
    elm_radio_label_get
    elm_radio_label_set
    elm_radio_state_value_set
    elm_radio_value_get
    elm_radio_value_set
    elm_run
    elm_scale_get
    elm_scale_set
    elm_scrolled_entry_add
    elm_scrolled_entry_context_menu_clear
    elm_scrolled_entry_context_menu_disabled_get
    elm_scrolled_entry_context_menu_disabled_set
    elm_scrolled_entry_cursor_begin_set
    elm_scrolled_entry_cursor_content_get
    elm_scrolled_entry_cursor_down
    elm_scrolled_entry_cursor_end_set
    elm_scrolled_entry_cursor_is_format_get
    elm_scrolled_entry_cursor_is_visible_format_get
    elm_scrolled_entry_cursor_line_begin_set
    elm_scrolled_entry_cursor_line_end_set
    elm_scrolled_entry_cursor_next
    elm_scrolled_entry_cursor_prev
    elm_scrolled_entry_cursor_selection_begin
    elm_scrolled_entry_cursor_selection_end
    elm_scrolled_entry_cursor_up
    elm_scrolled_entry_editable_set
    elm_scrolled_entry_entry_get
    elm_scrolled_entry_entry_insert
    elm_scrolled_entry_entry_set
    elm_scrolled_entry_line_char_wrap_set
    elm_scrolled_entry_line_wrap_set
    elm_scrolled_entry_password_set
    elm_scrolled_entry_scrollbar_policy_set
    elm_scrolled_entry_select_all
    elm_scrolled_entry_select_none
    elm_scrolled_entry_selection_copy
    elm_scrolled_entry_selection_cut
    elm_scrolled_entry_selection_get
    elm_scrolled_entry_selection_paste
    elm_scrolled_entry_single_line_set
    elm_scroller_add
    elm_scroller_bounce_set
    elm_scroller_child_size_get
    elm_scroller_content_min_limit
    elm_scroller_content_set
    elm_scroller_page_relative_set
    elm_scroller_page_size_set
    elm_scroller_policy_set
    elm_scroller_region_bring_in
    elm_scroller_region_get
    elm_scroller_region_show
    elm_separator_add
    elm_separator_horizontal_get
    elm_separator_horizontal_set
    elm_shutdown
    elm_slider_add
    elm_slider_horizontal_set
    elm_slider_icon_get
    elm_slider_icon_set
    elm_slider_indicator_format_set
    elm_slider_inverted_set
    elm_slider_label_get
    elm_slider_label_set
    elm_slider_min_max_set
    elm_slider_span_size_set
    elm_slider_unit_format_set
    elm_slider_value_get
    elm_slider_value_set
    elm_slideshow_add
    elm_slideshow_clear
    elm_slideshow_item_add
    elm_slideshow_item_current_get
    elm_slideshow_item_del
    elm_slideshow_item_object_get
    elm_slideshow_items_get
    elm_slideshow_loop_set
    elm_slideshow_next
    elm_slideshow_previous
    elm_slideshow_show
    elm_slideshow_timeout_get
    elm_slideshow_timeout_set
    elm_slideshow_transitions_get
    elm_spinner_add
    elm_spinner_label_format_get
    elm_spinner_label_format_set
    elm_spinner_min_max_set
    elm_spinner_step_set
    elm_spinner_value_get
    elm_spinner_value_set
    elm_spinner_wrap_set
    elm_table_add
    elm_table_homogeneous_set
    elm_table_homogenous_set
    elm_table_pack
    elm_table_padding_set
    elm_theme_extension_add
    elm_theme_extension_del
    elm_theme_flush
    elm_theme_overlay_add
    elm_theme_overlay_del
    elm_toggle_add
    elm_toggle_icon_get
    elm_toggle_icon_set
    elm_toggle_label_get
    elm_toggle_label_set
    elm_toggle_state_get
    elm_toggle_state_pointer_set
    elm_toggle_state_set
    elm_toggle_states_labels_set
    elm_toolbar_add
    elm_toolbar_align_set
    elm_toolbar_homogeneous_set
    elm_toolbar_homogenous_set
    elm_toolbar_icon_size_get
    elm_toolbar_icon_size_set
    elm_toolbar_item_add
    elm_toolbar_item_del
    elm_toolbar_item_disabled_get
    elm_toolbar_item_disabled_set
    elm_toolbar_item_icon_get
    elm_toolbar_item_label_get
    elm_toolbar_item_label_set
    elm_toolbar_item_menu_get
    elm_toolbar_item_menu_set
    elm_toolbar_item_select
    elm_toolbar_item_separator_get
    elm_toolbar_item_separator_set
    elm_toolbar_item_unselect_all
    elm_toolbar_menu_parent_set
    elm_toolbar_scrollable_set
    elm_win_activate
    elm_win_add
    elm_win_alpha_get
    elm_win_alpha_set
    elm_win_autodel_set
    elm_win_borderless_get
    elm_win_borderless_set
    elm_win_fullscreen_get
    elm_win_fullscreen_set
    elm_win_iconified_get
    elm_win_iconified_set
    elm_win_inwin_activate
    elm_win_inwin_add
    elm_win_inwin_content_set
    elm_win_keyboard_mode_set
    elm_win_keyboard_win_set
    elm_win_layer_get
    elm_win_layer_set
    elm_win_lower
    elm_win_maximized_get
    elm_win_maximized_set
    elm_win_override_get
    elm_win_override_set
    elm_win_raise
    elm_win_resize_object_add
    elm_win_resize_object_del
    elm_win_rotation_get
    elm_win_rotation_set
    elm_win_shaped_get
    elm_win_shaped_set
    elm_win_sticky_get
    elm_win_sticky_set
    elm_win_title_set

=head1 SUPPORT

You can find documentation for this module with the perldoc command.

    perldoc EFL::Elementary

You can also look for information at:

=over

=item * RT: CPAN's request tracker: L<http://rt.cpan.org/NoAuth/Bugs.html?Dist=EFL>

=item * AnnoCPAN: Annotated CPAN documentation: L<http://annocpan.org/dist/EFL>

=item * CPAN Ratings: L<http://cpanratings.perl.org/d/EFL>

=item * Search CPAN: L<http://search.cpan.org/dist/EFL>

=back

=head1 SEE ALSO

=over

=item * Main Enlightenment page: L<http://www.enlightenment.org>

=item * EFL API: L<http://docs.enlightenment.org>

=item * EFL Git Web Repository: L<http://git.npjh.com/?p=libefl-perl.git;a=summary>

=back

=head1 AUTHOR

  Adam Flott <adam@npjh.com>

=head1 COPYRIGHT AND LICENSE

This software is Copyright (c) 2010 by Adam Flott.

This is free software, licensed under:

  The GNU Lesser General Public License, Version 2.1, February 1999

=cut

