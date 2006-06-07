package Etk;

use 5.008007;
use strict;
use warnings;
use Carp;

require Exporter;
use AutoLoader;

our @ISA = qw(Exporter);

# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.

# This allows declaration	use Etk ':all';
# If you do not need this, moving things directly into @EXPORT or @EXPORT_OK
# will save memory.
our %EXPORT_TAGS = ( 'all' => [ qw(
	etk_accumulator_bool_and
	etk_accumulator_bool_or
	etk_accumulator_stopping_bool_and
	etk_accumulator_stopping_bool_or
	etk_alignment_get
	etk_alignment_new
	etk_alignment_set
	etk_alignment_type_get
	etk_argument_extra_find
	etk_argument_help_show
	etk_argument_is_set
	etk_arguments_parse
	etk_bin_child_get
	etk_bin_child_set
	etk_bin_type_get
	etk_box_child_packing_get
	etk_box_child_packing_set
	etk_box_homogeneous_get
	etk_box_homogeneous_set
	etk_box_pack_end
	etk_box_pack_start
	etk_box_spacing_get
	etk_box_spacing_set
	etk_box_type_get
	etk_button_alignment_get
	etk_button_alignment_set
	etk_button_click
	etk_button_image_get
	etk_button_image_set
	etk_button_label_get
	etk_button_label_set
	etk_button_new
	etk_button_new_from_stock
	etk_button_new_with_label
	etk_button_press
	etk_button_release
	etk_button_set_from_stock
	etk_button_type_get
	etk_cache_edje_object_add
	etk_cache_edje_object_add_with_state
	etk_cache_edje_object_find
	etk_cache_edje_object_find_with_state
	etk_cache_edje_object_remove
	etk_cache_edje_object_size_get
	etk_cache_edje_object_size_set
	etk_cache_empty
	etk_cache_image_object_add
	etk_cache_image_object_find
	etk_cache_image_object_remove
	etk_cache_image_object_size_get
	etk_cache_image_object_size_set
	etk_cache_shutdown
	etk_canvas_new
	etk_canvas_object_add
	etk_canvas_object_remove
	etk_canvas_type_get
	etk_check_button_new
	etk_check_button_new_with_label
	etk_check_button_type_get
	etk_clipboard_text_request
	etk_clipboard_text_set
	etk_colorpicker_current_color_get
	etk_colorpicker_current_color_set
	etk_colorpicker_mode_get
	etk_colorpicker_mode_set
	etk_colorpicker_new
	etk_colorpicker_type_get
	etk_combobox_active_item_get
	etk_combobox_active_item_set
	etk_combobox_build
	etk_combobox_clear
	etk_combobox_column_add
	etk_combobox_item_activate
	etk_combobox_item_append
	etk_combobox_item_append_relative
	etk_combobox_item_append_relative_valist
	etk_combobox_item_append_valist
	etk_combobox_item_data_get
	etk_combobox_item_data_set
	etk_combobox_item_data_set_full
	etk_combobox_item_height_get
	etk_combobox_item_height_set
	etk_combobox_item_prepend
	etk_combobox_item_prepend_relative
	etk_combobox_item_prepend_relative_valist
	etk_combobox_item_prepend_valist
	etk_combobox_item_remove
	etk_combobox_item_type_get
	etk_combobox_new
	etk_combobox_new_default
	etk_combobox_nth_item_get
	etk_combobox_type_get
	etk_container_add
	etk_container_border_width_get
	etk_container_border_width_set
	etk_container_child_space_fill
	etk_container_children_get
	etk_container_for_each
	etk_container_for_each_data
	etk_container_is_child
	etk_container_remove
	etk_container_type_get
	etk_dialog_button_add
	etk_dialog_button_add_from_stock
	etk_dialog_has_separator_get
	etk_dialog_has_separator_set
	etk_dialog_new
	etk_dialog_pack_button_in_action_area
	etk_dialog_pack_in_main_area
	etk_dialog_pack_widget_in_action_area
	etk_dialog_type_get
	etk_dnd_init
	etk_dnd_shutdown
	etk_drag_begin
	etk_drag_data_set
	etk_drag_new
	etk_drag_parent_widget_get
	etk_drag_parent_widget_set
	etk_drag_type_get
	etk_drag_types_set
	etk_editable_text_object_add
	etk_editable_text_object_cursor_hide
	etk_editable_text_object_cursor_move_at_end
	etk_editable_text_object_cursor_move_at_start
	etk_editable_text_object_cursor_move_left
	etk_editable_text_object_cursor_move_right
	etk_editable_text_object_cursor_show
	etk_editable_text_object_delete_char_after
	etk_editable_text_object_delete_char_before
	etk_editable_text_object_insert
	etk_editable_text_object_text_get
	etk_editable_text_object_text_set
	etk_entry_new
	etk_entry_password_get
	etk_entry_password_set
	etk_entry_text_get
	etk_entry_text_set
	etk_entry_type_get
	etk_filechooser_widget_current_folder_get
	etk_filechooser_widget_current_folder_set
	etk_filechooser_widget_new
	etk_filechooser_widget_select_multiple_get
	etk_filechooser_widget_select_multiple_set
	etk_filechooser_widget_selected_file_get
	etk_filechooser_widget_selected_files_get
	etk_filechooser_widget_show_hidden_get
	etk_filechooser_widget_show_hidden_set
	etk_filechooser_widget_type_get
	etk_frame_label_get
	etk_frame_label_set
	etk_frame_new
	etk_frame_type_get
	etk_hbox_new
	etk_hbox_type_get
	etk_hpaned_new
	etk_hpaned_type_get
	etk_hscrollbar_new
	etk_hscrollbar_type_get
	etk_hseparator_new
	etk_hseparator_type_get
	etk_hslider_new
	etk_hslider_type_get
	etk_iconbox_append
	etk_iconbox_clear
	etk_iconbox_current_model_get
	etk_iconbox_current_model_set
	etk_iconbox_freeze
	etk_iconbox_icon_data_get
	etk_iconbox_icon_data_set
	etk_iconbox_icon_data_set_full
	etk_iconbox_icon_del
	etk_iconbox_icon_file_get
	etk_iconbox_icon_file_set
	etk_iconbox_icon_get_at_xy
	etk_iconbox_icon_label_get
	etk_iconbox_icon_label_set
	etk_iconbox_icon_select
	etk_iconbox_icon_unselect
	etk_iconbox_is_selected
	etk_iconbox_model_free
	etk_iconbox_model_geometry_get
	etk_iconbox_model_geometry_set
	etk_iconbox_model_icon_geometry_get
	etk_iconbox_model_icon_geometry_set
	etk_iconbox_model_label_geometry_get
	etk_iconbox_model_label_geometry_set
	etk_iconbox_model_new
	etk_iconbox_new
	etk_iconbox_select_all
	etk_iconbox_thaw
	etk_iconbox_type_get
	etk_iconbox_unselect_all
	etk_image_copy
	etk_image_edje_get
	etk_image_file_get
	etk_image_keep_aspect_get
	etk_image_keep_aspect_set
	etk_image_new
	etk_image_new_from_edje
	etk_image_new_from_file
	etk_image_new_from_stock
	etk_image_set_from_edje
	etk_image_set_from_file
	etk_image_set_from_stock
	etk_image_size_get
	etk_image_stock_get
	etk_image_type_get
	etk_init
	etk_label_alignment_get
	etk_label_alignment_set
	etk_label_get
	etk_label_new
	etk_label_set
	etk_label_type_get
	etk_main
	etk_main_iterate
	etk_main_iteration_queue
	etk_main_quit
	etk_main_toplevel_widget_add
	etk_main_toplevel_widget_remove
	etk_main_toplevel_widgets_get
	etk_marshaller_BOOL__DOUBLE
	etk_marshaller_BOOL__POINTER_POINTER
	etk_marshaller_BOOL__VOID
	etk_marshaller_VOID__DOUBLE
	etk_marshaller_VOID__INT
	etk_marshaller_VOID__INT_POINTER
	etk_marshaller_VOID__POINTER
	etk_marshaller_VOID__POINTER_POINTER
	etk_marshaller_VOID__VOID
	etk_menu_bar_new
	etk_menu_bar_type_get
	etk_menu_item_activate
	etk_menu_item_check_active_get
	etk_menu_item_check_active_set
	etk_menu_item_check_new
	etk_menu_item_check_new_with_label
	etk_menu_item_check_type_get
	etk_menu_item_deselect
	etk_menu_item_image_new
	etk_menu_item_image_new_from_stock
	etk_menu_item_image_new_with_label
	etk_menu_item_image_set
	etk_menu_item_image_type_get
	etk_menu_item_label_get
	etk_menu_item_label_set
	etk_menu_item_new
	etk_menu_item_new_from_stock
	etk_menu_item_new_with_label
	etk_menu_item_radio_group_get
	etk_menu_item_radio_group_set
	etk_menu_item_radio_new
	etk_menu_item_radio_new_from_widget
	etk_menu_item_radio_new_with_label
	etk_menu_item_radio_new_with_label_from_widget
	etk_menu_item_radio_type_get
	etk_menu_item_select
	etk_menu_item_separator_new
	etk_menu_item_separator_type_get
	etk_menu_item_set_from_stock
	etk_menu_item_submenu_set
	etk_menu_item_type_get
	etk_menu_new
	etk_menu_popdown
	etk_menu_popup
	etk_menu_popup_at_xy
	etk_menu_shell_append
	etk_menu_shell_append_relative
	etk_menu_shell_insert
	etk_menu_shell_items_get
	etk_menu_shell_prepend
	etk_menu_shell_prepend_relative
	etk_menu_shell_remove
	etk_menu_shell_type_get
	etk_menu_type_get
	etk_message_dialog_buttons_get
	etk_message_dialog_buttons_set
	etk_message_dialog_message_type_get
	etk_message_dialog_message_type_set
	etk_message_dialog_new
	etk_message_dialog_text_get
	etk_message_dialog_text_set
	etk_message_dialog_type_get
	etk_notebook_current_page_get
	etk_notebook_current_page_set
	etk_notebook_new
	etk_notebook_num_pages_get
	etk_notebook_page_append
	etk_notebook_page_child_get
	etk_notebook_page_child_set
	etk_notebook_page_index_get
	etk_notebook_page_insert
	etk_notebook_page_next
	etk_notebook_page_prepend
	etk_notebook_page_prev
	etk_notebook_page_remove
	etk_notebook_page_tab_label_get
	etk_notebook_page_tab_label_set
	etk_notebook_page_tab_widget_get
	etk_notebook_page_tab_widget_set
	etk_notebook_type_get
	etk_object_check_cast
	etk_object_data_get
	etk_object_data_set
	etk_object_data_set_full
	etk_object_destroy
	etk_object_destroy_all_objects
	etk_object_new
	etk_object_new_valist
	etk_object_notification_callback_add
	etk_object_notification_callback_remove
	etk_object_notify
	etk_object_object_type_get
	etk_object_properties_get
	etk_object_properties_get_valist
	etk_object_properties_set
	etk_object_properties_set_valist
	etk_object_property_reset
	etk_object_signal_callback_add
	etk_object_signal_callback_remove
	etk_object_signal_callbacks_get
	etk_object_type_get
	etk_object_weak_pointer_add
	etk_object_weak_pointer_remove
	etk_paned_child1_get
	etk_paned_child1_set
	etk_paned_child2_get
	etk_paned_child2_set
	etk_paned_position_get
	etk_paned_position_set
	etk_paned_type_get
	etk_popup_window_focused_window_get
	etk_popup_window_focused_window_set
	etk_popup_window_is_popped_up
	etk_popup_window_popdown
	etk_popup_window_popdown_all
	etk_popup_window_popup
	etk_popup_window_popup_at_xy
	etk_popup_window_type_get
	etk_progress_bar_fraction_get
	etk_progress_bar_fraction_set
	etk_progress_bar_new
	etk_progress_bar_new_with_text
	etk_progress_bar_pulse
	etk_progress_bar_pulse_step_get
	etk_progress_bar_pulse_step_set
	etk_progress_bar_text_get
	etk_progress_bar_text_set
	etk_progress_bar_type_get
	etk_property_default_value_set
	etk_property_delete
	etk_property_new
	etk_property_type_get
	etk_property_value_bool
	etk_property_value_bool_get
	etk_property_value_bool_set
	etk_property_value_char
	etk_property_value_char_get
	etk_property_value_char_set
	etk_property_value_create
	etk_property_value_create_valist
	etk_property_value_delete
	etk_property_value_double
	etk_property_value_double_get
	etk_property_value_double_set
	etk_property_value_float
	etk_property_value_float_get
	etk_property_value_float_set
	etk_property_value_get
	etk_property_value_int
	etk_property_value_int_get
	etk_property_value_int_set
	etk_property_value_long
	etk_property_value_long_get
	etk_property_value_long_set
	etk_property_value_new
	etk_property_value_pointer
	etk_property_value_pointer_get
	etk_property_value_pointer_set
	etk_property_value_set
	etk_property_value_set_valist
	etk_property_value_short
	etk_property_value_short_get
	etk_property_value_short_set
	etk_property_value_string
	etk_property_value_string_get
	etk_property_value_string_set
	etk_property_value_type_get
	etk_radio_button_group_get
	etk_radio_button_group_set
	etk_radio_button_new
	etk_radio_button_new_from_widget
	etk_radio_button_new_with_label
	etk_radio_button_new_with_label_from_widget
	etk_radio_button_type_get
	etk_range_increments_set
	etk_range_page_size_get
	etk_range_page_size_set
	etk_range_range_set
	etk_range_type_get
	etk_range_value_get
	etk_range_value_set
	etk_scrollbar_type_get
	etk_scrolled_view_add_with_viewport
	etk_scrolled_view_hscrollbar_get
	etk_scrolled_view_new
	etk_scrolled_view_policy_get
	etk_scrolled_view_policy_set
	etk_scrolled_view_type_get
	etk_scrolled_view_vscrollbar_get
	etk_selection_text_request
	etk_selection_text_set
	etk_shutdown
	etk_signal_callback_call
	etk_signal_callback_call_valist
	etk_signal_callback_del
	etk_signal_callback_new
	etk_signal_connect
	etk_signal_connect_after
	etk_signal_connect_full
	etk_signal_connect_swapped
	etk_signal_delete
	etk_signal_disconnect
	etk_signal_emit
	etk_signal_emit_by_name
	etk_signal_emit_valist
	etk_signal_lookup
	etk_signal_marshaller_get
	etk_signal_name_get
	etk_signal_new
	etk_signal_shutdown
	etk_signal_stop
	etk_slider_type_get
	etk_spin_button_new
	etk_spin_button_type_get
	etk_statusbar_context_id_get
	etk_statusbar_has_resize_grip_get
	etk_statusbar_has_resize_grip_set
	etk_statusbar_new
	etk_statusbar_pop
	etk_statusbar_push
	etk_statusbar_remove
	etk_statusbar_type_get
	etk_stock_key_get
	etk_stock_label_get
	etk_string_append
	etk_string_append_char
	etk_string_append_printf
	etk_string_append_sized
	etk_string_append_vprintf
	etk_string_copy
	etk_string_get
	etk_string_insert
	etk_string_insert_char
	etk_string_insert_printf
	etk_string_insert_sized
	etk_string_insert_vprintf
	etk_string_length_get
	etk_string_new
	etk_string_new_printf
	etk_string_new_sized
	etk_string_new_vprintf
	etk_string_prepend
	etk_string_prepend_char
	etk_string_prepend_printf
	etk_string_prepend_sized
	etk_string_prepend_vprintf
	etk_string_set
	etk_string_set_printf
	etk_string_set_sized
	etk_string_set_vprintf
	etk_string_truncate
	etk_string_type_get
	etk_table_attach
	etk_table_attach_defaults
	etk_table_cell_clear
	etk_table_homogeneous_get
	etk_table_homogeneous_set
	etk_table_new
	etk_table_resize
	etk_table_type_get
	etk_text_view_new
	etk_text_view_type_get
	etk_textblock_iter_copy
	etk_textblock_iter_free
	etk_textblock_iter_goto_end
	etk_textblock_iter_goto_next_char
	etk_textblock_iter_goto_prev_char
	etk_textblock_iter_goto_start
	etk_textblock_iter_new
	etk_textblock_new
	etk_textblock_realize
	etk_textblock_text_set
	etk_textblock_type_get
	etk_textblock_unrealize
	etk_theme_default_icon_theme_get
	etk_theme_default_widget_theme_get
	etk_theme_icon_theme_get
	etk_theme_icon_theme_set
	etk_theme_init
	etk_theme_object_load
	etk_theme_object_load_from_parent
	etk_theme_shutdown
	etk_theme_widget_theme_get
	etk_theme_widget_theme_set
	etk_toggle_button_active_get
	etk_toggle_button_active_set
	etk_toggle_button_new
	etk_toggle_button_new_with_label
	etk_toggle_button_toggle
	etk_toggle_button_type_get
	etk_tooltips_disable
	etk_tooltips_enable
	etk_tooltips_init
	etk_tooltips_pop_down
	etk_tooltips_pop_up
	etk_tooltips_shutdown
	etk_tooltips_tip_get
	etk_tooltips_tip_set
	etk_tooltips_tip_visible
	etk_toplevel_widget_evas_get
	etk_toplevel_widget_focused_widget_get
	etk_toplevel_widget_focused_widget_next_get
	etk_toplevel_widget_focused_widget_prev_get
	etk_toplevel_widget_focused_widget_set
	etk_toplevel_widget_geometry_get
	etk_toplevel_widget_pointer_pop
	etk_toplevel_widget_pointer_push
	etk_toplevel_widget_type_get
	etk_tree_append
	etk_tree_append_to_row
	etk_tree_append_valist
	etk_tree_build
	etk_tree_clear
	etk_tree_col_expand_get
	etk_tree_col_expand_set
	etk_tree_col_min_width_get
	etk_tree_col_min_width_set
	etk_tree_col_new
	etk_tree_col_place_get
	etk_tree_col_reorder
	etk_tree_col_resizable_get
	etk_tree_col_resizable_set
	etk_tree_col_sort_func_set
	etk_tree_col_title_get
	etk_tree_col_title_set
	etk_tree_col_type_get
	etk_tree_col_visible_get
	etk_tree_col_visible_set
	etk_tree_col_width_get
	etk_tree_col_width_set
	etk_tree_first_row_get
	etk_tree_freeze
	etk_tree_headers_visible_get
	etk_tree_headers_visible_set
	etk_tree_last_row_get
	etk_tree_mode_get
	etk_tree_mode_set
	etk_tree_model_alignment_get
	etk_tree_model_alignment_set
	etk_tree_model_checkbox_new
	etk_tree_model_double_new
	etk_tree_model_free
	etk_tree_model_icon_text_icon_width_get
	etk_tree_model_icon_text_icon_width_set
	etk_tree_model_icon_text_new
	etk_tree_model_image_new
	etk_tree_model_int_new
	etk_tree_model_progress_bar_new
	etk_tree_model_text_new
	etk_tree_multiple_select_get
	etk_tree_multiple_select_set
	etk_tree_new
	etk_tree_next_row_get
	etk_tree_nth_col_get
	etk_tree_num_cols_get
	etk_tree_prev_row_get
	etk_tree_row_collapse
	etk_tree_row_data_get
	etk_tree_row_data_set
	etk_tree_row_data_set_full
	etk_tree_row_del
	etk_tree_row_expand
	etk_tree_row_fields_get
	etk_tree_row_fields_get_valist
	etk_tree_row_fields_set
	etk_tree_row_fields_set_valist
	etk_tree_row_first_child_get
	etk_tree_row_height_get
	etk_tree_row_height_set
	etk_tree_row_last_child_get
	etk_tree_row_scroll_to
	etk_tree_row_select
	etk_tree_row_unselect
	etk_tree_select_all
	etk_tree_selected_row_get
	etk_tree_selected_rows_get
	etk_tree_sort
	etk_tree_thaw
	etk_tree_type_get
	etk_tree_unselect_all
	etk_type_delete
	etk_type_destructors_call
	etk_type_inherits_from
	etk_type_name_get
	etk_type_new
	etk_type_object_construct
	etk_type_parent_type_get
	etk_type_property_add
	etk_type_property_find
	etk_type_property_list
	etk_type_shutdown
	etk_type_signal_add
	etk_type_signal_get
	etk_type_signal_remove
	etk_vbox_new
	etk_vbox_type_get
	etk_viewport_new
	etk_viewport_type_get
	etk_vpaned_new
	etk_vpaned_type_get
	etk_vscrollbar_new
	etk_vscrollbar_type_get
	etk_vseparator_new
	etk_vseparator_type_get
	etk_vslider_new
	etk_vslider_type_get
	etk_widget_clip_get
	etk_widget_clip_set
	etk_widget_clip_unset
	etk_widget_clipboard_received
	etk_widget_dnd_dest_get
	etk_widget_dnd_dest_set
	etk_widget_dnd_dest_widgets_get
	etk_widget_dnd_drag_data_set
	etk_widget_dnd_drag_widget_get
	etk_widget_dnd_drag_widget_set
	etk_widget_dnd_files_get
	etk_widget_dnd_internal_get
	etk_widget_dnd_internal_set
	etk_widget_dnd_source_get
	etk_widget_dnd_source_set
	etk_widget_dnd_types_get
	etk_widget_dnd_types_set
	etk_widget_drag_begin
	etk_widget_drag_drop
	etk_widget_drag_end
	etk_widget_drag_enter
	etk_widget_drag_leave
	etk_widget_drag_motion
	etk_widget_enter
	etk_widget_focus
	etk_widget_geometry_get
	etk_widget_has_event_object_get
	etk_widget_has_event_object_set
	etk_widget_hide
	etk_widget_hide_all
	etk_widget_inner_geometry_get
	etk_widget_is_swallowed
	etk_widget_is_swallowing_object
	etk_widget_is_swallowing_widget
	etk_widget_is_visible
	etk_widget_key_event_propagation_stop
	etk_widget_leave
	etk_widget_lower
	etk_widget_member_object_add
	etk_widget_member_object_del
	etk_widget_member_object_lower
	etk_widget_member_object_raise
	etk_widget_member_object_stack_above
	etk_widget_member_object_stack_below
	etk_widget_name_get
	etk_widget_name_set
	etk_widget_new
	etk_widget_parent_get
	etk_widget_parent_set
	etk_widget_parent_set_full
	etk_widget_pass_mouse_events_get
	etk_widget_pass_mouse_events_set
	etk_widget_raise
	etk_widget_redraw_queue
	etk_widget_repeat_mouse_events_get
	etk_widget_repeat_mouse_events_set
	etk_widget_selection_received
	etk_widget_show
	etk_widget_show_all
	etk_widget_size_allocate
	etk_widget_size_recalc_queue
	etk_widget_size_request
	etk_widget_size_request_full
	etk_widget_size_request_set
	etk_widget_swallow_widget
	etk_widget_theme_file_get
	etk_widget_theme_file_set
	etk_widget_theme_group_get
	etk_widget_theme_group_set
	etk_widget_theme_object_data_get
	etk_widget_theme_object_min_size_calc
	etk_widget_theme_object_part_text_set
	etk_widget_theme_object_signal_emit
	etk_widget_theme_object_swallow
	etk_widget_theme_object_unswallow
	etk_widget_theme_parent_get
	etk_widget_theme_parent_set
	etk_widget_toplevel_evas_get
	etk_widget_toplevel_parent_get
	etk_widget_type_get
	etk_widget_unfocus
	etk_widget_unswallow_widget
	etk_widget_visibility_locked_get
	etk_widget_visibility_locked_set
	etk_window_center_on_window
	etk_window_decorated_get
	etk_window_decorated_set
	etk_window_deiconify
	etk_window_dnd_aware_set
	etk_window_focus
	etk_window_fullscreen
	etk_window_geometry_get
	etk_window_hide_on_delete
	etk_window_iconify
	etk_window_is_focused
	etk_window_is_fullscreen
	etk_window_is_iconified
	etk_window_is_maximized
	etk_window_is_sticky
	etk_window_maximize
	etk_window_move
	etk_window_move_to_mouse
	etk_window_new
	etk_window_resize
	etk_window_shaped_get
	etk_window_shaped_set
	etk_window_skip_pager_hint_get
	etk_window_skip_pager_hint_set
	etk_window_skip_taskbar_hint_get
	etk_window_skip_taskbar_hint_set
	etk_window_stick
	etk_window_title_get
	etk_window_title_set
	etk_window_type_get
	etk_window_unfocus
	etk_window_unfullscreen
	etk_window_unmaximize
	etk_window_unstick
	etk_window_wmclass_set
) ] );

