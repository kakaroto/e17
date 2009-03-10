# Copyright (c) 2008 Simon Busch
#
# This file is part of python-elementary.
#
# python-elementary is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# python-elementary is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with python-elementary.  If not, see <http://www.gnu.org/licenses/>.
#

cimport evas.c_evas
import evas.c_evas

cdef extern from "Python.h":
    ctypedef unsigned int size_t

    void *PyMem_Malloc(size_t nbytes)
    void PyMem_Free(void *p)
    void memcpy(void *source, void *dest, size_t nbytes)

cdef enum Elm_Win_Type:
    ELM_WIN_BASIC
    ELM_WIN_DIALOG_BASIC

cdef enum Elm_Win_Keyboard_Mode:
    ELM_WIN_KEYBOARD_OFF
    ELM_WIN_KEYBOARD_ON
    ELM_WIN_KEYBOARD_ALPHA
    ELM_WIN_KEYBOARD_NUMERIC
    ELM_WIN_KEYBOARD_PIN
    ELM_WIN_KEYBOARD_PHONE_NUMBER
    ELM_WIN_KEYBOARD_HEX
    ELM_WIN_KEYBOARD_TERMINAL
    ELM_WIN_KEYBOARD_PASSWORD

cdef enum Elm_Hover_Axis:
    ELM_HOVER_AXIS_NONE
    ELM_HOVER_AXIS_HORIZONTAL
    ELM_HOVER_AXIS_VERTICAL
    ELM_HOVER_AXIS_BOTH

cdef enum Elm_Text_Format:
    ELM_TEXT_FORMAT_PLAIN_UTF8
    ELM_TEXT_FORMAT_MARKUP_UTF8

cdef enum Elm_Icon_Type:
    ELM_ICON_NONE
    ELM_ICON_FILE
    ELM_ICON_STANDARD

cdef enum Elementary_List_Mode:
    ELM_LIST_COMPRESS
    ELM_LIST_SCROLL
    ELM_LIST_LIMIT
    
cdef extern from "Ecore_X.h":
    ctypedef unsigned int Ecore_X_ID
    ctypedef Ecore_X_ID Ecore_X_Window

