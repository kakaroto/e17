#include "common.h"

#include <Elementary.h>

#include "elementary-const-c.inc"

#ifndef DEBUG
#define DEBUG 0
#endif

MODULE = EFL::Elementary		PACKAGE = EFL::Elementary		

INCLUDE: elementary-const-xs.inc

PROTOTYPES: ENABLE


# General
# ------------------------------------------------------------------------------

void
_elm_init(argc, argv)
    int argc
    char *argv
    CODE:
        elm_init(argc, (char **)argv);

void
elm_shutdown()

void
elm_run()

void
elm_exit()


void
_elm_quicklaunch_init(argc, argv)
    int argc
    char *argv
    CODE:
        elm_quicklaunch_init(argc, (char **)argv);

void
_elm_quicklaunch_sub_init(argc, argv)
    int argc
    char *argv
    CODE:
        elm_quicklaunch_sub_init(argc, (char **)argv);

void
elm_quicklaunch_sub_shutdown()

void
elm_quicklaunch_shutdown()

void
elm_quicklaunch_seed()

Eina_Bool
_elm_quicklaunch_prepare(argc, argv)
    int argc
    char *argv
    CODE:
        RETVAL = elm_quicklaunch_prepare(argc, (char **)argv);
    OUTPUT:
        RETVAL

# TODO
#Eina_Bool
#elm_quicklaunch_fork(argc, argv, cwd, data, postfork_data)
#    int argc
#    char **argv
#    char *cwd
#    void *data
#    void *postfork_data
#    CODE:
#        RETVAL =  elm_quicklaunch_fork(argc, argv, cwd, data, postfork_data);
#    OUTPUT:
#        RETVAL

void
elm_quicklaunch_cleanup()

int
_elm_quicklaunch_fallback(argc, argv);
    int argc
    char *argv
    CODE:
        RETVAL = elm_quicklaunch_fallback(argc, (char **)argv);
    OUTPUT:
        RETVAL

char *
elm_quicklaunch_exe_path_get(const char *exe);

void
elm_need_efreet()

void
elm_need_e_dbus()

Eina_Bool
elm_policy_set(unsigned int policy, int value)

int
elm_policy_get(unsigned int policy)

void
elm_object_scale_set(Evas_Object *obj, double scale)

double
elm_object_scale_get(const Evas_Object *obj);

void
elm_object_style_set(Evas_Object *obj, const char *style)

const char *
elm_object_style_get(const Evas_Object *obj)

void
elm_object_disabled_set(Evas_Object *obj, Eina_Bool disabled)

Eina_Bool
elm_object_disabled_get(const Evas_Object *obj)

double
elm_scale_get()

void
elm_scale_set(double scale)

Evas_Coord
elm_finger_size_get()

void
elm_finger_size_set(Evas_Coord size)

void
elm_object_focus(Evas_Object *obj)

void
elm_object_focus_allow_set(Evas_Object *obj, Eina_Bool enable)

Eina_Bool
elm_object_focus_allow_get(Evas_Object *obj);

void
elm_object_scroll_hold_push(Evas_Object *obj)

void
elm_object_scroll_hold_pop(Evas_Object *obj)

void
elm_object_scroll_freeze_push(Evas_Object *obj)

void
elm_object_scroll_freeze_pop(Evas_Object *obj)

void
elm_coords_finger_size_adjust(int times_w, Evas_Coord *w, int times_h, Evas_Coord *h)

void
elm_theme_overlay_add(const char *item)

void
elm_theme_overlay_del(const char *item)

void
elm_theme_extension_add(const char *item)

void
elm_theme_extension_del(const char *item)

void
elm_theme_flush()


# win
# ------------------------------------------------------------------------------

Evas_Object *
elm_win_add(Evas_Object *parent, const char *name, Elm_Win_Type type)

void
elm_win_resize_object_add(Evas_Object *win, Evas_Object *bg)

void
elm_win_resize_object_del(Evas_Object *obj, Evas_Object *subobj)

void
elm_win_title_set(Evas_Object *obj, const char *title)

void
elm_win_autodel_set(Evas_Object *obj, Eina_Bool autodel)

void
elm_win_activate(Evas_Object *obj)

void
elm_win_lower(Evas_Object *obj)

void
elm_win_raise(Evas_Object *obj)

void
elm_win_borderless_set(Evas_Object *obj, Eina_Bool borderless)

void
elm_win_borderless_get(Evas_Object *obj)

void
elm_win_shaped_set(Evas_Object *obj, Eina_Bool shaped)

Eina_Bool
elm_win_shaped_get(Evas_Object *obj)

void
elm_win_alpha_set(Evas_Object *obj, Eina_Bool alpha)

Eina_Bool
elm_win_alpha_get(Evas_Object *obj)

void
elm_win_override_set(Evas_Object *obj, Eina_Bool override)

Eina_Bool
elm_win_override_get(Evas_Object *obj)

void
elm_win_fullscreen_set(Evas_Object *obj, Eina_Bool fullscreen)

Eina_Bool
elm_win_fullscreen_get(Evas_Object *obj)

void
elm_win_maximized_set(Evas_Object *obj, Eina_Bool maximized);

Eina_Bool
elm_win_maximized_get(Evas_Object *obj)

void
elm_win_iconified_set(Evas_Object *obj, Eina_Bool iconified)

Eina_Bool
elm_win_iconified_get(Evas_Object *obj)

void
elm_win_layer_set(Evas_Object *obj, int layer)

int
elm_win_layer_get(Evas_Object *obj);

void
elm_win_rotation_set(Evas_Object *obj, int rotation);

int
elm_win_rotation_get(Evas_Object *obj)

void
elm_win_sticky_set(Evas_Object *obj, Eina_Bool sticky)

Eina_Bool
elm_win_sticky_get(Evas_Object *obj)

void
elm_win_keyboard_mode_set(Evas_Object *obj, Elm_Win_Keyboard_Mode mode)

void
elm_win_keyboard_win_set(Evas_Object *obj, Eina_Bool is_keyboard)

# TODO pointers
void
_elm_win_screen_position_get(obj, x, y)
     Evas_Object *obj
     int &x
     int &y
     CODE:
        elm_win_screen_position_get(obj, &x, &y);
     OUTPUT:
        x
        y

Evas_Object *
elm_win_inwin_add(Evas_Object *obj)

void
elm_win_inwin_activate(Evas_Object *obj)

void
elm_win_inwin_content_set(Evas_Object *obj, Evas_Object *content)

# TODO elm_win_xwindow_get