our @EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );

our @EXPORT = qw(
	
);

our $VERSION = '0.01';

sub AUTOLOAD {
    # This AUTOLOAD is used to 'autoload' constants from the constant()
    # XS function.

    my $constname;
    our $AUTOLOAD;
    ($constname = $AUTOLOAD) =~ s/.*:://;
    croak "&Etk::constant not defined" if $constname eq 'constant';
    my ($error, $val) = constant($constname);
    if ($error) { croak $error; }
    {
	no strict 'refs';
	# Fixed between 5.005_53 and 5.005_61
#XXX	if ($] >= 5.00561) {
#XXX	    *$AUTOLOAD = sub () { $val };
#XXX	}
#XXX	else {
	    *$AUTOLOAD = sub { $val };
#XXX	}
    }
    goto &$AUTOLOAD;
}

require XSLoader;
XSLoader::load('Etk', $VERSION);

# Preloaded methods go here.

# Autoload methods go after =cut, and are processed by the autosplit program.

1;
__END__
# Below is stub documentation for your module. You'd better edit it!

=head1 NAME

Etk - Perl bindings for the Enlightened ToolKit (Etk)

=head1 SYNOPSIS

use Etk;

Etk::etk_init();

my $win = Etk::Window->new();
my $button = Etk::Button->new("Click me!");