# For Debugging
"""
cdef extern from "Ecore_Evas.h":
    ctypedef struct Ecore_Evas
    void ecore_evas_geometry_get(Ecore_Evas *ee, int *x, int *y, int *w, int *h)

cdef extern from "Eina.h":
    ctypedef struct Eina_List

cdef extern from "Ecore_Job.h":
    ctypedef struct Ecore_Job

ctypedef struct Elm_Win:
    Ecore_Evas *ee
    evas.c_evas.Evas *evas
    evas.c_evas.Evas_Object *parent
    evas.c_evas.Evas_Object *win_obj
    Eina_List *subobjs
    Ecore_X_ID xwin
    Ecore_Job *deferred_resize_job
    Ecore_Job *deferred_child_eval_job
    Elm_Win_Type type
    Elm_Win_Keyboard_Mode kbdmode
    evas.c_evas.Evas_Bool autodel = 1
"""
cdef extern from "Elementary.h":
    cdef struct Elm_Entry_Anchor_Info
    cdef struct Elm_Entry_Anchorview_Info
    cdef struct Elm_Entry_Anchorblock_Info
    cdef struct Elm_Hoversel_Item
    cdef struct Elm_Toolbar_Item
    cdef struct Elm_List_Item
    cdef struct Elm_Carousel_Item

    # Basic elementary functions
    void elm_init(int argc,char** argv)
    void elm_shutdown()
    void elm_run()
    void elm_exit()
    
    # Quicklaunch
    void         elm_quicklaunch_init(int argc, char **argv)
    void         elm_quicklaunch_sub_init(int argc, char **argv)
    void         elm_quicklaunch_sub_shutdown(void)
    void         elm_quicklaunch_shutdown(void)
    void         elm_quicklaunch_seed(void)
    c_evas.Evas_Bool    elm_quicklaunch_prepare(int argc, char **argv)
    c_evas.Evas_Bool    elm_quicklaunch_fork(int argc, char **argv, char *cwd, void (postfork_func) (void *data), void *postfork_data)
    void         elm_quicklaunch_cleanup(void)
    int          elm_quicklaunch_fallback(int argc, char **argv)
    char        *elm_quicklaunch_exe_path_get(char *exe)
    
    # Object handling
    void         elm_object_scale_set(c_evas.Evas_Object *obj, double scale)
    double       elm_object_scale_get(c_evas.Evas_Object *obj)
    void         elm_object_focus(c_evas.Evas_Object *obj)
    
    void         elm_coords_finger_size_adjust(int times_w, c_evas.Evas_Coord *w, int times_h, c_evas.Evas_Coord *h)
    
    void         elm_theme_overlay_add(char *item)
    void         elm_theme_extension_add(char *item)
   
    # Window handling
    evas.c_evas.Evas_Object *elm_win_add(evas.c_evas.Evas_Object* parent,char* name,Elm_Win_Type type)
    void elm_win_resize_object_add(evas.c_evas.Evas_Object* obj,evas.c_evas.Evas_Object* subobj)
    void elm_win_resize_object_del(evas.c_evas.Evas_Object* obj,evas.c_evas.Evas_Object* subobj)
    void elm_win_title_set(evas.c_evas.Evas_Object* obj,char *title)
    void elm_win_autodel_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Bool autodel)
    void elm_win_activate(evas.c_evas.Evas_Object *obj)
    void elm_win_borderless_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Bool borderless)
    void elm_win_shaped_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Bool shaped)
    void elm_win_alpha_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Bool alpha)
    void elm_win_override_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Bool override)
    void elm_win_keyboard_mode_set(evas.c_evas.Evas_Object *obj, Elm_Win_Keyboard_Mode mode)
    void elm_win_keyboard_win_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Bool is_keyboard)
    
    # X specific call - wont't work on non-x engines (return 0)
    Ecore_X_Window elm_win_xwindow_get(evas.c_evas.Evas_Object *obj)
    
    evas.c_evas.Evas_Object *elm_win_inwin_add(evas.c_evas.Evas_Object *obj)
    void elm_win_inwin_activate(evas.c_evas.Evas_Object *obj)
    void elm_win_inwin_style_set(evas.c_evas.Evas_Object *obj, char *style)
    void         elm_win_inwin_content_set(c_evas.Evas_Object *obj, c_evas.Evas_Object *content)
    
    # Background object
    evas.c_evas.Evas_Object *elm_bg_add(evas.c_evas.Evas_Object *parent)
    void elm_bg_file_set(evas.c_evas.Evas_Object *obj, char *file, char *group)
    
    # Icon object
    evas.c_evas.Evas_Object *elm_icon_add(evas.c_evas.Evas_Object *parent)
    void elm_icon_file_set(evas.c_evas.Evas_Object *obj, char *file, char *group)
    void elm_icon_standard_set(evas.c_evas.Evas_Object *obj, char* name)
    void elm_icon_smooth_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Bool smooth)
    void elm_icon_no_scale_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Bool no_scale)
    void elm_icon_scale_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Bool scale_up, evas.c_evas.Evas_Bool scale_down)
    void elm_icon_fill_outside_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Bool fill_outside)
    
    # Box object
    evas.c_evas.Evas_Object *elm_box_add(evas.c_evas.Evas_Object *parent)
    void elm_box_horizontal_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Bool horizontal)
    void elm_box_homogenous_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Bool homogenous)
    void elm_box_pack_start(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *subobj)
    void elm_box_pack_end(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *subobj)
    void elm_box_pack_before(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *subobj, evas.c_evas.Evas_Object *before)
    void elm_box_pack_after(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *subobj, evas.c_evas.Evas_Object *after)
    
    # Button object
    evas.c_evas.Evas_Object *elm_button_add(evas.c_evas.Evas_Object *parent)
    void elm_button_label_set(evas.c_evas.Evas_Object *obj, char *label)
    void elm_button_icon_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *icon)
    void elm_button_style_set(evas.c_evas.Evas_Object *obj, char *style)
    
    # Scroller object
    evas.c_evas.Evas_Object *elm_scroller_add(evas.c_evas.Evas_Object *parent)
    void elm_scroller_content_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *child)
    void elm_scroller_content_min_limit(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Bool w, evas.c_evas.Evas_Bool h)
    void         elm_scroller_region_show(c_evas.Evas_Object *obj, c_evas.Evas_Coord x, c_evas.Evas_Coord y, c_evas.Evas_Coord w, c_evas.Evas_Coord h)
    # Label object  
    evas.c_evas.Evas_Object *elm_label_add(evas.c_evas.Evas_Object *parent)
    void elm_label_label_set(evas.c_evas.Evas_Object *obj, char *label)
    
    # Toggle object
    evas.c_evas.Evas_Object *elm_toggle_add(evas.c_evas.Evas_Object *parent)
    void elm_toggle_label_set(evas.c_evas.Evas_Object *obj, char *label)
    void elm_toggle_icon_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *icon)
    void elm_toggle_states_labels_set(evas.c_evas.Evas_Object *obj, char *onlabel, char *offlabel)
    void elm_toggle_state_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Bool state)
    c_Evas.Evas_Bool    elm_toggle_state_get(c_evas.Evas_Object *obj)
    void elm_toggle_state_pointer_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Bool *statep)
    
    # Frame object
    evas.c_evas.Evas_Object* elm_frame_add(evas.c_evas.Evas_Object *parent)
    void elm_frame_label_set(evas.c_evas.Evas_Object *obj, char *label)
    void elm_frame_content_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *content)
    void elm_frame_style_set(evas.c_evas.Evas_Object *obj, char *style)
   
    # Table object
    evas.c_evas.Evas_Object *elm_table_add(evas.c_evas.Evas_Object *parent)
    void elm_table_homogenous_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Bool homogenous)
    void elm_table_pack(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *subobj, int x, int y, int w, int h)
    
    # Clock object
    evas.c_evas.Evas_Object *elm_clock_add(evas.c_evas.Evas_Object *parent)
    void elm_clock_time_set(evas.c_evas.Evas_Object *obj, int hrs, int min, int sec)
    void elm_clock_time_get(evas.c_evas.Evas_Object *obj, int *hrs, int *min, int *sec)
    void elm_clock_edit_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Bool edit)
    void elm_clock_show_am_pm_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Bool am_pm)
    void elm_clock_show_seconds_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Bool seconds)
    
    # Layout object
    evas.c_evas.Evas_Object *elm_layout_add(evas.c_evas.Evas_Object *parent)
    void elm_layout_file_set(evas.c_evas.Evas_Object *obj, char *file, char *group)
    void elm_layout_content_set(evas.c_evas.Evas_Object *obj, char *swallow, evas.c_evas.Evas_Object *content)
    evas.c_evas.Evas_Object *elm_layout_edje_get(evas.c_evas.Evas_Object *obj)
    
    # Hover object
    evas.c_evas.Evas_Object *elm_hover_add(evas.c_evas.Evas_Object *parent)
    void elm_hover_target_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *target)
    void elm_hover_parent_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *parent)
    void elm_hover_content_set(evas.c_evas.Evas_Object *obj, char *swallow, evas.c_evas.Evas_Object *content)
    void elm_hover_style_set(evas.c_evas.Evas_Object *obj, char *style)

    # Entry Object 
    evas.c_evas.Evas_Object *elm_entry_add(evas.c_evas.Evas_Object *parent)
    void elm_entry_single_line_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Bool single_line)
    void elm_entry_password_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Bool password)
    void elm_entry_entry_set(evas.c_evas.Evas_Object *obj, char *entry)
    char *elm_entry_entry_get(evas.c_evas.Evas_Object *obj)
    char *elm_entry_selection_get(evas.c_evas.Evas_Object *obj)
    void elm_entry_entry_insert(evas.c_evas.Evas_Object *obj, char *entry)
    void elm_entry_line_wrap_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Bool wrap)
    void elm_entry_editable_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Bool editable)
    void elm_entry_select_none(evas.c_evas.Evas_Object *obj)
    void elm_entry_select_all(evas.c_evas.Evas_Object *obj)
    char        *elm_entry_markup_to_utf8(char *s)
    char        *elm_entry_utf8_to_markup(char *s)
    # Composite objects

    # Notepad object
    evas.c_evas.Evas_Object *elm_notepad_add(evas.c_evas.Evas_Object *parent)
    void elm_notepad_file_set(evas.c_evas.Evas_Object *obj, char *file, Elm_Text_Format format)

    # Anchorview object
    evas.c_evas.Evas_Object *elm_anchorview_add(evas.c_evas.Evas_Object *parent)
    void elm_anchorview_text_set(evas.c_evas.Evas_Object *obj, char *text)
    void elm_anchorview_hover_parent_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *parent)
    void elm_anchorview_hover_style_set(evas.c_evas.Evas_Object *obj, char *style)
    void elm_anchorview_hover_end(evas.c_evas.Evas_Object *obj)
    
    # Anchorblock object
    evas.c_evas.Evas_Object *elm_anchorblock_add(evas.c_evas.Evas_Object *parent)
    void         elm_anchorblock_text_set(evas.c_evas.Evas_Object *obj, char *text)
    void         elm_anchorblock_hover_parent_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *parent)
    void         elm_anchorblock_hover_style_set(evas.c_evas.Evas_Object *obj, char *style)
    void         elm_anchorblock_hover_end(evas.c_evas.Evas_Object *obj)

    # Bubble object
    evas.c_evas.Evas_Object *elm_bubble_add(evas.c_evas.Evas_Object *parent)
    void elm_bubble_label_set(evas.c_evas.Evas_Object *obj, char *label)
    void elm_bubble_info_set(evas.c_evas.Evas_Object *obj, char *info)
    void elm_bubble_content_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *content)
    void elm_bubble_icon_set(evas.c_evas.Evas_Object *pbj, evas.c_evas.Evas_Object *icon)
    void elm_bubble_corner_set(evas.c_evas.Evas_Object *obj, char *corner) 

    # Photo object
    evas.c_evas.Evas_Object *elm_photo_add(evas.c_evas.Evas_Object *parent) 
    void elm_photo_file_set(evas.c_evas.Evas_Object *obj, char *file)
    void elm_photo_size_set(evas.c_evas.Evas_Object *obj, int size)

    # Hoversel object
    evas.c_evas.Evas_Object *elm_hoversel_add(evas.c_evas.Evas_Object *parent)
    void elm_hoversel_hover_parent_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object
            *parent)
    void elm_hoversel_label_set(evas.c_evas.Evas_Object *obj, char *label)
    void elm_hoversel_icon_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *icon)
    void elm_hoversel_hover_end(evas.c_evas.Evas_Object *obj)
    Elm_Hoversel_Item *elm_hoversel_item_add(evas.c_evas.Evas_Object *obj, char *label, char
            *icon_file, Elm_Icon_Type icon_type, void (*func)(void *data, evas.c_evas.Evas_Object *obj, void *event_info), void *data)
    void elm_hoversel_item_del(Elm_Hoversel_Item *item)

    # Toolbar object
    evas.c_evas.Evas_Object *elm_toolbar_add(evas.c_evas.Evas_Object *parent)
    Elm_Toolbar_Item *elm_toolbar_item_add(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *icon, char *label, 
                    void (*func) (void *data, evas.c_evas.Evas_Object *obj, void *event_info), void *data)
    void         elm_toolbar_item_del(Elm_Toolbar_Item *item)
    void         elm_toolbar_item_select(Elm_Toolbar_Item *item)
    void         elm_toolbar_scrollable_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Bool scrollable)

    # List object
    evas.c_evas.Evas_Object *elm_list_add(evas.c_evas.Evas_Object *parent)
    Elm_List_Item *elm_list_item_append(evas.c_evas.Evas_Object *obj, char *label, evas.c_evas.Evas_Object *icon, evas.c_evas.Evas_Object *end, void (*func) (void *data, evas.c_evas.Evas_Object *obj, void *event_info), void *data)
    Elm_List_Item *elm_list_item_prepend(evas.c_evas.Evas_Object *obj, char *label, evas.c_evas.Evas_Object *icon, evas.c_evas.Evas_Object *end, void (*func) (void *data, evas.c_evas.Evas_Object *obj, void *event_info), void *data)
    Elm_List_Item *elm_list_item_insert_before(evas.c_evas.Evas_Object *obj, Elm_List_Item *before, char *label, evas.c_evas.Evas_Object *icon, evas.c_evas.Evas_Object *end, void (*func) (void *data, evas.c_evas.Evas_Object *obj, void *event_info), void *data)
    Elm_List_Item *elm_list_item_insert_after(evas.c_evas.Evas_Object *obj, Elm_List_Item *after, char *label, evas.c_evas.Evas_Object *icon, evas.c_evas.Evas_Object *end, void (*func) (void *data, evas.c_evas.Evas_Object *obj, void *event_info), void *data)
    void         elm_list_go(evas.c_evas.Evas_Object *obj) 
    void      elm_list_multi_select_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Bool multi) 
    void         elm_list_horizontal_mode_set(evas.c_evas.Evas_Object *obj, Elementary_List_Mode mode) 
    evas.c_evas.Eina_List     *elm_list_items_get(evas.c_evas.Evas_Object *obj) 
    Elm_List_Item *elm_list_selected_item_get(evas.c_evas.Evas_Object *obj) 
    evas.c_evas.Eina_List     *elm_list_selected_items_get(evas.c_evas.Evas_Object *obj) 
    void         elm_list_item_selected_set(Elm_List_Item *item, evas.c_evas.Evas_Bool selected) 
    void         elm_list_item_show(Elm_List_Item *item) 
    void         elm_list_item_del(Elm_List_Item *item) 
    void  *elm_list_item_data_get(Elm_List_Item *item) 
    evas.c_evas.Evas_Object *elm_list_item_icon_get(Elm_List_Item *item) 
    evas.c_evas.Evas_Object *elm_list_item_end_get(Elm_List_Item *item) 

    # Carousel object
    evas.c_evas.Evas_Object *elm_carousel_add(evas.c_evas.Evas_Object *parent)
    Elm_Carousel_Item *elm_carousel_item_add(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *icon, char *label, void (*func) (void *data, evas.c_evas.Evas_Object *obj, void *event_info), void *data)
    void         elm_carousel_item_del(Elm_Carousel_Item *item)
    void         elm_carousel_item_select(Elm_Carousel_Item *item)

    # Slider object
    evas.c_evas.Evas_Object *elm_slider_add(evas.c_evas.Evas_Object *parent)
    void elm_slider_label_set(evas.c_evas.Evas_Object *obj, char *label)
    void elm_slider_icon_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *icon)
    void elm_slider_span_size_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord size)
    void elm_slider_unit_format_set(evas.c_evas.Evas_Object *obj, char *format)
    void elm_slider_indicator_format_set(evas.c_evas.Evas_Object *obj, char *indicator)
    void elm_slider_horizontal_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Bool horizontal)
    void elm_slider_min_max_set(evas.c_evas.Evas_Object *obj, double min, double max)
    void elm_slider_value_set(evas.c_evas.Evas_Object *obj, double val)
    double elm_slider_value_get(evas.c_evas.Evas_Object *obj)
    void elm_slider_inverted_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Bool inverted)

    # Geneirc List
    Evas_Object      *elm_genlist_add(c_evas.Evas_Object *parent);
    Elm_Genlist_Item *elm_genlist_item_append(c_evas.Evas_Object *obj,  Elm_Genlist_Item_Class *itc,  void *data, Elm_Genlist_Item *parent, Elm_Genlist_Item_Flags flags, void (*func) (void *data, c_evas.Evas_Object *obj, void *event_info),  void *func_data);
    Elm_Genlist_Item *elm_genlist_item_prepend(c_evas.Evas_Object *obj,  Elm_Genlist_Item_Class *itc,  void *data, Elm_Genlist_Item *parent, Elm_Genlist_Item_Flags flags, void (*func) (void *data, c_evas.Evas_Object *obj, void *event_info),  void *func_data);
    Elm_Genlist_Item *elm_genlist_item_insert_before(c_evas.Evas_Object *obj,  Elm_Genlist_Item_Class *itc,  void *data, Elm_Genlist_Item *before, Elm_Genlist_Item_Flags flags, void (*func) (void *data, c_evas.Evas_Object *obj, void *event_info),  void *func_data);
    Elm_Genlist_Item *elm_genlist_item_insert_after(c_evas.Evas_Object *obj,  Elm_Genlist_Item_Class *itc,  void *data, Elm_Genlist_Item *after, Elm_Genlist_Item_Flags flags, void (*func) (void *data, c_evas.Evas_Object *obj, void *event_info),  void *func_data);
    void              elm_genlist_clear(c_evas.Evas_Object *obj);
    Elm_Genlist_Item *elm_genlist_selected_item_get( c_evas.Evas_Object *obj);
     Eina_List  *elm_genlist_selected_items_get( c_evas.Evas_Object *obj);
    Elm_Genlist_Item *elm_genlist_first_item_get( Evas_Object *obj);
    Elm_Genlist_Item *elm_genlist_last_item_get( c_evas.Evas_Object *obj);
    Elm_Genlist_Item *elm_genlist_item_next_get( Elm_Genlist_Item *item);
    Elm_Genlist_Item *elm_genlist_item_prev_get( Elm_Genlist_Item *item);
    void              elm_genlist_item_selected_set(Elm_Genlist_Item *item, Evas_Bool selected);
    Evas_Bool         elm_genlist_item_selected_get( Elm_Genlist_Item *item);
    void              elm_genlist_item_disabled_set(Elm_Genlist_Item *item, Evas_Bool disabled);
    Evas_Bool         elm_genlist_item_disabled_get( Elm_Genlist_Item *item);
    void              elm_genlist_item_show(Elm_Genlist_Item *item);
    void              elm_genlist_item_del(Elm_Genlist_Item *item);
    void       *elm_genlist_item_data_get( Elm_Genlist_Item *item);
    void              elm_genlist_item_update(Elm_Genlist_Item *item);
    
    # Check widget
    c_evas.Evas_Object *elm_check_add(c_evas.Evas_Object *parent);
    void         elm_check_label_set(c_evas.Evas_Object *obj, char *label);
    void         elm_check_icon_set(c_evas.Evas_Object *obj, c_evas.Evas_Object *icon);
    void         elm_check_state_set(c_evas.Evas_Object *obj, c_evas.Evas_Bool state);
    c_evas.Evas_Bool    elm_check_state_get(c_evas.Evas_Object *obj);
    void         elm_check_state_pointer_set(c_evas.Evas_Object *obj, c_evas.Evas_Bool *statep);
    
    # Radio widget
    c_evas.Evas_Object *elm_radio_add(c_evas.Evas_Object *parent);
    void         elm_radio_label_set(c_evas.Evas_Object *obj, char *label);
    void         elm_radio_icon_set(Evas_Object *obj, c_evas.Evas_Object *icon);
    void         elm_radio_group_add(c_evas.Evas_Object *obj, c_evas.Evas_Object *group);
    void         elm_radio_state_value_set(c_evas.Evas_Object *obj, int value);
    void         elm_radio_value_set(c_evas.Evas_Object *obj, int value);
    int          elm_radio_value_get(c_evas.Evas_Object *obj);
    void         elm_radio_value_pointer_set(c_evas.Evas_Object *obj, int *valuep);
    
    # Pager widget
    c_evas.Evas_Object *elm_pager_add(c_evas.Evas_Object *parent);
    void         elm_pager_content_push(c_evas.Evas_Object *obj, c_evas.Evas_Object *content);
    void         elm_pager_content_pop(c_evas.Evas_Object *obj);
    void         elm_pager_content_promote(c_evas.Evas_Object *obj, c_evas.Evas_Object *content);
    c_evas.Evas_Object *elm_pager_content_bottom_get(c_evas.Evas_Object *obj);
    c_evas.Evas_Object *elm_pager_content_top_get(c_evas.Evas_Object *obj);
   
# Forward declaration of some classes
cdef class Object(evas.c_evas.Object)
cdef class Hoversel(Object)
cdef class Toolbar(Object)
cdef class List(Object)