# bg
# ------------------------------------------------------------------------------

Evas_Object *
elm_bg_add(Evas_Object *parent)

void
elm_bg_file_set(Evas_Object *obj, const char *file, const char *group)


# icon
# ------------------------------------------------------------------------------

Evas_Object *
elm_icon_add(Evas_Object *parent)

Eina_Bool
elm_icon_file_set(Evas_Object *obj, const char *file, const char *group)

void
elm_icon_standard_set(Evas_Object *obj, const char *name)

void
elm_icon_smooth_set(Evas_Object *obj, Eina_Bool smooth)

void
elm_icon_no_scale_set(Evas_Object *obj, Eina_Bool no_scale)

void
elm_icon_scale_set(Evas_Object *obj, Eina_Bool scale_up, Eina_Bool scale_down)

void
elm_icon_fill_outside_set(Evas_Object *obj, Eina_Bool fill_outside);

void
elm_icon_prescale_set(Evas_Object *obj, int size)


# image
# ------------------------------------------------------------------------------

Evas_Object *
elm_image_add(Evas_Object *parent)

Eina_Bool
elm_image_file_set(Evas_Object *obj, const char *file, const char *group)

void
elm_image_smooth_set(Evas_Object *obj, Eina_Bool smooth)

void
elm_image_no_scale_set(Evas_Object *obj, Eina_Bool no_scale)

# TODO
void
elm_image_object_size_get(obj, w, h)
    Evas_Object *obj
    int w = NO_INIT
    int h = NO_INIT
    CODE:
        elm_image_object_size_get(obj, &w, &h);
    OUTPUT:
        w
        h

void
elm_image_scale_set(Evas_Object *obj, Eina_Bool scale_up, Eina_Bool scale_down)

void
elm_image_fill_outside_set(Evas_Object *obj, Eina_Bool fill_outside)

void
elm_image_prescale_set(Evas_Object *obj, int size)

void
elm_image_orient_set(Evas_Object *obj, Elm_Image_Orient orient)


# box
# ------------------------------------------------------------------------------

Evas_Object *
elm_box_add(Evas_Object *parent)

void
elm_box_horizontal_set(Evas_Object *obj, Eina_Bool horizontal)

void
elm_box_homogeneous_set(Evas_Object *obj, Eina_Bool homogeneous)

void
elm_box_homogenous_set(Evas_Object *obj, Eina_Bool homogenous)

void
elm_box_pack_start(Evas_Object *obj, Evas_Object *subobj)

void
elm_box_pack_end(Evas_Object *obj, Evas_Object *subobj)

void
elm_box_pack_before(Evas_Object *obj, Evas_Object *subobj, Evas_Object *before)

void
elm_box_pack_after(Evas_Object *obj, Evas_Object *subobj, Evas_Object *after)

void
elm_box_clear(Evas_Object *obj)

void
elm_box_unpack(Evas_Object *obj, Evas_Object *subobj)

void
elm_box_unpack_all(Evas_Object *obj)


# button
# ------------------------------------------------------------------------------

Evas_Object *
elm_button_add(Evas_Object *parent)

void
elm_button_label_set(Evas_Object *obj, const char *label)

const char *
elm_button_label_get(Evas_Object *obj)

void
elm_button_icon_set(Evas_Object *obj, Evas_Object *icon)

Evas_Object *
elm_button_icon_get(Evas_Object *obj)


# scroller
# ------------------------------------------------------------------------------

Evas_Object *
elm_scroller_add(Evas_Object *parent)

void
elm_scroller_content_set(Evas_Object *obj, Evas_Object *child)

void
elm_scroller_content_min_limit(Evas_Object *obj, Evas_Coord w, Evas_Coord h)

void
elm_scroller_region_show(Evas_Object *obj, Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h)

void
elm_scroller_policy_set(Evas_Object *obj, Elm_Scroller_Policy policy_h, Elm_Scroller_Policy policy_v)

# TODO
void
elm_scroller_region_get(obj, x, y, w, h)
    Evas_Object *obj
    Evas_Coord x = NO_INIT
    Evas_Coord y = NO_INIT
    Evas_Coord w = NO_INIT
    Evas_Coord h = NO_INIT
    CODE:
        elm_scroller_region_get(obj, &x, &y, &w, &h);

# TODO
void
elm_scroller_child_size_get(obj, w, h)
    Evas_Object *obj
    Evas_Coord w = NO_INIT
    Evas_Coord h = NO_INIT
    CODE:
        elm_scroller_child_size_get(obj, &w, &h);

void
elm_scroller_bounce_set(Evas_Object *obj, Eina_Bool h_bounce, Eina_Bool v_bounce)

void
elm_scroller_page_relative_set(Evas_Object *obj, double h_pagerel, double v_pagerel)

void
elm_scroller_page_size_set(Evas_Object *obj, Evas_Coord h_pagesize, Evas_Coord v_pagesize)

void
elm_scroller_region_bring_in(Evas_Object *obj, Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h)


# label
# ------------------------------------------------------------------------------

Evas_Object *
elm_label_add(Evas_Object *parent)

void
elm_label_label_set(Evas_Object *obj, const char *label)

const char *
elm_label_label_get(Evas_Object *obj)

void
elm_label_line_wrap_set(Evas_Object *obj, Eina_Bool wrap)


# toggle
# ------------------------------------------------------------------------------

Evas_Object *
elm_toggle_add(Evas_Object *parent)

void
elm_toggle_label_set(Evas_Object *obj, const char *label)

const char *
elm_toggle_label_get(Evas_Object *obj)

void
elm_toggle_icon_set(Evas_Object *obj, Evas_Object *icon)

Evas_Object *
elm_toggle_icon_get(Evas_Object *obj)

void
elm_toggle_states_labels_set(Evas_Object *obj, const char *onlabel, const char *offlabel)

void
elm_toggle_state_set(Evas_Object *obj, Eina_Bool state)

Eina_Bool
elm_toggle_state_get(const Evas_Object *obj)

void
elm_toggle_state_pointer_set(Evas_Object *obj, Eina_Bool *statep)


# frame
# ------------------------------------------------------------------------------

Evas_Object *
elm_frame_add(Evas_Object *parent)

void
elm_frame_label_set(Evas_Object *obj, const char *label)

const char *
elm_frame_label_get(Evas_Object *obj)

void
elm_frame_content_set(Evas_Object *obj, Evas_Object *content)


# table
# ------------------------------------------------------------------------------

Evas_Object *
elm_table_add(Evas_Object *parent)