$win->Add($button);
$win->ShowAll();

$button->SignalAttach("clicked", \&clicked_cb);

sub clicked_cb
{
   print "button clicked!\n";
}

=head1 DESCRIPTION

This module allows the use of Etk from within Perl. You can use them in one
of two ways, either by using the object oriented approach or directly by
calling the functions (although this is not recommended).

=head2 EXPORT

None by default.

=head2 Exportable functions

  void etk_accumulator_bool_and(void *return_value, const void *value_to_accum, void *data)
  void etk_accumulator_bool_or(void *return_value, const void *value_to_accum, void *data)
  void etk_accumulator_stopping_bool_and(void *return_value, const void *value_to_accum, void *data)
  void etk_accumulator_stopping_bool_or(void *return_value, const void *value_to_accum, void *data)
  void etk_alignment_get(Etk_Alignment *alignment, float *xalign, float *yalign, float *xscale, float *yscale)
  Etk_Widget *etk_alignment_new(float xalign, float yalign, float xscale, float yscale)
  void etk_alignment_set(Etk_Alignment *alignment, float xalign, float yalign, float xscale, float yscale)
  Etk_Type *etk_alignment_type_get()
  Evas_List *etk_argument_extra_find(const char *key)
  void etk_argument_help_show(Etk_Argument *args)
  Etk_Bool etk_argument_is_set(Etk_Argument *args, const char *long_name, char short_name)
  int etk_arguments_parse(Etk_Argument *args, int argc, char **argv)
  Etk_Widget *etk_bin_child_get(Etk_Bin *bin)
  void etk_bin_child_set(Etk_Bin *bin, Etk_Widget *child)
  Etk_Type *etk_bin_type_get()
  void etk_box_child_packing_get(Etk_Box *box, Etk_Widget *child, int *padding, Etk_Bool *expand, Etk_Bool *fill, Etk_Bool *pack_end)
  void etk_box_child_packing_set(Etk_Box *box, Etk_Widget *child, int padding, Etk_Bool expand, Etk_Bool fill, Etk_Bool pack_end)
  Etk_Bool etk_box_homogeneous_get(Etk_Box *box)
  void etk_box_homogeneous_set(Etk_Box *box, Etk_Bool homogeneous)
  void etk_box_pack_end(Etk_Box *box, Etk_Widget *child, Etk_Bool expand, Etk_Bool fill, int padding)
  void etk_box_pack_start(Etk_Box *box, Etk_Widget *child, Etk_Bool expand, Etk_Bool fill, int padding)
  int etk_box_spacing_get(Etk_Box *box)
  void etk_box_spacing_set(Etk_Box *box, int spacing)
  Etk_Type *etk_box_type_get()
  void etk_button_alignment_get(Etk_Button *button, float *xalign, float *yalign)
  void etk_button_alignment_set(Etk_Button *button, float xalign, float yalign)
  void etk_button_click(Etk_Button *button)
  Etk_Image *etk_button_image_get(Etk_Button *button)
  void etk_button_image_set(Etk_Button *button, Etk_Image *image)
  const char *etk_button_label_get(Etk_Button *button)
  void etk_button_label_set(Etk_Button *button, const char *label)
  Etk_Widget *etk_button_new()
  Etk_Widget *etk_button_new_from_stock(Etk_Stock_Id stock_id)
  Etk_Widget *etk_button_new_with_label(const char *label)
  void etk_button_press(Etk_Button *button)
  void etk_button_release(Etk_Button *button)
  void etk_button_set_from_stock(Etk_Button *button, Etk_Stock_Id stock_id)
  Etk_Type *etk_button_type_get()
  void etk_cache_edje_object_add(Evas_Object *edje_object)
  void etk_cache_edje_object_add_with_state(Evas_Object *edje_object, int state)
  Evas_Object *etk_cache_edje_object_find(Evas *evas, const char *filename, const char *group)
  Evas_Object *etk_cache_edje_object_find_with_state(Evas *evas, const char *filename, const char *group, int state)
  void etk_cache_edje_object_remove(Evas_Object *edje_object)
  int etk_cache_edje_object_size_get(Evas *evas)
  void etk_cache_edje_object_size_set(Evas *evas, int size)
  void etk_cache_empty(Evas *evas)
  void etk_cache_image_object_add(Evas_Object *image_object)
  Evas_Object *etk_cache_image_object_find(Evas *evas, const char *filename)
  void etk_cache_image_object_remove(Evas_Object *image_object)
  int etk_cache_image_object_size_get(Evas *evas)
  void etk_cache_image_object_size_set(Evas *evas, int size)
  void etk_cache_shutdown()
  Etk_Widget *etk_canvas_new()
  Etk_Bool etk_canvas_object_add(Etk_Canvas *canvas, Evas_Object *object)
  void etk_canvas_object_remove(Etk_Canvas *canvas, Evas_Object *object)
  Etk_Type *etk_canvas_type_get()
  Etk_Widget *etk_check_button_new()
  Etk_Widget *etk_check_button_new_with_label(const char *label)
  Etk_Type *etk_check_button_type_get()
  void etk_clipboard_text_request(Etk_Widget *widget)
  void etk_clipboard_text_set(Etk_Widget *widget, const char *data, int length)
  Etk_Color etk_colorpicker_current_color_get(Etk_Colorpicker *cp)
  void etk_colorpicker_current_color_set(Etk_Colorpicker *cp, Etk_Color color)
  Etk_Colorpicker_Mode etk_colorpicker_mode_get(Etk_Colorpicker *cp)
  void etk_colorpicker_mode_set(Etk_Colorpicker *cp, Etk_Colorpicker_Mode mode)
  Etk_Widget *etk_colorpicker_new()
  Etk_Type *etk_colorpicker_type_get()
  Etk_Combobox_Item *etk_combobox_active_item_get(Etk_Combobox *combobox)
  void etk_combobox_active_item_set(Etk_Combobox *combobox, Etk_Combobox_Item *item)
  void etk_combobox_build(Etk_Combobox *combobox)
  void etk_combobox_clear(Etk_Combobox *combobox)
  void etk_combobox_column_add(Etk_Combobox *combobox, Etk_Combobox_Column_Type col_type, int size, Etk_Bool expand, Etk_Bool hfill, Etk_Bool vfill, float xalign, float yalign)
  void etk_combobox_item_activate(Etk_Combobox_Item *item)
  Etk_Combobox_Item *etk_combobox_item_append(Etk_Combobox *combobox, ...)
  Etk_Combobox_Item *etk_combobox_item_append_relative(Etk_Combobox *combobox, Etk_Combobox_Item *relative, ...)
  Etk_Combobox_Item *etk_combobox_item_append_relative_valist(Etk_Combobox *combobox, Etk_Combobox_Item *relative, va_list args)
  Etk_Combobox_Item *etk_combobox_item_append_valist(Etk_Combobox *combobox, va_list args)
  void *etk_combobox_item_data_get(Etk_Combobox_Item *item)
  void etk_combobox_item_data_set(Etk_Combobox_Item *item, void *data)
  void etk_combobox_item_data_set_full(Etk_Combobox_Item *item, void *data, void (*free_cb)(void *data))
  int etk_combobox_item_height_get(Etk_Combobox *combobox)
  void etk_combobox_item_height_set(Etk_Combobox *combobox, int item_height)
  Etk_Combobox_Item *etk_combobox_item_prepend(Etk_Combobox *combobox, ...)
  Etk_Combobox_Item *etk_combobox_item_prepend_relative(Etk_Combobox *combobox, Etk_Combobox_Item *relative, ...)
  Etk_Combobox_Item *etk_combobox_item_prepend_relative_valist(Etk_Combobox *combobox, Etk_Combobox_Item *relative, va_list args)
  Etk_Combobox_Item *etk_combobox_item_prepend_valist(Etk_Combobox *combobox, va_list args)
  void etk_combobox_item_remove(Etk_Combobox *combobox, Etk_Combobox_Item *item)
  Etk_Type *etk_combobox_item_type_get()
  Etk_Widget *etk_combobox_new()
  Etk_Widget *etk_combobox_new_default()
  Etk_Combobox_Item *etk_combobox_nth_item_get(Etk_Combobox *combobox, int index)
  Etk_Type *etk_combobox_type_get()
  void etk_container_add(Etk_Container *container, Etk_Widget *widget)
  int etk_container_border_width_get(Etk_Container *container)
  void etk_container_border_width_set(Etk_Container *container, int border_width)
  void etk_container_child_space_fill(Etk_Widget *child, Etk_Geometry *child_space, Etk_Bool hfill, Etk_Bool vfill, float xalign, float yalign)
  Evas_List *etk_container_children_get(Etk_Container *container)
  void etk_container_for_each(Etk_Container *container, void (*for_each_cb)(Etk_Widget *child))
  void etk_container_for_each_data(Etk_Container *container, void (*for_each_cb)(Etk_Widget *child, void *data), void *data)
  Etk_Bool etk_container_is_child(Etk_Container *container, Etk_Widget *widget)
  void etk_container_remove(Etk_Container *container, Etk_Widget *widget)
  Etk_Type *etk_container_type_get()
  Etk_Widget *etk_dialog_button_add(Etk_Dialog *dialog, const char *label, int response_id)
  Etk_Widget *etk_dialog_button_add_from_stock(Etk_Dialog *dialog, int stock_id, int response_id)
  Etk_Bool etk_dialog_has_separator_get(Etk_Dialog *dialog)
  void etk_dialog_has_separator_set(Etk_Dialog *dialog, Etk_Bool has_separator)
  Etk_Widget *etk_dialog_new()
  void etk_dialog_pack_button_in_action_area(Etk_Dialog *dialog, Etk_Button *button, int response_id, Etk_Bool expand, Etk_Bool fill, int padding, Etk_Bool pack_at_end)
  void etk_dialog_pack_in_main_area(Etk_Dialog *dialog, Etk_Widget *widget, Etk_Bool expand, Etk_Bool fill, int padding, Etk_Bool pack_at_end)
  void etk_dialog_pack_widget_in_action_area(Etk_Dialog *dialog, Etk_Widget *widget, Etk_Bool expand, Etk_Bool fill, int padding, Etk_Bool pack_at_end)
  Etk_Type *etk_dialog_type_get()
  Etk_Bool etk_dnd_init()
  void etk_dnd_shutdown()
  void etk_drag_begin(Etk_Drag *drag)
  void etk_drag_data_set(Etk_Drag *drag, void *data, int size)
  Etk_Widget *etk_drag_new(Etk_Widget *widget)
  Etk_Widget *etk_drag_parent_widget_get(Etk_Drag *drag)
  void etk_drag_parent_widget_set(Etk_Drag *drag, Etk_Widget *widget)
  Etk_Type *etk_drag_type_get()
  void etk_drag_types_set(Etk_Drag *drag, const char **types, unsigned int num_types)
  Evas_Object *etk_editable_text_object_add(Evas *evas)
  void etk_editable_text_object_cursor_hide(Evas_Object *object)
  void etk_editable_text_object_cursor_move_at_end(Evas_Object *object)
  void etk_editable_text_object_cursor_move_at_start(Evas_Object *object)
  void etk_editable_text_object_cursor_move_left(Evas_Object *object)
  void etk_editable_text_object_cursor_move_right(Evas_Object *object)
  void etk_editable_text_object_cursor_show(Evas_Object *object)
  Etk_Bool etk_editable_text_object_delete_char_after(Evas_Object *object)
  Etk_Bool etk_editable_text_object_delete_char_before(Evas_Object *object)
  Etk_Bool etk_editable_text_object_insert(Evas_Object *object, const char *text)
  const char *etk_editable_text_object_text_get(Evas_Object *object)
  void etk_editable_text_object_text_set(Evas_Object *object, const char *text)
  Etk_Widget *etk_entry_new()
  Etk_Bool etk_entry_password_get(Etk_Entry *entry)
  void etk_entry_password_set(Etk_Entry *entry, Etk_Bool on)
  const char *etk_entry_text_get(Etk_Entry *entry)
  void etk_entry_text_set(Etk_Entry *entry, const char *text)
  Etk_Type *etk_entry_type_get()
  const char *etk_filechooser_widget_current_folder_get(Etk_Filechooser_Widget *filechooser_widget)
  void etk_filechooser_widget_current_folder_set(Etk_Filechooser_Widget *filechooser_widget, const char *folder)
  Etk_Widget *etk_filechooser_widget_new()
  Etk_Bool etk_filechooser_widget_select_multiple_get(Etk_Filechooser_Widget *filechooser_widget)
  void etk_filechooser_widget_select_multiple_set(Etk_Filechooser_Widget *filechooser_widget, Etk_Bool select_multiple)
  const char *etk_filechooser_widget_selected_file_get(Etk_Filechooser_Widget *widget)
  Evas_List *etk_filechooser_widget_selected_files_get(Etk_Filechooser_Widget *widget)
  Etk_Bool etk_filechooser_widget_show_hidden_get(Etk_Filechooser_Widget *filechooser_widget)
  void etk_filechooser_widget_show_hidden_set(Etk_Filechooser_Widget *filechooser_widget, Etk_Bool show_hidden)
  Etk_Type *etk_filechooser_widget_type_get()
  const char *etk_frame_label_get(Etk_Frame *frame)
  void etk_frame_label_set(Etk_Frame *frame, const char *label)
  Etk_Widget *etk_frame_new(const char *label)
  Etk_Type *etk_frame_type_get()
  Etk_Widget *etk_hbox_new(Etk_Bool homogeneous, int spacing)
  Etk_Type *etk_hbox_type_get()
  Etk_Widget *etk_hpaned_new()
  Etk_Type *etk_hpaned_type_get()
  Etk_Widget *etk_hscrollbar_new(double lower, double upper, double value, double step_increment, double page_increment, double page_size)
  Etk_Type *etk_hscrollbar_type_get()
  Etk_Widget *etk_hseparator_new()
  Etk_Type *etk_hseparator_type_get()
  Etk_Widget *etk_hslider_new(double lower, double upper, double value, double step_increment, double page_increment)
  Etk_Type *etk_hslider_type_get()
  Etk_Iconbox_Icon *etk_iconbox_append(Etk_Iconbox *iconbox, const char *filename, const char *edje_group, const char *label)
  void etk_iconbox_clear(Etk_Iconbox *iconbox)
  Etk_Iconbox_Model *etk_iconbox_current_model_get(Etk_Iconbox *iconbox)
  void etk_iconbox_current_model_set(Etk_Iconbox *iconbox, Etk_Iconbox_Model *model)
  void etk_iconbox_freeze(Etk_Iconbox *iconbox)
  void *etk_iconbox_icon_data_get(Etk_Iconbox_Icon *icon)
  void etk_iconbox_icon_data_set(Etk_Iconbox_Icon *icon, void *data)
  void etk_iconbox_icon_data_set_full(Etk_Iconbox_Icon *icon, void *data, void (*free_cb)(void *data))
  void etk_iconbox_icon_del(Etk_Iconbox_Icon *icon)
  void etk_iconbox_icon_file_get(Etk_Iconbox_Icon *icon, const char **filename, const char **edje_group)
  void etk_iconbox_icon_file_set(Etk_Iconbox_Icon *icon, const char *filename, const char *edje_group)
  Etk_Iconbox_Icon *etk_iconbox_icon_get_at_xy(Etk_Iconbox *iconbox, int x, int y, Etk_Bool over_cell, Etk_Bool over_icon, Etk_Bool over_label)
  const char *etk_iconbox_icon_label_get(Etk_Iconbox_Icon *icon)
  void etk_iconbox_icon_label_set(Etk_Iconbox_Icon *icon, const char *label)
  void etk_iconbox_icon_select(Etk_Iconbox_Icon *icon)
  void etk_iconbox_icon_unselect(Etk_Iconbox_Icon *icon)
  Etk_Bool etk_iconbox_is_selected(Etk_Iconbox_Icon *icon)
  void etk_iconbox_model_free(Etk_Iconbox_Model *model)
  void etk_iconbox_model_geometry_get(Etk_Iconbox_Model *model, int *width, int *height, int *xpadding, int *ypadding)
  void etk_iconbox_model_geometry_set(Etk_Iconbox_Model *model, int width, int height, int xpadding, int ypadding)
  void etk_iconbox_model_icon_geometry_get(Etk_Iconbox_Model *model, int *x, int *y, int *width, int *height, Etk_Bool *fill, Etk_Bool *keep_aspect_ratio)
  void etk_iconbox_model_icon_geometry_set(Etk_Iconbox_Model *model, int x, int y, int width, int height, Etk_Bool fill, Etk_Bool keep_aspect_ratio)
  void etk_iconbox_model_label_geometry_get(Etk_Iconbox_Model *model, int *x, int *y, int *width, int *height, float *xalign, float *yalign)
  void etk_iconbox_model_label_geometry_set(Etk_Iconbox_Model *model, int x, int y, int width, int height, float xalign, float yalign)
  Etk_Iconbox_Model *etk_iconbox_model_new(Etk_Iconbox *iconbox)
  Etk_Widget *etk_iconbox_new()
  void etk_iconbox_select_all(Etk_Iconbox *iconbox)
  void etk_iconbox_thaw(Etk_Iconbox *iconbox)
  Etk_Type *etk_iconbox_type_get()
  void etk_iconbox_unselect_all(Etk_Iconbox *iconbox)
  void etk_image_copy(Etk_Image *dest_image, Etk_Image *src_image)
  void etk_image_edje_get(Etk_Image *image, char **edje_filename, char **edje_group)
  const char *etk_image_file_get(Etk_Image *image)
  Etk_Bool etk_image_keep_aspect_get(Etk_Image *image)
  void etk_image_keep_aspect_set(Etk_Image *image, Etk_Bool keep_aspect)
  Etk_Widget *etk_image_new()
  Etk_Widget *etk_image_new_from_edje(const char *edje_filename, const char *edje_group)
  Etk_Widget *etk_image_new_from_file(const char *filename)
  Etk_Widget *etk_image_new_from_stock(Etk_Stock_Id stock_id, Etk_Stock_Size stock_size)
  void etk_image_set_from_edje(Etk_Image *image, const char *edje_filename, const char *edje_group)
  void etk_image_set_from_file(Etk_Image *image, const char *filename)
  void etk_image_set_from_stock(Etk_Image *image, Etk_Stock_Id stock_id, Etk_Stock_Size stock_size)
  void etk_image_size_get(Etk_Image *image, int *width, int *height)
  void etk_image_stock_get(Etk_Image *image, Etk_Stock_Id *stock_id, Etk_Stock_Size *stock_size)
  Etk_Type *etk_image_type_get()
  Etk_Bool etk_init()
  void etk_label_alignment_get(Etk_Label *label, float *xalign, float *yalign)
  void etk_label_alignment_set(Etk_Label *label, float xalign, float yalign)
  const char *etk_label_get(Etk_Label *label)
  Etk_Widget *etk_label_new(const char *text)
  void etk_label_set(Etk_Label *label, const char *text)
  Etk_Type *etk_label_type_get()
  void etk_main()
  void etk_main_iterate()
  void etk_main_iteration_queue()
  void etk_main_quit()
  void etk_main_toplevel_widget_add(Etk_Toplevel_Widget *widget)
  void etk_main_toplevel_widget_remove(Etk_Toplevel_Widget *widget)
  Evas_List *etk_main_toplevel_widgets_get()
  void etk_marshaller_BOOL__DOUBLE(Etk_Signal_Callback_Function callback, Etk_Object *object, void *data, void *return_value, va_list arguments)
  void etk_marshaller_BOOL__POINTER_POINTER(Etk_Signal_Callback_Function callback, Etk_Object *object, void *data, void *return_value, va_list arguments)
  void etk_marshaller_BOOL__VOID(Etk_Signal_Callback_Function callback, Etk_Object *object, void *data, void *return_value, va_list arguments)
  void etk_marshaller_VOID__DOUBLE(Etk_Signal_Callback_Function callback, Etk_Object *object, void *data, void *return_value, va_list arguments)
  void etk_marshaller_VOID__INT(Etk_Signal_Callback_Function callback, Etk_Object *object, void *data, void *return_value, va_list arguments)
  void etk_marshaller_VOID__INT_POINTER(Etk_Signal_Callback_Function callback, Etk_Object *object, void *data, void *return_value, va_list arguments)
  void etk_marshaller_VOID__POINTER(Etk_Signal_Callback_Function callback, Etk_Object *object, void *data, void *return_value, va_list arguments)
  void etk_marshaller_VOID__POINTER_POINTER(Etk_Signal_Callback_Function callback, Etk_Object *object, void *data, void *return_value, va_list arguments)
  void etk_marshaller_VOID__VOID(Etk_Signal_Callback_Function callback, Etk_Object *object, void *data, void *return_value, va_list arguments)
  Etk_Widget *etk_menu_bar_new()
  Etk_Type *etk_menu_bar_type_get()
  void etk_menu_item_activate(Etk_Menu_Item *menu_item)
  Etk_Bool etk_menu_item_check_active_get(Etk_Menu_Item_Check *check_item)
  void etk_menu_item_check_active_set(Etk_Menu_Item_Check *check_item, Etk_Bool active)
  Etk_Widget *etk_menu_item_check_new()
  Etk_Widget *etk_menu_item_check_new_with_label(const char *label)
  Etk_Type *etk_menu_item_check_type_get()
  void etk_menu_item_deselect(Etk_Menu_Item *menu_item)
  Etk_Widget *etk_menu_item_image_new()
  Etk_Widget *etk_menu_item_image_new_from_stock(Etk_Stock_Id stock_id)
  Etk_Widget *etk_menu_item_image_new_with_label(const char *label)
  void etk_menu_item_image_set(Etk_Menu_Item_Image *image_item, Etk_Image *image)
  Etk_Type *etk_menu_item_image_type_get()
  const char *etk_menu_item_label_get(Etk_Menu_Item *menu_item)
  void etk_menu_item_label_set(Etk_Menu_Item *menu_item, const char *label)
  Etk_Widget *etk_menu_item_new()
  Etk_Widget *etk_menu_item_new_from_stock(Etk_Stock_Id stock_id)
  Etk_Widget *etk_menu_item_new_with_label(const char *label)
  Evas_List **etk_menu_item_radio_group_get(Etk_Menu_Item_Radio *radio_item)
  void etk_menu_item_radio_group_set(Etk_Menu_Item_Radio *radio_item, Evas_List **group)
  Etk_Widget *etk_menu_item_radio_new(Evas_List **group)
  Etk_Widget *etk_menu_item_radio_new_from_widget(Etk_Menu_Item_Radio *radio_item)
  Etk_Widget *etk_menu_item_radio_new_with_label(const char *label, Evas_List **group)
  Etk_Widget *etk_menu_item_radio_new_with_label_from_widget(const char *label, Etk_Menu_Item_Radio *radio_item)
  Etk_Type *etk_menu_item_radio_type_get()
  void etk_menu_item_select(Etk_Menu_Item *menu_item)
  Etk_Widget *etk_menu_item_separator_new()
  Etk_Type *etk_menu_item_separator_type_get()
  void etk_menu_item_set_from_stock(Etk_Menu_Item *menu_item, Etk_Stock_Id stock_id)
  void etk_menu_item_submenu_set(Etk_Menu_Item *menu_item, Etk_Menu *submenu)
  Etk_Type *etk_menu_item_type_get()
  Etk_Widget *etk_menu_new()
  void etk_menu_popdown(Etk_Menu *menu)
  void etk_menu_popup(Etk_Menu *menu)
  void etk_menu_popup_at_xy(Etk_Menu *menu, int x, int y)
  void etk_menu_shell_append(Etk_Menu_Shell *menu_shell, Etk_Menu_Item *item)
  void etk_menu_shell_append_relative(Etk_Menu_Shell *menu_shell, Etk_Menu_Item *item, Etk_Menu_Item *relative)
  void etk_menu_shell_insert(Etk_Menu_Shell *menu_shell, Etk_Menu_Item *item, int position)
  Evas_List *etk_menu_shell_items_get(Etk_Menu_Shell *menu_shell)
  void etk_menu_shell_prepend(Etk_Menu_Shell *menu_shell, Etk_Menu_Item *item)
  void etk_menu_shell_prepend_relative(Etk_Menu_Shell *menu_shell, Etk_Menu_Item *item, Etk_Menu_Item *relative)
  void etk_menu_shell_remove(Etk_Menu_Shell *menu_shell, Etk_Menu_Item *item)
  Etk_Type *etk_menu_shell_type_get()
  Etk_Type *etk_menu_type_get()
  Etk_Message_Dialog_Buttons etk_message_dialog_buttons_get(Etk_Message_Dialog *dialog)
  void etk_message_dialog_buttons_set(Etk_Message_Dialog *dialog, Etk_Message_Dialog_Buttons buttons)
  Etk_Message_Dialog_Type etk_message_dialog_message_type_get(Etk_Message_Dialog *dialog)
  void etk_message_dialog_message_type_set(Etk_Message_Dialog *dialog, Etk_Message_Dialog_Type type)
  Etk_Widget *etk_message_dialog_new(Etk_Message_Dialog_Type message_type, Etk_Message_Dialog_Buttons buttons, const char *text)
  const char *etk_message_dialog_text_get(Etk_Message_Dialog *dialog)
  void etk_message_dialog_text_set(Etk_Message_Dialog *dialog, const char *text)
  Etk_Type *etk_message_dialog_type_get()
  int etk_notebook_current_page_get(Etk_Notebook *notebook)
  void etk_notebook_current_page_set(Etk_Notebook *notebook, int page_num)
  Etk_Widget *etk_notebook_new()
  int etk_notebook_num_pages_get(Etk_Notebook *notebook)
  int etk_notebook_page_append(Etk_Notebook *notebook, const char *tab_label, Etk_Widget *page_child)
  Etk_Widget *etk_notebook_page_child_get(Etk_Notebook *notebook, int page_num)
  void etk_notebook_page_child_set(Etk_Notebook *notebook, int page_num, Etk_Widget *child)
  int etk_notebook_page_index_get(Etk_Notebook *notebook, Etk_Widget *child)
  int etk_notebook_page_insert(Etk_Notebook *notebook, const char *tab_label, Etk_Widget *page_child, int position)
  int etk_notebook_page_next(Etk_Notebook *notebook)
  int etk_notebook_page_prepend(Etk_Notebook *notebook, const char *tab_label, Etk_Widget *page_child)
  int etk_notebook_page_prev(Etk_Notebook *notebook)
  void etk_notebook_page_remove(Etk_Notebook *notebook, int page_num)
  const char *etk_notebook_page_tab_label_get(Etk_Notebook *notebook, int page_num)
  void etk_notebook_page_tab_label_set(Etk_Notebook *notebook, int page_num, const char *tab_label)
  Etk_Widget *etk_notebook_page_tab_widget_get(Etk_Notebook *notebook, int page_num)
  void etk_notebook_page_tab_widget_set(Etk_Notebook *notebook, int page_num, Etk_Widget *tab_widget)
  Etk_Type *etk_notebook_type_get()
  Etk_Object *etk_object_check_cast(Etk_Object *object, Etk_Type *type)
  void *etk_object_data_get(Etk_Object *object, const char *key)
  void etk_object_data_set(Etk_Object *object, const char *key, void *value)
  void etk_object_data_set_full(Etk_Object *object, const char *key, void *value, void (*free_cb)(void *data))
  void etk_object_destroy(Etk_Object *object)
  void etk_object_destroy_all_objects()
  Etk_Object *etk_object_new(Etk_Type *object_type, const char *first_property, ...)
  Etk_Object *etk_object_new_valist(Etk_Type *object_type, const char *first_property, va_list args)
  void etk_object_notification_callback_add(Etk_Object *object, const char *property_name, void (*callback)(Etk_Object *object, const char *property_name, void *data), void *data)
  void etk_object_notification_callback_remove(Etk_Object *object, const char *property_name, void (*callback)(Etk_Object *object, const char *property_name, void *data))
  void etk_object_notify(Etk_Object *object, const char *property_name)
  Etk_Type *etk_object_object_type_get(Etk_Object *object)
  void etk_object_properties_get(Etk_Object *object, const char *first_property, ...)
  void etk_object_properties_get_valist(Etk_Object *object, const char *first_property, va_list args)
  void etk_object_properties_set(Etk_Object *object, const char *first_property, ...)
  void etk_object_properties_set_valist(Etk_Object *object, const char *first_property, va_list args)
  void etk_object_property_reset(Etk_Object *object, const char *property_name)
  void etk_object_signal_callback_add(Etk_Object *object, Etk_Signal_Callback *signal_callback, Etk_Bool after)
  void etk_object_signal_callback_remove(Etk_Object *object, Etk_Signal_Callback *signal_callback)
  void etk_object_signal_callbacks_get(Etk_Object *object, Etk_Signal *signal, Evas_List **callbacks, Etk_Bool after)
  Etk_Type *etk_object_type_get()
  void etk_object_weak_pointer_add(Etk_Object *object, void **pointer_location)
  void etk_object_weak_pointer_remove(Etk_Object *object, void **pointer_location)
  Etk_Widget *etk_paned_child1_get(Etk_Paned *paned)
  void etk_paned_child1_set(Etk_Paned *paned, Etk_Widget *child, Etk_Bool expand)
  Etk_Widget *etk_paned_child2_get(Etk_Paned *paned)
  void etk_paned_child2_set(Etk_Paned *paned, Etk_Widget *child, Etk_Bool expand)
  int etk_paned_position_get(Etk_Paned *paned)
  void etk_paned_position_set(Etk_Paned *paned, int position)
  Etk_Type *etk_paned_type_get()
  Etk_Popup_Window *etk_popup_window_focused_window_get()
  void etk_popup_window_focused_window_set(Etk_Popup_Window *popup_window)
  Etk_Bool etk_popup_window_is_popped_up(Etk_Popup_Window *popup_window)
  void etk_popup_window_popdown(Etk_Popup_Window *popup_window)
  void etk_popup_window_popdown_all()
  void etk_popup_window_popup(Etk_Popup_Window *popup_window)
  void etk_popup_window_popup_at_xy(Etk_Popup_Window *popup_window, int x, int y)
  Etk_Type *etk_popup_window_type_get()
  double etk_progress_bar_fraction_get(Etk_Progress_Bar *progress_bar)
  void etk_progress_bar_fraction_set(Etk_Progress_Bar *progress_bar, double fraction)
  Etk_Widget *etk_progress_bar_new()
  Etk_Widget *etk_progress_bar_new_with_text(const char *label)
  void etk_progress_bar_pulse(Etk_Progress_Bar *progress_bar)
  double etk_progress_bar_pulse_step_get(Etk_Progress_Bar *progress_bar)
  void etk_progress_bar_pulse_step_set(Etk_Progress_Bar *progress_bar, double pulse_step)
  const char *etk_progress_bar_text_get(Etk_Progress_Bar *progress_bar)
  void etk_progress_bar_text_set(Etk_Progress_Bar *progress_bar, const char *label)
  Etk_Type *etk_progress_bar_type_get()
  Etk_Bool etk_property_default_value_set(Etk_Property *property, Etk_Property_Value *default_value)
  void etk_property_delete(Etk_Property *property)
  Etk_Property *etk_property_new(const char *name, int property_id, Etk_Property_Type type, Etk_Property_Flags flags, Etk_Property_Value *default_value)
  Etk_Property_Type etk_property_type_get(Etk_Property *property)
  Etk_Property_Value *etk_property_value_bool (Etk_Bool value)
  Etk_Bool etk_property_value_bool_get(Etk_Property_Value *value)
  void etk_property_value_bool_set (Etk_Property_Value *property_value, Etk_Bool value)
  Etk_Property_Value *etk_property_value_char (char value)
  char etk_property_value_char_get(Etk_Property_Value *value)
  void etk_property_value_char_set (Etk_Property_Value *property_value, char value)
  Etk_Property_Value *etk_property_value_create (Etk_Property_Type type, ...)
  Etk_Property_Value *etk_property_value_create_valist (Etk_Property_Type type, va_list *arg)
  void etk_property_value_delete(Etk_Property_Value *value)
  Etk_Property_Value *etk_property_value_double (double value)
  double etk_property_value_double_get(Etk_Property_Value *value)
  void etk_property_value_double_set (Etk_Property_Value *property_value, double value)
  Etk_Property_Value *etk_property_value_float (float value)
  float etk_property_value_float_get(Etk_Property_Value *value)
  void etk_property_value_float_set (Etk_Property_Value *property_value, float value)
  void etk_property_value_get(Etk_Property_Value *value, Etk_Property_Type type, void *value_location)
  Etk_Property_Value *etk_property_value_int (int value)
  int etk_property_value_int_get(Etk_Property_Value *value)
  void etk_property_value_int_set (Etk_Property_Value *property_value, int value)
  Etk_Property_Value *etk_property_value_long (long value)
  long etk_property_value_long_get(Etk_Property_Value *value)
  void etk_property_value_long_set (Etk_Property_Value *property_value, long value)
  Etk_Property_Value *etk_property_value_new()
  Etk_Property_Value *etk_property_value_pointer (void *value)
  void * etk_property_value_pointer_get(Etk_Property_Value *value)
  void etk_property_value_pointer_set (Etk_Property_Value *property_value, void *value)
  void etk_property_value_set (Etk_Property_Value *property_value, Etk_Property_Type type, ...)
  void etk_property_value_set_valist (Etk_Property_Value *property_value, Etk_Property_Type type, va_list *arg)
  Etk_Property_Value *etk_property_value_short (short value)
  short etk_property_value_short_get(Etk_Property_Value *value)
  void etk_property_value_short_set (Etk_Property_Value *property_value, short value)
  Etk_Property_Value *etk_property_value_string (const char *value)
  const char *etk_property_value_string_get(Etk_Property_Value *value)
  void etk_property_value_string_set (Etk_Property_Value *property_value, const char *value)
  Etk_Property_Type etk_property_value_type_get(Etk_Property_Value *value)
  Evas_List **etk_radio_button_group_get(Etk_Radio_Button *radio_button)
  void etk_radio_button_group_set(Etk_Radio_Button *radio_button, Evas_List **group)
  Etk_Widget *etk_radio_button_new(Evas_List **group)
  Etk_Widget *etk_radio_button_new_from_widget(Etk_Radio_Button *radio_button)
  Etk_Widget *etk_radio_button_new_with_label(const char *label, Evas_List **group)
  Etk_Widget *etk_radio_button_new_with_label_from_widget(const char *label, Etk_Radio_Button *radio_button)
  Etk_Type *etk_radio_button_type_get()
  void etk_range_increments_set(Etk_Range *range, double step, double page)
  double etk_range_page_size_get(Etk_Range *range)
  void etk_range_page_size_set(Etk_Range *range, double page_size)
  void etk_range_range_set(Etk_Range *range, double lower, double upper)
  Etk_Type *etk_range_type_get()
  double etk_range_value_get(Etk_Range *range)
  void etk_range_value_set(Etk_Range *range, double value)
  Etk_Type *etk_scrollbar_type_get()
  void etk_scrolled_view_add_with_viewport(Etk_Scrolled_View *scrolled_view, Etk_Widget *child)
  Etk_Range *etk_scrolled_view_hscrollbar_get(Etk_Scrolled_View *scrolled_view)
  Etk_Widget *etk_scrolled_view_new()
  void etk_scrolled_view_policy_get(Etk_Scrolled_View *scrolled_view, Etk_Scrolled_View_Policy *hpolicy, Etk_Scrolled_View_Policy *vpolicy)
  void etk_scrolled_view_policy_set(Etk_Scrolled_View *scrolled_view, Etk_Scrolled_View_Policy hpolicy, Etk_Scrolled_View_Policy vpolicy)
  Etk_Type *etk_scrolled_view_type_get()
  Etk_Range *etk_scrolled_view_vscrollbar_get(Etk_Scrolled_View *scrolled_view)
  void etk_selection_text_request(Etk_Widget *widget)
  void etk_selection_text_set(Etk_Widget *widget, const char *data, int length)
  void etk_shutdown()
  void etk_signal_callback_call(Etk_Signal_Callback *callback, Etk_Object *object, void *return_value, ...)
  void etk_signal_callback_call_valist(Etk_Signal_Callback *callback, Etk_Object *object, void *return_value, va_list args)
  void etk_signal_callback_del(Etk_Signal_Callback *signal_callback)
  Etk_Signal_Callback *etk_signal_callback_new(Etk_Signal *signal, Etk_Signal_Callback_Function callback, void *data, Etk_Bool swapped)
  void etk_signal_connect(const char *signal_name, Etk_Object *object, Etk_Signal_Callback_Function callback, void *data)
  void etk_signal_connect_after(const char *signal_name, Etk_Object *object, Etk_Signal_Callback_Function callback, void *data)
  void etk_signal_connect_full(Etk_Signal *signal, Etk_Object *object, Etk_Signal_Callback_Function callback, void *data, Etk_Bool swapped, Etk_Bool after)
  void etk_signal_connect_swapped(const char *signal_name, Etk_Object *object, Etk_Signal_Callback_Function callback, void *data)
  void etk_signal_delete(Etk_Signal *signal)
  void etk_signal_disconnect(const char *signal_name, Etk_Object *object, Etk_Signal_Callback_Function callback)
  void etk_signal_emit(Etk_Signal *signal, Etk_Object *object, void *return_value, ...)
  void etk_signal_emit_by_name(const char *signal_name, Etk_Object *object, void *return_value, ...)
  void etk_signal_emit_valist(Etk_Signal *signal, Etk_Object *object, void *return_value, va_list args)
  Etk_Signal *etk_signal_lookup(const char *signal_name, Etk_Type *type)
  Etk_Marshaller etk_signal_marshaller_get(Etk_Signal *signal)
  const char *etk_signal_name_get(Etk_Signal *signal)
  Etk_Signal *etk_signal_new(const char *signal_name, Etk_Type *object_type, long default_handler_offset, Etk_Marshaller marshaller, Etk_Accumulator accumulator, void *accum_data)
  void etk_signal_shutdown()
  void etk_signal_stop()
  Etk_Type *etk_slider_type_get()
  Etk_Widget *etk_spin_button_new(double min, double max, double step)
  Etk_Type *etk_spin_button_type_get()
  int etk_statusbar_context_id_get(Etk_Statusbar *statusbar, const char *context)
  Etk_Bool etk_statusbar_has_resize_grip_get(Etk_Statusbar *statusbar)
  void etk_statusbar_has_resize_grip_set(Etk_Statusbar *statusbar, Etk_Bool has_resize_grip)
  Etk_Widget *etk_statusbar_new()
  void etk_statusbar_pop(Etk_Statusbar *statusbar, int context_id)
  int etk_statusbar_push(Etk_Statusbar *statusbar, const char *message, int context_id)
  void etk_statusbar_remove(Etk_Statusbar *statusbar, int message_id)
  Etk_Type *etk_statusbar_type_get()
  char *etk_stock_key_get(Etk_Stock_Id stock_id, Etk_Stock_Size size)
  char *etk_stock_label_get(Etk_Stock_Id stock_id)
  Etk_String *etk_string_append(Etk_String *string, const char *text)
  Etk_String *etk_string_append_char(Etk_String *string, char c)
  Etk_String *etk_string_append_printf(Etk_String *string, const char *format, ...)
  Etk_String *etk_string_append_sized(Etk_String *string, const char *text, int length)
  Etk_String *etk_string_append_vprintf(Etk_String *string, const char *format, va_list args)
  Etk_String *etk_string_copy(const Etk_String *string)
  const char *etk_string_get(Etk_String *string)
  Etk_String *etk_string_insert(Etk_String *string, int pos, const char *text)
  Etk_String *etk_string_insert_char(Etk_String *string, int pos, char c)
  Etk_String *etk_string_insert_printf(Etk_String *string, int pos, const char *format, ...)
  Etk_String *etk_string_insert_sized(Etk_String *string, int pos, const char *text, int length)
  Etk_String *etk_string_insert_vprintf(Etk_String *string, int pos, const char *format, va_list args)
  int etk_string_length_get(Etk_String *string)
  Etk_String *etk_string_new(const char *value)
  Etk_String *etk_string_new_printf(const char *format, ...)
  Etk_String *etk_string_new_sized(const char *value, int size)
  Etk_String *etk_string_new_vprintf(const char *format, va_list args)
  Etk_String *etk_string_prepend(Etk_String *string, const char *text)
  Etk_String *etk_string_prepend_char(Etk_String *string, char c)
  Etk_String *etk_string_prepend_printf(Etk_String *string, const char *format, ...)
  Etk_String *etk_string_prepend_sized(Etk_String *string, const char *text, int length)
  Etk_String *etk_string_prepend_vprintf(Etk_String *string, const char *format, va_list args)
  Etk_String *etk_string_set(Etk_String *string, const char *value)
  Etk_String *etk_string_set_printf(Etk_String *string, const char *format, ...)
  Etk_String *etk_string_set_sized(Etk_String *string, const char *value, int length)
  Etk_String *etk_string_set_vprintf(Etk_String *string, const char *format, va_list args)
  Etk_String *etk_string_truncate(Etk_String *string, int length)
  Etk_Type *etk_string_type_get()
  void etk_table_attach(Etk_Table *table, Etk_Widget *child, int left_attach, int right_attach,
   int top_attach, int bottom_attach, int x_padding, int y_padding, Etk_Fill_Policy_Flags fill_policy)
  void etk_table_attach_defaults(Etk_Table *table, Etk_Widget *child, int left_attach, int right_attach, int top_attach, int bottom_attach)
  void etk_table_cell_clear(Etk_Table *table, int col, int row)
  Etk_Bool etk_table_homogeneous_get(Etk_Table *table)
  void etk_table_homogeneous_set(Etk_Table *table, Etk_Bool homogeneous)
  Etk_Widget *etk_table_new(int num_cols, int num_rows, Etk_Bool homogeneous)
  void etk_table_resize(Etk_Table *table, int num_cols, int num_rows)
  Etk_Type *etk_table_type_get()
  Etk_Widget *etk_text_view_new()
  Etk_Type *etk_text_view_type_get()
  void etk_textblock_iter_copy(Etk_Textblock_Iter *iter, Etk_Textblock_Iter *dest_iter)
  void etk_textblock_iter_free(Etk_Textblock_Iter *iter)
  void etk_textblock_iter_goto_end(Etk_Textblock_Iter *iter)
  void etk_textblock_iter_goto_next_char(Etk_Textblock_Iter *iter)
  void etk_textblock_iter_goto_prev_char(Etk_Textblock_Iter *iter)
  void etk_textblock_iter_goto_start(Etk_Textblock_Iter *iter)
  Etk_Textblock_Iter *etk_textblock_iter_new(Etk_Textblock *textblock)
  Etk_Textblock *etk_textblock_new()
  void etk_textblock_realize(Etk_Textblock *textblock, Evas *evas)
  void etk_textblock_text_set(Etk_Textblock *textblock, const char *text)
  Etk_Type *etk_textblock_type_get()
  void etk_textblock_unrealize(Etk_Textblock *textblock)
  const char *etk_theme_default_icon_theme_get()
  const char *etk_theme_default_widget_theme_get()
  const char *etk_theme_icon_theme_get()
  Etk_Bool etk_theme_icon_theme_set(const char *theme_name)
  void etk_theme_init()
  Evas_Object *etk_theme_object_load(Evas *evas, const char *filename, const char *group)
  Evas_Object *etk_theme_object_load_from_parent(Evas *evas, Etk_Widget *theme_parent, const char *filename, const char *group)
  void etk_theme_shutdown()
  const char *etk_theme_widget_theme_get()
  Etk_Bool etk_theme_widget_theme_set(const char *theme_name)
  Etk_Bool etk_toggle_button_active_get(Etk_Toggle_Button *toggle_button)
  void etk_toggle_button_active_set(Etk_Toggle_Button *toggle_button, Etk_Bool active)
  Etk_Widget *etk_toggle_button_new()
  Etk_Widget *etk_toggle_button_new_with_label(const char *label)
  void etk_toggle_button_toggle(Etk_Toggle_Button *toggle_button)
  Etk_Type *etk_toggle_button_type_get()
  void etk_tooltips_disable()
  void etk_tooltips_enable()
  void etk_tooltips_init()
  void etk_tooltips_pop_down()
  void etk_tooltips_pop_up(Etk_Widget *widget)
  void etk_tooltips_shutdown()
  const char *etk_tooltips_tip_get(Etk_Widget *widget)
  void etk_tooltips_tip_set(Etk_Widget *widget, const char *text)
  Etk_Bool etk_tooltips_tip_visible()
  Evas *etk_toplevel_widget_evas_get(Etk_Toplevel_Widget *toplevel_widget)
  Etk_Widget *etk_toplevel_widget_focused_widget_get(Etk_Toplevel_Widget *toplevel_widget)
  Etk_Widget *etk_toplevel_widget_focused_widget_next_get(Etk_Toplevel_Widget *toplevel_widget)
  Etk_Widget *etk_toplevel_widget_focused_widget_prev_get(Etk_Toplevel_Widget *toplevel_widget)
  void etk_toplevel_widget_focused_widget_set(Etk_Toplevel_Widget *toplevel_widget, Etk_Widget *widget)
  void etk_toplevel_widget_geometry_get(Etk_Toplevel_Widget *toplevel_widget, int *x, int *y, int *w, int *h)
  void etk_toplevel_widget_pointer_pop(Etk_Toplevel_Widget *toplevel_widget, Etk_Pointer_Type pointer_type)
  void etk_toplevel_widget_pointer_push(Etk_Toplevel_Widget *toplevel_widget, Etk_Pointer_Type pointer_type)
  Etk_Type *etk_toplevel_widget_type_get()
  Etk_Tree_Row *etk_tree_append(Etk_Tree *tree, ...)
  Etk_Tree_Row *etk_tree_append_to_row(Etk_Tree_Row *row, ...)
  Etk_Tree_Row *etk_tree_append_valist(Etk_Tree *tree, va_list args)
  void etk_tree_build(Etk_Tree *tree)
  void etk_tree_clear(Etk_Tree *tree)
  Etk_Bool etk_tree_col_expand_get(Etk_Tree_Col *col)
  void etk_tree_col_expand_set(Etk_Tree_Col *col, Etk_Bool expand)
  int etk_tree_col_min_width_get(Etk_Tree_Col *col)
  void etk_tree_col_min_width_set(Etk_Tree_Col *col, int min_width)
  Etk_Tree_Col *etk_tree_col_new(Etk_Tree *tree, const char *title, Etk_Tree_Model *model, int width)
  int etk_tree_col_place_get(Etk_Tree_Col *col)
  void etk_tree_col_reorder(Etk_Tree_Col *col, int new_place)
  Etk_Bool etk_tree_col_resizable_get(Etk_Tree_Col *col)
  void etk_tree_col_resizable_set(Etk_Tree_Col *col, Etk_Bool resizable)
  void etk_tree_col_sort_func_set(Etk_Tree_Col *col, int (*compare_cb)(Etk_Tree *tree, Etk_Tree_Row *row1, Etk_Tree_Row *row2, Etk_Tree_Col *col, void *data), void *data)
  const char *etk_tree_col_title_get(Etk_Tree_Col *col)
  void etk_tree_col_title_set(Etk_Tree_Col *col, const char *title)
  Etk_Type *etk_tree_col_type_get()
  Etk_Bool etk_tree_col_visible_get(Etk_Tree_Col *col)
  void etk_tree_col_visible_set(Etk_Tree_Col *col, Etk_Bool visible)
  int etk_tree_col_width_get(Etk_Tree_Col *col)
  void etk_tree_col_width_set(Etk_Tree_Col *col, int width)
  Etk_Tree_Row *etk_tree_first_row_get(Etk_Tree *tree)
  void etk_tree_freeze(Etk_Tree *tree)
  Etk_Bool etk_tree_headers_visible_get(Etk_Tree *tree)
  void etk_tree_headers_visible_set(Etk_Tree *tree, Etk_Bool headers_visible)
  Etk_Tree_Row *etk_tree_last_row_get(Etk_Tree *tree, Etk_Bool walking_through_hierarchy, Etk_Bool include_collapsed_children)
  Etk_Tree_Mode etk_tree_mode_get(Etk_Tree *tree)
  void etk_tree_mode_set(Etk_Tree *tree, Etk_Tree_Mode mode)
  void etk_tree_model_alignment_get(Etk_Tree_Model *model, float *xalign, float *yalign)
  void etk_tree_model_alignment_set(Etk_Tree_Model *model, float xalign, float yalign)
  Etk_Tree_Model *etk_tree_model_checkbox_new(Etk_Tree *tree)
  Etk_Tree_Model *etk_tree_model_double_new(Etk_Tree *tree)
  void etk_tree_model_free(Etk_Tree_Model *model)
  int etk_tree_model_icon_text_icon_width_get(Etk_Tree_Model *model)
  void etk_tree_model_icon_text_icon_width_set(Etk_Tree_Model *model, int icon_width)
  Etk_Tree_Model *etk_tree_model_icon_text_new(Etk_Tree *tree, Etk_Tree_Model_Image_Type icon_type)
  Etk_Tree_Model *etk_tree_model_image_new(Etk_Tree *tree, Etk_Tree_Model_Image_Type image_type)
  Etk_Tree_Model *etk_tree_model_int_new(Etk_Tree *tree)
  Etk_Tree_Model *etk_tree_model_progress_bar_new(Etk_Tree *tree)
  Etk_Tree_Model *etk_tree_model_text_new(Etk_Tree *tree)
  Etk_Bool etk_tree_multiple_select_get(Etk_Tree *tree)
  void etk_tree_multiple_select_set(Etk_Tree *tree, Etk_Bool multiple_select)
  Etk_Widget *etk_tree_new()
  Etk_Tree_Row *etk_tree_next_row_get(Etk_Tree_Row *row, Etk_Bool walking_through_hierarchy, Etk_Bool include_collapsed_children)
  Etk_Tree_Col *etk_tree_nth_col_get(Etk_Tree *tree, int nth)
  int etk_tree_num_cols_get(Etk_Tree *tree)
  Etk_Tree_Row *etk_tree_prev_row_get(Etk_Tree_Row *row, Etk_Bool walking_through_hierarchy, Etk_Bool include_collapsed_children)
  void etk_tree_row_collapse(Etk_Tree_Row *row)
  void *etk_tree_row_data_get(Etk_Tree_Row *row)
  void etk_tree_row_data_set(Etk_Tree_Row *row, void *data)
  void etk_tree_row_data_set_full(Etk_Tree_Row *row, void *data, void (*free_cb)(void *data))
  void etk_tree_row_del(Etk_Tree_Row *row)
  void etk_tree_row_expand(Etk_Tree_Row *row)
  void etk_tree_row_fields_get(Etk_Tree_Row *row, ...)
  void etk_tree_row_fields_get_valist(Etk_Tree_Row *row, va_list args)
  void etk_tree_row_fields_set(Etk_Tree_Row *row, ...)
  void etk_tree_row_fields_set_valist(Etk_Tree_Row *row, va_list args)
  Etk_Tree_Row *etk_tree_row_first_child_get(Etk_Tree_Row *row)
  int etk_tree_row_height_get(Etk_Tree *tree)
  void etk_tree_row_height_set(Etk_Tree *tree, int row_height)
  Etk_Tree_Row *etk_tree_row_last_child_get(Etk_Tree_Row *row, Etk_Bool walking_through_hierarchy, Etk_Bool include_collapsed_children)
  void etk_tree_row_scroll_to(Etk_Tree_Row *row, Etk_Bool center_the_row)
  void etk_tree_row_select(Etk_Tree_Row *row)
  void etk_tree_row_unselect(Etk_Tree_Row *row)
  void etk_tree_select_all(Etk_Tree *tree)
  Etk_Tree_Row *etk_tree_selected_row_get(Etk_Tree *tree)
  Evas_List *etk_tree_selected_rows_get(Etk_Tree *tree)
  void etk_tree_sort(Etk_Tree *tree, int (*compare_cb)(Etk_Tree *tree, Etk_Tree_Row *row1, Etk_Tree_Row *row2, Etk_Tree_Col *col, void *data), Etk_Bool ascendant, Etk_Tree_Col *col, void *data)
  void etk_tree_thaw(Etk_Tree *tree)
  Etk_Type *etk_tree_type_get()
  void etk_tree_unselect_all(Etk_Tree *tree)
  void etk_type_delete(Etk_Type *type)
  void etk_type_destructors_call(Etk_Type *type, Etk_Object *object)
  Etk_Bool etk_type_inherits_from(Etk_Type *type, Etk_Type *parent)
  const char *etk_type_name_get(Etk_Type *type)
  Etk_Type *etk_type_new(const char *type_name, Etk_Type *parent_type, int type_size,
   Etk_Constructor constructor, Etk_Destructor destructor)
  void etk_type_object_construct(Etk_Type *type, Etk_Object *object)
  Etk_Type *etk_type_parent_type_get(Etk_Type *type)
  Etk_Property *etk_type_property_add(Etk_Type *type, const char *name, int property_id, Etk_Property_Type property_type, Etk_Property_Flags flags, Etk_Property_Value *default_value)
  Etk_Bool etk_type_property_find(Etk_Type *type, const char *name, Etk_Type **property_owner, Etk_Property **property)
  void etk_type_property_list(Etk_Type *type, Evas_List **properties)
  void etk_type_shutdown()
  void etk_type_signal_add(Etk_Type *type, Etk_Signal *signal)
  Etk_Signal *etk_type_signal_get(Etk_Type *type, const char *signal_name)
  void etk_type_signal_remove(Etk_Type *type, Etk_Signal *signal)
  Etk_Widget *etk_vbox_new(Etk_Bool homogeneous, int spacing)
  Etk_Type *etk_vbox_type_get()
  Etk_Widget *etk_viewport_new()
  Etk_Type *etk_viewport_type_get()
  Etk_Widget *etk_vpaned_new()
  Etk_Type *etk_vpaned_type_get()
  Etk_Widget *etk_vscrollbar_new(double lower, double upper, double value, double step_increment, double page_increment, double page_size)
  Etk_Type *etk_vscrollbar_type_get()
  Etk_Widget *etk_vseparator_new()
  Etk_Type *etk_vseparator_type_get()
  Etk_Widget *etk_vslider_new(double lower, double upper, double value, double step_increment, double page_increment)
  Etk_Type *etk_vslider_type_get()
  Evas_Object *etk_widget_clip_get(Etk_Widget *widget)
  void etk_widget_clip_set(Etk_Widget *widget, Evas_Object *clip)
  void etk_widget_clip_unset(Etk_Widget *widget)
  void etk_widget_clipboard_received(Etk_Widget *widget, Etk_Event_Selection_Request *event)
  Etk_Bool etk_widget_dnd_dest_get(Etk_Widget *widget)
  void etk_widget_dnd_dest_set(Etk_Widget *widget, Etk_Bool on)
  Evas_List *etk_widget_dnd_dest_widgets_get()
  void etk_widget_dnd_drag_data_set(Etk_Widget *widget, const char **types, int num_types, void *data, int data_size)
  Etk_Widget *etk_widget_dnd_drag_widget_get(Etk_Widget *widget)
  void etk_widget_dnd_drag_widget_set(Etk_Widget *widget, Etk_Widget *drag_widget)
  const char **etk_widget_dnd_files_get(Etk_Widget *e, int *num_files)
  Etk_Bool etk_widget_dnd_internal_get(Etk_Widget *widget)
  void etk_widget_dnd_internal_set(Etk_Widget *widget, Etk_Bool on)
  Etk_Bool etk_widget_dnd_source_get(Etk_Widget *widget)
  void etk_widget_dnd_source_set(Etk_Widget *widget, Etk_Bool on)
  const char **etk_widget_dnd_types_get(Etk_Widget *widget, int *num)
  void etk_widget_dnd_types_set(Etk_Widget *widget, const char **types, int num)
  void etk_widget_drag_begin(Etk_Widget *widget)
  void etk_widget_drag_drop(Etk_Widget *widget, Etk_Event_Selection_Request *event)
  void etk_widget_drag_end(Etk_Widget *widget)
  void etk_widget_drag_enter(Etk_Widget *widget)
  void etk_widget_drag_leave(Etk_Widget *widget)
  void etk_widget_drag_motion(Etk_Widget *widget)
  void etk_widget_enter(Etk_Widget *widget)
  void etk_widget_focus(Etk_Widget *widget)
  void etk_widget_geometry_get(Etk_Widget *widget, int *x, int *y, int *w, int *h)
  Etk_Bool etk_widget_has_event_object_get(Etk_Widget *widget)
  void etk_widget_has_event_object_set(Etk_Widget *widget, Etk_Bool has_event_object)
  void etk_widget_hide(Etk_Widget *widget)
  void etk_widget_hide_all(Etk_Widget *widget)
  void etk_widget_inner_geometry_get(Etk_Widget *widget, int *x, int *y, int *w, int *h)
  Etk_Bool etk_widget_is_swallowed(Etk_Widget *widget)
  Etk_Bool etk_widget_is_swallowing_object(Etk_Widget *widget, Evas_Object *object)
  Etk_Bool etk_widget_is_swallowing_widget(Etk_Widget *widget, Etk_Widget *swallowed_widget)
  Etk_Bool etk_widget_is_visible(Etk_Widget *widget)
  void etk_widget_key_event_propagation_stop()
  void etk_widget_leave(Etk_Widget *widget)
  void etk_widget_lower(Etk_Widget *widget)
  Etk_Bool etk_widget_member_object_add(Etk_Widget *widget, Evas_Object *object)
  void etk_widget_member_object_del(Etk_Widget *widget, Evas_Object *object)
  void etk_widget_member_object_lower(Etk_Widget *widget, Evas_Object *object)
  void etk_widget_member_object_raise(Etk_Widget *widget, Evas_Object *object)
  void etk_widget_member_object_stack_above(Etk_Widget *widget, Evas_Object *object, Evas_Object *above)
  void etk_widget_member_object_stack_below(Etk_Widget *widget, Evas_Object *object, Evas_Object *below)
  const char *etk_widget_name_get(Etk_Widget *widget)
  void etk_widget_name_set(Etk_Widget *widget, const char *name)
  Etk_Widget *etk_widget_new(Etk_Type *widget_type, const char *first_property, ...)
  Etk_Widget *etk_widget_parent_get(Etk_Widget *widget)
  void etk_widget_parent_set(Etk_Widget *widget, Etk_Widget *parent)
  void etk_widget_parent_set_full(Etk_Widget *widget, Etk_Widget *parent, Etk_Bool remove_from_container)
  Etk_Bool etk_widget_pass_mouse_events_get(Etk_Widget *widget)
  void etk_widget_pass_mouse_events_set(Etk_Widget *widget, Etk_Bool pass_mouse_events)
  void etk_widget_raise(Etk_Widget *widget)
  void etk_widget_redraw_queue(Etk_Widget *widget)
  Etk_Bool etk_widget_repeat_mouse_events_get(Etk_Widget *widget)
  void etk_widget_repeat_mouse_events_set(Etk_Widget *widget, Etk_Bool repeat_mouse_events)
  void etk_widget_selection_received(Etk_Widget *widget, Etk_Event_Selection_Request *event)
  void etk_widget_show(Etk_Widget *widget)
  void etk_widget_show_all(Etk_Widget *widget)
  void etk_widget_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
  void etk_widget_size_recalc_queue(Etk_Widget *widget)
  void etk_widget_size_request(Etk_Widget *widget, Etk_Size *size_requisition)
  void etk_widget_size_request_full(Etk_Widget *widget, Etk_Size *size_requisition, Etk_Bool hidden_has_no_size)
  void etk_widget_size_request_set(Etk_Widget *widget, int w, int h)
  Etk_Bool etk_widget_swallow_widget(Etk_Widget *swallowing_widget, const char *part, Etk_Widget *widget_to_swallow)
  const char *etk_widget_theme_file_get(Etk_Widget *widget)
  void etk_widget_theme_file_set(Etk_Widget *widget, const char *theme_file)
  const char *etk_widget_theme_group_get(Etk_Widget *widget)
  void etk_widget_theme_group_set(Etk_Widget *widget, const char *theme_group)
  int etk_widget_theme_object_data_get(Etk_Widget *widget, const char *data_name, const char *format, ...)
  void etk_widget_theme_object_min_size_calc(Etk_Widget *widget, int *w, int *h)
  void etk_widget_theme_object_part_text_set(Etk_Widget *widget, const char *part_name, const char *text)
  void etk_widget_theme_object_signal_emit(Etk_Widget *widget, const char *signal_name)
  Etk_Bool etk_widget_theme_object_swallow(Etk_Widget *swallowing_widget, const char *part, Evas_Object *object)
  void etk_widget_theme_object_unswallow(Etk_Widget *swallowing_widget, Evas_Object *object)
  Etk_Widget *etk_widget_theme_parent_get(Etk_Widget *widget)
  void etk_widget_theme_parent_set(Etk_Widget *widget, Etk_Widget *theme_parent)
  Evas *etk_widget_toplevel_evas_get(Etk_Widget *widget)
  Etk_Toplevel_Widget *etk_widget_toplevel_parent_get(Etk_Widget *widget)
  Etk_Type *etk_widget_type_get()
  void etk_widget_unfocus(Etk_Widget *widget)
  void etk_widget_unswallow_widget(Etk_Widget *swallowing_widget, Etk_Widget *widget)
  Etk_Bool etk_widget_visibility_locked_get(Etk_Widget *widget)
  void etk_widget_visibility_locked_set(Etk_Widget *widget, Etk_Bool visibility_locked)
  void etk_window_center_on_window(Etk_Window *window_to_center, Etk_Window *window)
  Etk_Bool etk_window_decorated_get(Etk_Window *window)
  void etk_window_decorated_set(Etk_Window *window, Etk_Bool decorated)
  void etk_window_deiconify(Etk_Window *window)
  void etk_window_dnd_aware_set(Etk_Window *window, Etk_Bool on)
  void etk_window_focus(Etk_Window *window)
  void etk_window_fullscreen(Etk_Window *window)
  void etk_window_geometry_get(Etk_Window *window, int *x, int *y, int *w, int *h)
  Etk_Bool etk_window_hide_on_delete(Etk_Object *window, void *data)
  void etk_window_iconify(Etk_Window *window)
  Etk_Bool etk_window_is_focused(Etk_Window *window)
  Etk_Bool etk_window_is_fullscreen(Etk_Window *window)
  Etk_Bool etk_window_is_iconified(Etk_Window *window)
  Etk_Bool etk_window_is_maximized(Etk_Window *window)
  Etk_Bool etk_window_is_sticky(Etk_Window *window)
  void etk_window_maximize(Etk_Window *window)
  void etk_window_move(Etk_Window *window, int x, int y)
  void etk_window_move_to_mouse(Etk_Window *window)
  Etk_Widget *etk_window_new()
  void etk_window_resize(Etk_Window *window, int w, int h)
  Etk_Bool etk_window_shaped_get(Etk_Window *window)
  void etk_window_shaped_set(Etk_Window *window, Etk_Bool shaped)
  Etk_Bool etk_window_skip_pager_hint_get(Etk_Window *window)
  void etk_window_skip_pager_hint_set(Etk_Window *window, Etk_Bool skip_pager_hint)
  Etk_Bool etk_window_skip_taskbar_hint_get(Etk_Window *window)
  void etk_window_skip_taskbar_hint_set(Etk_Window *window, Etk_Bool skip_taskbar_hint)
  void etk_window_stick(Etk_Window *window)
  const char *etk_window_title_get(Etk_Window *window)
  void etk_window_title_set(Etk_Window *window, const char *title)
  Etk_Type *etk_window_type_get()
  void etk_window_unfocus(Etk_Window *window)
  void etk_window_unfullscreen(Etk_Window *window)
  void etk_window_unmaximize(Etk_Window *window)
  void etk_window_unstick(Etk_Window *window)
  void etk_window_wmclass_set(Etk_Window *window, const char *window_name, const char *window_class)



=head1 SEE ALSO

Etk documentation is available as Doxygen or in the Etk Explained book:
http://hisham.cc/etk_explained.pdf

http://www.enlightenment.org

=head1 AUTHOR

Hisham Mardam Bey, E<lt>hisham.mardambey@gmail.comE<gt>

=head1 COPYRIGHT AND LICENSE

Copyright (C) 2006 by Hisham Mardam Bey

This library is free software; you can redistribute it and/or modify
it under the same terms as Perl itself, either Perl version 5.8.7 or,
at your option, any later version of Perl 5 you may have available.


=cut