void
elm_table_homogenous_set(Evas_Object *obj, Eina_Bool homogenous)

void
elm_table_pack(Evas_Object *obj, Evas_Object *subobj, int x, int y, int w, int h)

void
elm_table_padding_set(Evas_Object *obj, Evas_Coord horizontal, Evas_Coord vertical)


# clock
# ------------------------------------------------------------------------------

Evas_Object *
elm_clock_add(Evas_Object *parent)

void
elm_clock_time_set(Evas_Object *obj, int hrs, int min, int sec)

# TODO
void
elm_clock_time_get(obj, hrs, min, sec)
    Evas_Object *obj
    int hrs = NO_INIT
    int min = NO_INIT
    int sec = NO_INIT
    CODE:
        elm_clock_time_get(obj, &hrs, &min, &sec);
    OUTPUT:
        hrs
        min
        sec

void
elm_clock_edit_set(Evas_Object *obj, Eina_Bool edit)

void
elm_clock_show_am_pm_set(Evas_Object *obj, Eina_Bool am_pm)

void
elm_clock_show_seconds_set(Evas_Object *obj, Eina_Bool seconds)


# layout
# ------------------------------------------------------------------------------

Evas_Object *
elm_layout_add(Evas_Object *parent);

Eina_Bool
elm_layout_file_set(Evas_Object *obj, const char *file, const char *group)

void
elm_layout_content_set(Evas_Object *obj, const char *swallow, Evas_Object *content)

Evas_Object *
elm_layout_edje_get(Evas_Object *obj);

void
elm_layout_sizing_eval(Evas_Object *obj)


# notify
# ------------------------------------------------------------------------------

Evas_Object *
elm_notify_add(Evas_Object *parent)

void
elm_notify_content_set(Evas_Object *obj, Evas_Object *content)

void
elm_notify_orient_set(Evas_Object *obj, Elm_Notify_Orient orient)

void
elm_notify_timeout_set(Evas_Object *obj, int timeout)

void
elm_notify_timer_init(Evas_Object *obj)

void
elm_notify_repeat_events_set(Evas_Object *obj, Eina_Bool repeat)

void
elm_notify_parent_set(Evas_Object *obj, Evas_Object *parent)


# hover
# ------------------------------------------------------------------------------

Evas_Object *
elm_hover_add(Evas_Object *parent)

void
elm_hover_target_set(Evas_Object *obj, Evas_Object *target)

void
elm_hover_parent_set(Evas_Object *obj, Evas_Object *parent)

void
elm_hover_content_set(Evas_Object *obj, const char *swallow, Evas_Object *content)

const char *
elm_hover_best_content_location_get(const Evas_Object *obj, Elm_Hover_Axis pref_axis)


# entry
# ------------------------------------------------------------------------------

Evas_Object *
elm_entry_add(Evas_Object *parent)

void
elm_entry_single_line_set(Evas_Object *obj, Eina_Bool single_line)

void
elm_entry_password_set(Evas_Object *obj, Eina_Bool password)

void
elm_entry_entry_set(Evas_Object *obj, const char *entry)

const char *
elm_entry_entry_get(Evas_Object *obj)

const char *
elm_entry_selection_get(Evas_Object *obj)

void
elm_entry_entry_insert(Evas_Object *obj, const char *entry)

void
elm_entry_line_wrap_set(Evas_Object *obj, Eina_Bool wrap)

void
elm_entry_line_char_wrap_set(Evas_Object *obj, Eina_Bool wrap)

void
elm_entry_editable_set(Evas_Object *obj, Eina_Bool editable)

void
elm_entry_select_none(Evas_Object *obj)

void
elm_entry_select_all(Evas_Object *obj)

Eina_Bool
elm_entry_cursor_next(Evas_Object *obj)

Eina_Bool
elm_entry_cursor_prev(Evas_Object *obj)

Eina_Bool
elm_entry_cursor_up(Evas_Object *obj)

Eina_Bool
elm_entry_cursor_down(Evas_Object *obj)

void
elm_entry_cursor_begin_set(Evas_Object *obj)

void
elm_entry_cursor_end_set(Evas_Object *obj)

void
elm_entry_cursor_line_begin_set(Evas_Object *obj)

void
elm_entry_cursor_line_end_set(Evas_Object *obj)

void
elm_entry_cursor_selection_begin(Evas_Object *obj)

void
elm_entry_cursor_selection_end(Evas_Object *obj)

Eina_Bool
elm_entry_cursor_is_format_get(Evas_Object *obj)

Eina_Bool
elm_entry_cursor_is_visible_format_get(Evas_Object *obj)

const char *
elm_entry_cursor_content_get(Evas_Object *obj)

void
elm_entry_selection_cut(Evas_Object *obj)

void
elm_entry_selection_copy(Evas_Object *obj)

void
elm_entry_selection_paste(Evas_Object *obj)

void
elm_entry_context_menu_clear(Evas_Object *obj)

void
elm_entry_context_menu_item_add(obj, label, icon_file, icon_type, func, data)
    Evas_Object *obj
    const char *label
    const char *icon_file
    Elm_Icon_Type icon_type
    SV *func
    SV *data
    CODE:
        _saved_callback *sc = NULL;
        Newx(sc, 1, _saved_callback);

        if (DEBUG) {
            fprintf(stderr, "elm_entry_context_menu_item_add() func:%p, data:%p, sc:%p\n", func, data, sc);
        }

        sc->func = SvRV((SV *)func);
        sc->data = data;

        elm_entry_context_menu_item_add(obj, label, icon_file, icon_type, call_perl_sub, sc);

void
elm_entry_context_menu_disabled_set(Evas_Object *obj, Eina_Bool disabled)

Eina_Bool
elm_entry_context_menu_disabled_get(Evas_Object *obj)

char *
elm_entry_markup_to_utf8(const char *s)

char *
elm_entry_utf8_to_markup(const char *s)


# notepad
# ------------------------------------------------------------------------------

Evas_Object *
elm_notepad_add(Evas_Object *parent)

void
elm_notepad_file_set(Evas_Object *obj, char *file, Elm_Text_Format format)


# anchorview
# ------------------------------------------------------------------------------

Evas_Object *
elm_anchorview_add(Evas_Object *parent);

void
elm_anchorview_text_set(Evas_Object *obj, const char *text)

void
elm_anchorview_hover_parent_set(Evas_Object *obj, Evas_Object *parent)

void
elm_anchorview_hover_style_set(Evas_Object *obj, const char *style)

void
elm_anchorview_hover_end(Evas_Object *obj)


# anchorblock
# ------------------------------------------------------------------------------

Evas_Object *
elm_anchorblock_add(Evas_Object *parent)

void
elm_anchorblock_text_set(Evas_Object *obj, const char *text)

void
elm_anchorblock_hover_parent_set(Evas_Object *obj, Evas_Object *parent)

void
elm_anchorblock_hover_style_set(Evas_Object *obj, const char *style);

void
elm_anchorblock_hover_end(Evas_Object *obj)


# bubble
# ------------------------------------------------------------------------------

Evas_Object *
elm_bubble_add(Evas_Object *parent)

void
elm_bubble_label_set(Evas_Object *obj, const char *label)

const char *
elm_bubble_label_get(Evas_Object *obj)

void
elm_bubble_info_set(Evas_Object *obj, const char *info)

void
elm_bubble_content_set(Evas_Object *obj, Evas_Object *content)

void
elm_bubble_icon_set(Evas_Object *obj, Evas_Object *icon)

Evas_Object *
elm_bubble_icon_get(Evas_Object *obj)

void
elm_bubble_corner_set(Evas_Object *obj, const char *corner)


# photo
# ------------------------------------------------------------------------------

Evas_Object *
elm_photo_add(Evas_Object *parent)

Eina_Bool
elm_photo_file_set(Evas_Object *obj, const char *file)

void
elm_photo_size_set(Evas_Object *obj, int size)


# hoversel
# ------------------------------------------------------------------------------

Evas_Object *
elm_hoversel_add(Evas_Object *parent)

void
elm_hoversel_hover_parent_set(Evas_Object *obj, Evas_Object *parent)

void
elm_hoversel_label_set(Evas_Object *obj, const char *label)

const char *
elm_hoversel_label_get(Evas_Object *obj)

void
elm_hoversel_icon_set(Evas_Object *obj, Evas_Object *icon)

Evas_Object *
elm_hoversel_icon_get(Evas_Object *obj)

void
elm_hoversel_hover_begin(Evas_Object *obj)

void
elm_hoversel_hover_end(Evas_Object *obj)

void
elm_hoversel_clear(Evas_Object *obj)

# TODO
#const Eina_List *
#elm_hoversel_items_get(obj)
#    Evas_Object *obj
#    CODE:
#        RETVAL = elm_hoversel_items_get(obj);
#    OUTPUT:
#        RETVAL


Elm_Hoversel_Item *
elm_hoversel_item_add(obj, label, icon_file, icon_type, func, data)
    Evas_Object *obj
    const char *label
    const char *icon_file
    Elm_Icon_Type icon_type
    SV *func
    SV *data
    PREINIT:
        _saved_callback *sc = NULL;
    CODE:
        sc = perl_save_callback_new(func, data);

        if (DEBUG) {
            fprintf(stderr, "elm_hoversel_item_add() func:%p, data:%p, sc:%p\n", func, data, sc);
        }

        RETVAL = elm_hoversel_item_add(obj, label, icon_file, icon_type, call_perl_sub, sc);
    OUTPUT:
        RETVAL

void
elm_hoversel_item_del(Elm_Hoversel_Item *item)

void *
elm_hoversel_item_data_get(Elm_Hoversel_Item *it)

const char *
elm_hoversel_item_label_get(Elm_Hoversel_Item *it)

void
elm_hoversel_item_icon_set(Elm_Hoversel_Item *it, const char *icon_file, const char *icon_group, Elm_Icon_Type icon_type)

# TODO const char **
void
elm_hoversel_item_icon_get(Elm_Hoversel_Item *it, const char **icon_file, const char **icon_group, Elm_Icon_Type *icon_type)


# toolbar
# ------------------------------------------------------------------------------

Evas_Object *
elm_toolbar_add(Evas_Object *parent)

void
elm_toolbar_icon_size_set(Evas_Object *obj, int icon_size)

void
elm_toolbar_align_set(Evas_Object *obj, double align)

int
elm_toolbar_icon_size_get(Evas_Object *obj)

void
elm_toolbar_item_unselect_all(Evas_Object *obj)

Elm_Toolbar_Item *
elm_toolbar_item_add(obj, icon, label, func, data)
    Evas_Object *obj
    Evas_Object *icon
    const char *label
    SV *func
    SV *data
    PREINIT:
        _saved_callback *sc = NULL;        
    CODE:
        sc = perl_save_callback_new(func, data);

        if (DEBUG) {
            fprintf(stderr, "elm_toolbar_item_add() func:%p, data:%p, sc:%p\n", func, data, sc);
        }

        RETVAL = elm_toolbar_item_add(obj, icon, label, call_perl_sub, sc);
    OUTPUT:
        RETVAL

Evas_Object *
elm_toolbar_item_icon_get(Elm_Toolbar_Item *item)

const char *
elm_toolbar_item_label_get(Elm_Toolbar_Item *item)

void
elm_toolbar_item_label_set(Elm_Toolbar_Item *item, const char *label)

void
elm_toolbar_item_del(Elm_Toolbar_Item *item)

void
elm_toolbar_item_select(Elm_Toolbar_Item *item)

Eina_Bool
elm_toolbar_item_disabled_get(Elm_Toolbar_Item *item)

void
elm_toolbar_item_disabled_set(Elm_Toolbar_Item *item, Eina_Bool disabled)

void
elm_toolbar_item_separator_set(Elm_Toolbar_Item *item, Eina_Bool separator)

Eina_Bool
elm_toolbar_item_separator_get(Elm_Toolbar_Item *item)

void
elm_toolbar_item_menu_set(Elm_Toolbar_Item *item, Eina_Bool menu)

Evas_Object *
elm_toolbar_item_menu_get(Elm_Toolbar_Item *item)

void
elm_toolbar_menu_parent_set(Evas_Object *obj, Evas_Object *parent)

void
elm_toolbar_scrollable_set(Evas_Object *obj, Eina_Bool scrollable)

void
elm_toolbar_homogenous_set(Evas_Object *obj, Eina_Bool homogenous)


# menu
# ------------------------------------------------------------------------------

Evas_Object *
elm_menu_add(Evas_Object *parent)

void
elm_menu_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y)

void
elm_menu_parent_set(Evas_Object *obj, Evas_Object *parent)

Elm_Menu_Item *
elm_menu_item_add(obj, parent, icon, label, func, data)
    Evas_Object *obj
    Elm_Menu_Item *parent
    Evas_Object *icon
    const char *label
    SV *func
    SV *data
    PREINIT:
        _saved_callback *sc = NULL;
    CODE:
        sc = perl_save_callback_new(func, data);

        if (DEBUG) {
            fprintf(stderr, "elm_menu_item_add() func:%p, data:%p, sc:%p\n", func, data, sc);
        }

        RETVAL = elm_menu_item_add(obj, parent, icon, label, call_perl_sub, sc);
    OUTPUT:
        RETVAL

Elm_Menu_Item *
elm_menu_item_separator_add(Evas_Object *obj, Elm_Menu_Item *parent)

void
elm_menu_item_label_set(Elm_Menu_Item *item, const char *label)

const char *
elm_menu_item_label_get(Elm_Menu_Item *item)

void
elm_menu_item_icon_set(Elm_Menu_Item *item, Evas_Object *icon)

void
elm_menu_item_disabled_set(Elm_Menu_Item *item, Eina_Bool disabled)

void
elm_menu_item_del(Elm_Menu_Item *item)

Evas_Object *
elm_menu_object_get(const Elm_Menu_Item *it)

const Eina_List *
elm_menu_item_subitems_get(Elm_Menu_Item *item)

# list
# ------------------------------------------------------------------------------

Evas_Object *
elm_list_add(Evas_Object *parent)

Elm_List_Item *
elm_list_item_append(obj, label, icon, end, func, data)
    Evas_Object *obj
    const char *label
    Evas_Object *icon
    Evas_Object *end
    SV *func
    SV *data
    PREINIT:
        _saved_callback *sc = NULL;

    CODE:
        sc = perl_save_callback_new(func, data);

        /*
        if (SvROK(func)) {
            sc->func = SvRV((SV *)func);
        }
        else {
            sc->func = NULL;
        }

        if (data) {
            sc->data = data;
        }
        else {
            sc->data = NULL;
        }*/

        if (DEBUG) {
            fprintf(stderr, "elm_list_item_append() func:%p, data:%p, sc:%p\n", (SV *)func, data, sc);
        }

        RETVAL = elm_list_item_append(obj, label, icon, end, call_perl_sub, sc);
    OUTPUT:
        RETVAL

# TODO
#   EAPI Elm_List_Item   *elm_list_item_prepend(Evas_Object *obj, const char *label, Evas_Object *icon, Evas_Object *end, void (*func) (void *data, Evas_Object *obj, void *event_info), const void *data);
#  EAPI Elm_List_Item   *elm_list_item_insert_before(Evas_Object *obj, Elm_List_Item *before, const char *label, Evas_Object *icon, Evas_Object *end, void (*func) (void *data, Evas_Object *obj, void *event_info), const void *data);
#   EAPI Elm_List_Item   *elm_list_item_insert_after(Evas_Object *obj, Elm_List_Item *after, const char *label, Evas_Object *icon, Evas_Object *end, void (*func) (void *data, Evas_Object *obj, void *event_info), const void *data);

void
elm_list_clear(Evas_Object *obj)

void
elm_list_go(Evas_Object *li)

void
elm_list_multi_select_set(Evas_Object *obj, Eina_Bool multi)

void
elm_list_horizontal_mode_set(Evas_Object *obj, Elm_List_Mode mode)

void
elm_list_always_select_mode_set(Evas_Object *obj, Eina_Bool always_select)

const Eina_List *
elm_list_items_get(const Evas_Object *obj)

Elm_List_Item *
elm_list_selected_item_get(const Evas_Object *obj)

const Eina_List *
elm_list_selected_items_get(const Evas_Object *obj)

void
elm_list_item_selected_set(Elm_List_Item *item, Eina_Bool selected)

void
elm_list_item_show(Elm_List_Item *item)

void
elm_list_item_del(Elm_List_Item *item)


# TODO
# void
#elm_list_item_del_cb_set(Elm_List_Item *item, void (*func)(void *data, Evas_Object *obj, void *event_info));

void *
elm_list_item_data_get(const Elm_List_Item *item)

Evas_Object *
elm_list_item_icon_get(const Elm_List_Item *item)

void
elm_list_item_icon_set(Elm_List_Item *item, Evas_Object *icon)

Evas_Object *
elm_list_item_end_get(const Elm_List_Item *item)

void
elm_list_item_end_set(Elm_List_Item *item, Evas_Object *end)

Evas_Object *
elm_list_item_object_get(const Elm_List_Item *item)

const char *
elm_list_item_label_get(const Elm_List_Item *item)

void
elm_list_item_label_set(Elm_List_Item *item, const char *text)

Elm_List_Item *
elm_list_item_prev(const Elm_List_Item *it)

Elm_List_Item *
elm_list_item_next(const Elm_List_Item *it)


# slider
# ------------------------------------------------------------------------------

Evas_Object *
elm_slider_add(Evas_Object *parent)

void
elm_slider_label_set(Evas_Object *obj, const char *label)

const char *
elm_slider_label_get(Evas_Object *obj)

void
elm_slider_icon_set(Evas_Object *obj, Evas_Object *icon)

Evas_Object *
elm_slider_icon_get(Evas_Object *obj)

void
elm_slider_span_size_set(Evas_Object *obj, Evas_Coord size)

void
elm_slider_unit_format_set(Evas_Object *obj, const char *format)

void
elm_slider_indicator_format_set(Evas_Object *obj, const char *indicator)

void
elm_slider_horizontal_set(Evas_Object *obj, Eina_Bool horizontal)

void
elm_slider_min_max_set(Evas_Object *obj, double min, double max)

void
elm_slider_value_set(Evas_Object *obj, double val)

double
elm_slider_value_get(const Evas_Object *obj)

void
elm_slider_inverted_set(Evas_Object *obj, Eina_Bool inverted)


# genlist
# ------------------------------------------------------------------------------

Evas_Object *
elm_genlist_add(Evas_Object *parent)

Elm_Genlist_Item *
elm_genlist_item_append(obj, itc, data, parent, flags, func, func_data)
    Evas_Object *obj
    Elm_Genlist_Item_Class *itc
    void *data
    Elm_Genlist_Item *parent
    Elm_Genlist_Item_Flags flags
    SV *func
    SV *func_data
    PREINIT:
        _saved_callback *sc = NULL;
    CODE:
        sc = perl_save_callback_new(func, func_data);

        if (DEBUG) {
            fprintf(stderr, "elm_genlist_item_append() func:%p, data:%p, sc:%p\n", func, data, sc);
        }

        RETVAL = elm_genlist_item_append(obj, itc, data, parent, flags, call_perl_sub, sc);
    OUTPUT:
        RETVAL


=pod
TODO
Elm_Genlist_Item *
elm_genlist_item_prepend(Evas_Object *obj, Elm_Genlist_Item_Class *itc, void *data, Elm_Genlist_Item *parent, Elm_Genlist_Item_Flags flags, void (*func) (void *data, Evas_Object *obj, void *event_info), void *func_data);

Elm_Genlist_Item *
elm_genlist_item_insert_before(Evas_Object *obj, Elm_Genlist_Item_Class *itc, void *data, Elm_Genlist_Item *parent, Elm_Genlist_Item *before, Elm_Genlist_Item_Flags flags, void (*func) (void *data, Evas_Object *obj, void *event_info), void *func_data);

Elm_Genlist_Item *
elm_genlist_item_insert_after(Evas_Object *obj, Elm_Genlist_Item_Class *itc, void *data, Elm_Genlist_Item *parent, Elm_Genlist_Item *after, Elm_Genlist_Item_Flags flags, void (*func) (void *data, Evas_Object *obj, void *event_info), void *func_data);

=cut

=pod

void
elm_genlist_clear(Evas_Object *obj)

void
elm_genlist_multi_select_set(Evas_Object *obj, Eina_Bool multi)

Elm_Genlist_Item *
elm_genlist_selected_item_get(Evas_Object *obj)

Eina_List *
elm_genlist_selected_items_get(Evas_Object *obj)

Elm_Genlist_Item *
elm_genlist_at_xy_item_get(Evas_Object *obj, Evas_Coord x, Evas_Coord y, int *posret)

Elm_Genlist_Item *
elm_genlist_first_item_get(Evas_Object *obj)

Elm_Genlist_Item *
elm_genlist_last_item_get(Evas_Object *obj)

Elm_Genlist_Item *
elm_genlist_item_next_get(Elm_Genlist_Item *item)

Elm_Genlist_Item *
elm_genlist_item_prev_get(Elm_Genlist_Item *item)

Evas_Object *
elm_genlist_item_genlist_get(Elm_Genlist_Item *item)

Elm_Genlist_Item *
elm_genlist_item_parent_get(Elm_Genlist_Item *it)

void
elm_genlist_item_subitems_clear(Elm_Genlist_Item *item)

void
elm_genlist_item_selected_set(Elm_Genlist_Item *item, Eina_Bool selected)

Eina_Bool
elm_genlist_item_selected_get(Elm_Genlist_Item *item)

void
elm_genlist_item_expanded_set(Elm_Genlist_Item *item, Eina_Bool expanded)

Eina_Bool
elm_genlist_item_expanded_get(Elm_Genlist_Item *item)

void
elm_genlist_item_disabled_set(Elm_Genlist_Item *item, Eina_Bool disabled)

Eina_Bool
elm_genlist_item_disabled_get(Elm_Genlist_Item *item)

void
elm_genlist_item_show(Elm_Genlist_Item *item)

void
elm_genlist_item_bring_in(Elm_Genlist_Item *item)

void
elm_genlist_item_top_show(Elm_Genlist_Item *item)

void
elm_genlist_item_top_bring_in(Elm_Genlist_Item *item)

void
elm_genlist_item_middle_show(Elm_Genlist_Item *it)

void
elm_genlist_item_middle_bring_in(Elm_Genlist_Item *it)

void
elm_genlist_item_del(Elm_Genlist_Item *item)

void *
elm_genlist_item_data_get(Elm_Genlist_Item *item)

void
elm_genlist_item_data_set(Elm_Genlist_Item *it, void *data)

Evas_Object *
elm_genlist_item_object_get(Elm_Genlist_Item *it)

void
elm_genlist_item_update(Elm_Genlist_Item *item)

void
elm_genlist_horizontal_mode_set(Evas_Object *obj, Elm_List_Mode mode)

void
elm_genlist_always_select_mode_set(Evas_Object *obj, Eina_Bool always_select)

void
elm_genlist_no_select_mode_set(Evas_Object *obj, Eina_Bool no_select)

void
elm_genlist_compress_mode_set(Evas_Object *obj, Eina_Bool compress)

void
elm_genlist_bounce_set(Evas_Object *obj, Eina_Bool h_bounce, Eina_Bool v_bounce)

=cut


# check
# ------------------------------------------------------------------------------

Evas_Object *
elm_check_add(Evas_Object *parent)

void
elm_check_label_set(Evas_Object *obj, const char *label)

const char *
elm_check_label_get(Evas_Object *obj)

void
elm_check_icon_set(Evas_Object *obj, Evas_Object *icon)

Evas_Object *
elm_check_icon_get(Evas_Object *obj)

void
elm_check_state_set(Evas_Object *obj, Eina_Bool state)

Eina_Bool
elm_check_state_get(const Evas_Object *obj)

void
elm_check_state_pointer_set(Evas_Object *obj, Eina_Bool *statep)

# radio
# ------------------------------------------------------------------------------

Evas_Object *
elm_radio_add(Evas_Object *parent)

void
elm_radio_label_set(Evas_Object *obj, const char *label)

const char *
elm_radio_label_get(Evas_Object *obj)

void
elm_radio_icon_set(Evas_Object *obj, Evas_Object *icon)

Evas_Object *
elm_radio_icon_get(Evas_Object *obj)

void
elm_radio_group_add(Evas_Object *obj, Evas_Object *group)

void
elm_radio_state_value_set(Evas_Object *obj, int value)

void
elm_radio_value_set(Evas_Object *obj, int value)

int
elm_radio_value_get(const Evas_Object *obj)

#void
#elm_radio_value_pointer_set(Evas_Object *obj, int *valuep)


# pager
# ------------------------------------------------------------------------------

Evas_Object *
elm_pager_add(Evas_Object *parent)

void
elm_pager_content_push(Evas_Object *obj, Evas_Object *content)

void
elm_pager_content_pop(Evas_Object *obj)

void
elm_pager_content_promote(Evas_Object *obj, Evas_Object *content)

Evas_Object *
elm_pager_content_bottom_get(Evas_Object *obj)

Evas_Object *
elm_pager_content_top_get(Evas_Object *obj)


# slideshow
# ------------------------------------------------------------------------------

Evas_Object *
elm_slideshow_add(Evas_Object *parent)

Elm_Slideshow_Item *
elm_slideshow_item_add(Evas_Object *obj, const Elm_Slideshow_Item_Class *itc, const void *data)

void
elm_slideshow_show(Elm_Slideshow_Item *item)

void
elm_slideshow_next(Evas_Object *obj)

void
elm_slideshow_previous(Evas_Object *obj)

const Eina_List *
elm_slideshow_transitions_get(Evas_Object *obj)

void
elm_slideshow_transition_set(Evas_Object *obj, const char *s)

void
elm_slideshow_timeout_set(Evas_Object *obj ,int timeout)

int
elm_slideshow_timeout_get(Evas_Object *obj)

void
elm_slideshow_loop_set(Evas_Object *obj, int loop)

void
elm_slideshow_clear(Evas_Object *obj)

const Eina_List *
elm_slideshow_items_get(const Evas_Object *obj)

void
elm_slideshow_item_del(Elm_Slideshow_Item *item)

Elm_Slideshow_Item *
elm_slideshow_item_current_get(Evas_Object *obj)

Evas_Object *
elm_slideshow_item_object_get(Elm_Slideshow_Item* item)


# fileselector
# ------------------------------------------------------------------------------

Evas_Object *
elm_fileselector_add(Evas_Object *parent)

void
elm_fileselector_is_save_set(Evas_Object *obj, Eina_Bool is_save)

Eina_Bool
elm_fileselector_is_save_get(Evas_Object *obj)

void
elm_fileselector_folder_only_set(Evas_Object *obj, Eina_Bool only)

#Eina_Bool
#elm_fileselector_folder_only_get(Evas_Object *obj)

void
elm_fileselector_buttons_ok_cancel_set(Evas_Object *obj, Eina_Bool buttons)

#Eina_Bool
#elm_fileselector_buttons_ok_cancel_get(Evas_Object *obj)

void
elm_fileselector_expandable_set(Evas_Object *obj, Eina_Bool expand)

void
elm_fileselector_path_set(Evas_Object *obj, const char *path)

const char *
elm_fileselector_path_get(Evas_Object *obj)

const char *
elm_fileselector_selected_get(Evas_Object *obj)


# progressbar
# ------------------------------------------------------------------------------

Evas_Object *
elm_progressbar_add(Evas_Object *parent)

void
elm_progressbar_label_set(Evas_Object *obj, const char *label)

void
elm_progressbar_icon_set(Evas_Object *obj, Evas_Object *icon)

void
elm_progressbar_span_size_set(Evas_Object *obj, Evas_Coord size)

void
elm_progressbar_horizontal_set(Evas_Object *obj, Eina_Bool horizontal)

void
elm_progressbar_inverted_set(Evas_Object *obj, Eina_Bool inverted)

void
elm_progressbar_pulse_set(Evas_Object *obj, Eina_Bool pulse)

void
elm_progressbar_pulse(Evas_Object *obj, Eina_Bool state)

void elm_progressbar_unit_format_set(Evas_Object *obj, const char *format)

void
elm_progressbar_value_set(Evas_Object *obj, double val)

double
elm_progressbar_value_get(obj)
    Evas_Object *obj
    CODE:
        RETVAL = elm_progressbar_value_get(obj);
    OUTPUT:
        RETVAL


# separator
# ------------------------------------------------------------------------------

Evas_Object *
elm_separator_add(Evas_Object *parent)

void
elm_separator_horizontal_set(Evas_Object *obj, Eina_Bool horizontal)

Eina_Bool
elm_separator_horizontal_get(Evas_Object *obj)


# spinner
# ------------------------------------------------------------------------------

Evas_Object *
elm_spinner_add(Evas_Object *parent)

void
elm_spinner_label_format_set(Evas_Object *obj, const char *fmt)

const char *
elm_spinner_label_format_get(Evas_Object *obj)

void
elm_spinner_min_max_set(Evas_Object *obj, double min, double max)

void
elm_spinner_step_set(Evas_Object *obj, double step)

void
elm_spinner_value_set(Evas_Object *obj, double val)

double
elm_spinner_value_get(const Evas_Object *obj)

void
elm_spinner_wrap_set(Evas_Object *obj, Eina_Bool wrap)


# index
# ------------------------------------------------------------------------------

Evas_Object *
elm_index_add(Evas_Object *parent)

void
elm_index_active_set(Evas_Object *obj, Eina_Bool active)

void
elm_index_item_level_set(Evas_Object *obj, int level)

int
elm_index_item_level_get(Evas_Object *obj)

const void *
elm_index_item_selected_get(Evas_Object *obj, int level)

void
elm_index_item_append(Evas_Object *obj, const char *letter, const void *item)

void
elm_index_item_prepend(Evas_Object *obj, const char *letter, const void *item)

void
elm_index_item_append_relative(Evas_Object *obj, const char *letter, const void *item, const void *relative)

void
elm_index_item_prepend_relative(Evas_Object *obj, const char *letter, const void *item, const void *relative)

void
elm_index_item_del(Evas_Object *obj, const void *item)

void
elm_index_item_clear(Evas_Object *obj)

void
elm_index_item_go(Evas_Object *obj, int level)


# photocam
# ------------------------------------------------------------------------------

Evas_Object *
elm_photocam_add(Evas_Object *parent)

int
elm_photocam_file_set(Evas_Object *obj, const char *file)

const char *
elm_photocam_file_get(Evas_Object *obj)

void
elm_photocam_zoom_set(Evas_Object *obj, double zoom)

double
elm_photocam_zoom_get(Evas_Object *obj)

void
elm_photocam_zoom_mode_set(Evas_Object *obj, Elm_Photocam_Zoom_Mode mode)

Elm_Photocam_Zoom_Mode
elm_photocam_zoom_mode_get(Evas_Object *obj)

void
elm_photocam_image_size_get(obj, w, h)
    Evas_Object *obj
    int w = NO_INIT
    int h = NO_INIT
    CODE:
        elm_photocam_image_size_get(obj, &w, &h);

void
elm_photocam_region_get(obj, x, y, w, h)
    Evas_Object *obj
    int x = NO_INIT
    int y = NO_INIT
    int w = NO_INIT
    int h = NO_INIT
    CODE:
        elm_photocam_region_get(obj, &x, &y, &w, &h);

void
elm_photocam_image_region_show(Evas_Object *obj, int x, int y, int w, int h)

void
elm_photocam_image_region_bring_in(Evas_Object *obj, int x, int y, int w, int h)

void
elm_photocam_paused_set(Evas_Object *obj, Eina_Bool paused)

Eina_Bool
elm_photocam_paused_get(Evas_Object *obj)


# map
# ------------------------------------------------------------------------------

Evas_Object *
elm_map_add(Evas_Object *parent)

void
elm_map_zoom_set(Evas_Object *obj, int zoom)

double
elm_map_zoom_get(Evas_Object *obj)

void
elm_map_zoom_mode_set(Evas_Object *obj, Elm_Map_Zoom_Mode mode)

Elm_Map_Zoom_Mode
elm_map_zoom_mode_get(Evas_Object *obj)

void
elm_map_geo_region_get(obj, lon, lat)
    Evas_Object *obj
    double lon = NO_INIT
    double lat = NO_INIT
    CODE:
        elm_map_geo_region_get(obj, &lon, &lat);

void
elm_map_geo_region_bring_in(Evas_Object *obj, double lon, double lat)

void
elm_map_geo_region_show(Evas_Object *obj, double lon, double lat)

void
elm_map_paused_set(Evas_Object *obj, Eina_Bool paused)

Eina_Bool
elm_map_paused_get(Evas_Object *obj)

void
elm_map_paused_markers_set(Evas_Object *obj, Eina_Bool paused)

Eina_Bool
elm_map_paused_markers_get(Evas_Object *obj)

void
elm_map_utils_convert_coord_into_geo(x, y, size, lon, lat)
    int x
    int y
    int size
    double lon = NO_INIT
    double lat = NO_INIT
    CODE:
        elm_map_utils_convert_coord_into_geo(x, y, size, &lon, &lat);

void
elm_map_utils_convert_geo_into_coord(lon, lat, size, x, y)
    double lon
    double lat
    int size
    int x = NO_INIT
    int y = NO_INIT
    CODE:
        elm_map_utils_convert_geo_into_coord(lon, lat, size, &x, &y);

Elm_Map_Marker *
elm_map_marker_add(Evas_Object *obj, double lon, double lat, Elm_Map_Marker_Class *clas, Elm_Map_Group_Class *group, void *data);

void
elm_map_max_marker_per_group_set(Evas_Object *obj, int max)

void
elm_map_marker_remove(Elm_Map_Marker *marker)

void
elm_map_marker_bring_in(Elm_Map_Marker *marker)

void
elm_map_marker_show(Elm_Map_Marker *marker)

void
elm_map_markers_list_show(Eina_List *markers)

Evas_Object *
elm_map_marker_object_get(Elm_Map_Marker *marker)

void
elm_map_marker_update(Elm_Map_Marker *marker)

void
elm_map_bubbles_close(Evas_Object *obj)


# panel
# ------------------------------------------------------------------------------

Evas_Object *
elm_panel_add(Evas_Object *parent)

void
elm_panel_orient_set(Evas_Object *obj, Elm_Panel_Orient orient)

void
elm_panel_content_set(Evas_Object *obj, Evas_Object *content)


# flip
# ------------------------------------------------------------------------------

Evas_Object *
elm_flip_add(Evas_Object *parent)

void
elm_flip_content_front_set(Evas_Object *obj, Evas_Object *content)

void
elm_flip_content_back_set(Evas_Object *obj, Evas_Object *content)

Eina_Bool
elm_flip_front_get(Evas_Object *obj)

void
elm_flip_perspective_set(Evas_Object *obj, Evas_Coord foc, Evas_Coord x, Evas_Coord y)

void
elm_flip_go(Evas_Object *obj, Elm_Flip_Mode mode)


# scrolled
# ------------------------------------------------------------------------------

Evas_Object *
elm_scrolled_entry_add(Evas_Object *parent)

void
elm_scrolled_entry_single_line_set(Evas_Object *obj, Eina_Bool single_line)

void
elm_scrolled_entry_password_set(Evas_Object *obj, Eina_Bool password)

void
elm_scrolled_entry_entry_set(Evas_Object *obj, const char *entry)

const char *
elm_scrolled_entry_entry_get(const Evas_Object *obj)

const char *
elm_scrolled_entry_selection_get(const Evas_Object *obj)

void
elm_scrolled_entry_entry_insert(Evas_Object *obj, const char *entry)

void
elm_scrolled_entry_line_wrap_set(Evas_Object *obj, Eina_Bool wrap);

void
elm_scrolled_entry_line_char_wrap_set(Evas_Object *obj, Eina_Bool wrap)

void
elm_scrolled_entry_editable_set(Evas_Object *obj, Eina_Bool editable)

void
elm_scrolled_entry_select_none(Evas_Object *obj)

void
elm_scrolled_entry_select_all(Evas_Object *obj)

Eina_Bool
elm_scrolled_entry_cursor_next(Evas_Object *obj)

Eina_Bool
elm_scrolled_entry_cursor_prev(Evas_Object *obj)

Eina_Bool
elm_scrolled_entry_cursor_up(Evas_Object *obj)

Eina_Bool
elm_scrolled_entry_cursor_down(Evas_Object *obj)

void
elm_scrolled_entry_cursor_begin_set(Evas_Object *obj)

void
elm_scrolled_entry_cursor_end_set(Evas_Object *obj)

void
elm_scrolled_entry_cursor_line_begin_set(Evas_Object *obj)

void
elm_scrolled_entry_cursor_line_end_set(Evas_Object *obj)

void
elm_scrolled_entry_cursor_selection_begin(Evas_Object *obj)

void
elm_scrolled_entry_cursor_selection_end(Evas_Object *obj)

Eina_Bool
elm_scrolled_entry_cursor_is_format_get(Evas_Object *obj)

Eina_Bool
elm_scrolled_entry_cursor_is_visible_format_get(Evas_Object *obj)

const char *
elm_scrolled_entry_cursor_content_get(Evas_Object *obj)

void
elm_scrolled_entry_selection_cut(Evas_Object *obj)

void
elm_scrolled_entry_selection_copy(Evas_Object *obj)

void
elm_scrolled_entry_selection_paste(Evas_Object *obj)

void
elm_scrolled_entry_context_menu_clear(Evas_Object *obj)

# TODO
#void
#elm_scrolled_entry_context_menu_item_add(Evas_Object *obj, const char *label, const char *icon_file, Elm_Icon_Type icon_type, void (*func) (void *data, Evas_Object *obj, void *event_info), const void *data);

void
elm_scrolled_entry_context_menu_disabled_set(Evas_Object *obj, Eina_Bool disabled)

Eina_Bool
elm_scrolled_entry_context_menu_disabled_get(Evas_Object *obj)

void
elm_scrolled_entry_scrollbar_policy_set(Evas_Object *obj, Elm_Scroller_Policy h, Elm_Scroller_Policy v)
