# Copyright (C) 2007-2008 Gustavo Sverzut Barbieri, Ulisses Furquim
#
# This file is part of Python-Edje.
#
# Python-Edje is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Edje is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Edje.  If not, see <http://www.gnu.org/licenses/>.

cimport evas.c_evas
import evas.c_evas
cimport edje.c_edje
import edje.c_edje


cdef extern from "Edje_Edit.h":
    ####################################################################
    # Enumerations
    #
    ctypedef enum Edje_Edit_Image_Comp:
        EDJE_EDIT_IMAGE_COMP_RAW = 0
        EDJE_EDIT_IMAGE_COMP_USER = 1
        EDJE_EDIT_IMAGE_COMP_COMP = 2
        EDJE_EDIT_IMAGE_COMP_LOSSY =3

    ####################################################################
    # Structures
    #


    ####################################################################
    # Engine
    #
    void edje_edit_string_list_free( evas.c_evas.Eina_List *lst)
    void edje_edit_string_free( char *str)
    char* edje_edit_compiler_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_Bool edje_edit_save(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_Bool edje_edit_save_all(evas.c_evas.Evas_Object *obj)
    void edje_edit_print_internal_status(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_Bool edje_edit_group_add(evas.c_evas.Evas_Object *obj, char  *name)
    evas.c_evas.Eina_Bool edje_edit_group_del(evas.c_evas.Evas_Object *obj, char *group_name)
    evas.c_evas.Eina_Bool edje_edit_group_exist(evas.c_evas.Evas_Object *obj, char *group)
    evas.c_evas.Eina_Bool edje_edit_group_name_set(evas.c_evas.Evas_Object *obj, char  *new_name)
    int edje_edit_group_min_w_get(evas.c_evas.Evas_Object *obj)
    void edje_edit_group_min_w_set(evas.c_evas.Evas_Object *obj, int w)
    int edje_edit_group_min_h_get(evas.c_evas.Evas_Object *obj)
    void edje_edit_group_min_h_set(evas.c_evas.Evas_Object *obj, int h)
    int edje_edit_group_max_w_get(evas.c_evas.Evas_Object *obj)
    void edje_edit_group_max_w_set(evas.c_evas.Evas_Object *obj, int w)
    int edje_edit_group_max_h_get(evas.c_evas.Evas_Object *obj)
    void edje_edit_group_max_h_set(evas.c_evas.Evas_Object *obj, int h)

    evas.c_evas.Evas_Object * edje_edit_object_add(evas.c_evas.Evas *evas)

    # DATA API
    evas.c_evas.Eina_List * edje_edit_data_list_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_Bool edje_edit_data_add(evas.c_evas.Evas_Object *obj, char *itemname, char *value)
    evas.c_evas.Eina_Bool edje_edit_data_del(evas.c_evas.Evas_Object *obj, char *itemname)
    char * edje_edit_data_value_get(evas.c_evas.Evas_Object * obj, char *itemname)
    evas.c_evas.Eina_Bool edje_edit_data_value_set(evas.c_evas.Evas_Object * obj, char *itemname, char *value)
    evas.c_evas.Eina_Bool edje_edit_data_name_set(evas.c_evas.Evas_Object *obj, char *itemname, char *newname)

    evas.c_evas.Eina_List * edje_edit_group_data_list_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_Bool edje_edit_group_data_add(evas.c_evas.Evas_Object *obj, char *itemname, char *value)
    evas.c_evas.Eina_Bool edje_edit_group_data_del(evas.c_evas.Evas_Object *obj, char *itemname)
    char * edje_edit_group_data_value_get(evas.c_evas.Evas_Object * obj, char *itemname)
    evas.c_evas.Eina_Bool edje_edit_group_data_value_set(evas.c_evas.Evas_Object * obj, char *itemname, char *value)
    evas.c_evas.Eina_Bool edje_edit_group_data_name_set(evas.c_evas.Evas_Object *obj, char *itemname, char *newname)

    # COLOR CLASSES API
    evas.c_evas.Eina_List * edje_edit_color_classes_list_get(evas.c_evas.Evas_Object * obj)
    evas.c_evas.Eina_Bool edje_edit_color_class_add(evas.c_evas.Evas_Object *obj, char *name)
    evas.c_evas.Eina_Bool edje_edit_color_class_del(evas.c_evas.Evas_Object *obj, char *name)
    evas.c_evas.Eina_Bool edje_edit_color_class_colors_get(evas.c_evas.Evas_Object *obj, char *class_name, int *r, int *g, int *b, int *a, int *r2, int *g2, int *b2, int *a2, int *r3, int *g3, int *b3, int *a3)
    evas.c_evas.Eina_Bool edje_edit_color_class_colors_set(evas.c_evas.Evas_Object *obj, char *class_name, int r, int g, int b, int a, int r2, int g2, int b2, int a2, int r3, int g3, int b3, int a3)
    evas.c_evas.Eina_Bool edje_edit_color_class_name_set(evas.c_evas.Evas_Object *obj, char *name, char *newname)

    # TEXT STYLES
    evas.c_evas.Eina_List * edje_edit_styles_list_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_Bool edje_edit_style_add(evas.c_evas.Evas_Object *obj, char *style)
    void edje_edit_style_del(evas.c_evas.Evas_Object *obj, char *style)
    evas.c_evas.Eina_List * edje_edit_style_tags_list_get(evas.c_evas.Evas_Object *obj, char *style)
    char* edje_edit_style_tag_value_get(evas.c_evas.Evas_Object *obj, char *style, char *tag)
    void edje_edit_style_tag_value_set(evas.c_evas.Evas_Object *obj, char *style, char *tag, char *new_value)
    void edje_edit_style_tag_name_set(evas.c_evas.Evas_Object *obj, char *style, char *tag, char *new_name)
    evas.c_evas.Eina_Bool edje_edit_style_tag_add(evas.c_evas.Evas_Object *obj, char *style, char* tag_name)
    void edje_edit_style_tag_del(evas.c_evas.Evas_Object *obj, char *style, char* tag)

    # EXTERNALS API
    evas.c_evas.Eina_List *edje_edit_externals_list_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_Bool edje_edit_external_add(evas.c_evas.Evas_Object *obj, char *name)
    evas.c_evas.Eina_Bool edje_edit_external_del(evas.c_evas.Evas_Object *obj, char *name)

    # PARTS API
    evas.c_evas.Eina_List * edje_edit_parts_list_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_Bool edje_edit_part_add(evas.c_evas.Evas_Object *obj, char *name, edje.c_edje.Edje_Part_Type type)
    evas.c_evas.Eina_Bool edje_edit_part_external_add(evas.c_evas.Evas_Object *obj, char *name, char *source)
    evas.c_evas.Eina_Bool edje_edit_part_del(evas.c_evas.Evas_Object *obj, char *part)
    evas.c_evas.Eina_Bool edje_edit_part_exist(evas.c_evas.Evas_Object *obj, char *part)
    char * edje_edit_part_above_get(evas.c_evas.Evas_Object *obj, char *part)
    char * edje_edit_part_below_get(evas.c_evas.Evas_Object *obj, char *part)
    evas.c_evas.Eina_Bool edje_edit_part_restack_below(evas.c_evas.Evas_Object *obj, char *part)
    evas.c_evas.Eina_Bool edje_edit_part_restack_above(evas.c_evas.Evas_Object *obj, char *part)
    evas.c_evas.Eina_Bool edje_edit_part_name_set(evas.c_evas.Evas_Object *obj, char  *part, char  *new_name)
    edje.c_edje.Edje_Part_Type edje_edit_part_type_get(evas.c_evas.Evas_Object *obj, char *part)
    char * edje_edit_part_clip_to_get(evas.c_evas.Evas_Object *obj, char *part)
    evas.c_evas.Eina_Bool edje_edit_part_clip_to_set(evas.c_evas.Evas_Object *obj, char *part, char *clip_to)
    char * edje_edit_part_source_get(evas.c_evas.Evas_Object *obj, char *part)
    evas.c_evas.Eina_Bool edje_edit_part_source_set(evas.c_evas.Evas_Object *obj, char *part, char *source)
    edje.c_edje.Edje_Text_Effect edje_edit_part_effect_get(evas.c_evas.Evas_Object *obj, char *part)
    void edje_edit_part_effect_set(evas.c_evas.Evas_Object *obj, char *part, edje.c_edje.Edje_Text_Effect effect)
    char * edje_edit_part_selected_state_get(evas.c_evas.Evas_Object *obj, char *part, double *value)
    evas.c_evas.Eina_Bool edje_edit_part_selected_state_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    evas.c_evas.Eina_Bool edje_edit_part_mouse_events_get(evas.c_evas.Evas_Object *obj, char *part)
    void edje_edit_part_mouse_events_set(evas.c_evas.Evas_Object *obj, char *part, evas.c_evas.Eina_Bool mouse_events)
    evas.c_evas.Eina_Bool edje_edit_part_repeat_events_get(evas.c_evas.Evas_Object *obj, char *part)
    void edje_edit_part_repeat_events_set(evas.c_evas.Evas_Object *obj, char *part, evas.c_evas.Eina_Bool repeat_events)
    evas.c_evas.Evas_Event_Flags edje_edit_part_ignore_flags_get(evas.c_evas.Evas_Object *obj, char *part)
    void edje_edit_part_ignore_flags_set(evas.c_evas.Evas_Object *obj, char *part, evas.c_evas.Evas_Event_Flags ignore_flags)
    int edje_edit_part_drag_x_get(evas.c_evas.Evas_Object *obj, char *part)
    void edje_edit_part_drag_x_set(evas.c_evas.Evas_Object *obj, char *part, int drag)
    int edje_edit_part_drag_y_get(evas.c_evas.Evas_Object *obj, char *part)
    void edje_edit_part_drag_y_set(evas.c_evas.Evas_Object *obj, char *part, int drag)
    int edje_edit_part_drag_step_x_get(evas.c_evas.Evas_Object *obj, char *part)
    void edje_edit_part_drag_step_x_set(evas.c_evas.Evas_Object *obj, char *part, int step)
    int edje_edit_part_drag_step_y_get(evas.c_evas.Evas_Object *obj, char *part)
    void edje_edit_part_drag_step_y_set(evas.c_evas.Evas_Object *obj, char *part, int step)
    int edje_edit_part_drag_count_x_get(evas.c_evas.Evas_Object *obj, char *part)
    void edje_edit_part_drag_count_x_set(evas.c_evas.Evas_Object *obj, char *part, int count)
    int edje_edit_part_drag_count_y_get(evas.c_evas.Evas_Object *obj, char *part)
    void edje_edit_part_drag_count_y_set(evas.c_evas.Evas_Object *obj, char *part, int count)
    char* edje_edit_part_drag_confine_get(evas.c_evas.Evas_Object *obj, char *part)
    void edje_edit_part_drag_confine_set(evas.c_evas.Evas_Object *obj, char *part, char *confine)
    char* edje_edit_part_drag_event_get(evas.c_evas.Evas_Object *obj, char *part)
    void edje_edit_part_drag_event_set(evas.c_evas.Evas_Object *obj, char *part, char *event)

    # STATES API
    evas.c_evas.Eina_List * edje_edit_part_states_list_get(evas.c_evas.Evas_Object *obj, char *part)
    int edje_edit_state_name_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, char *new_name, double new_value)
    void edje_edit_state_add(evas.c_evas.Evas_Object *obj, char *part, char *name, double value)
    void edje_edit_state_del(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    evas.c_evas.Eina_Bool edje_edit_state_exist(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    evas.c_evas.Eina_Bool edje_edit_state_copy(evas.c_evas.Evas_Object *obj, char *part, char *sfrom, double vfrom, char *sto, double vto)
    double edje_edit_state_rel1_relative_x_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    double edje_edit_state_rel1_relative_y_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    double edje_edit_state_rel2_relative_x_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    double edje_edit_state_rel2_relative_y_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    void edje_edit_state_rel1_relative_x_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, double x)
    void edje_edit_state_rel1_relative_y_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, double y)
    void edje_edit_state_rel2_relative_x_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, double x)
    void edje_edit_state_rel2_relative_y_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, double y)
    int edje_edit_state_rel1_offset_x_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    int edje_edit_state_rel1_offset_y_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    int edje_edit_state_rel2_offset_x_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    int edje_edit_state_rel2_offset_y_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    void edje_edit_state_rel1_offset_x_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, double x)
    void edje_edit_state_rel1_offset_y_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, double y)
    void edje_edit_state_rel2_offset_x_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, double x)
    void edje_edit_state_rel2_offset_y_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, double y)
    char * edje_edit_state_rel1_to_x_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    char * edje_edit_state_rel1_to_y_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    char * edje_edit_state_rel2_to_x_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    char * edje_edit_state_rel2_to_y_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    void edje_edit_state_rel1_to_x_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, char *rel_to)
    void edje_edit_state_rel1_to_y_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, char *rel_to)
    void edje_edit_state_rel2_to_x_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, char *rel_to)
    void edje_edit_state_rel2_to_y_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, char *rel_to)
    void edje_edit_state_color_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, int *r, int *g, int *b, int *a)
    void edje_edit_state_color2_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, int *r, int *g, int *b, int *a)
    void edje_edit_state_color3_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, int *r, int *g, int *b, int *a)
    void edje_edit_state_color_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, int r, int g, int b, int a)
    void edje_edit_state_color2_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, int r, int g, int b, int a)
    void edje_edit_state_color3_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, int r, int g, int b, int a)
    double edje_edit_state_align_x_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    double edje_edit_state_align_y_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    void edje_edit_state_align_x_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, double align)
    void edje_edit_state_align_y_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, double align)
    int edje_edit_state_min_w_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    void edje_edit_state_min_w_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, int min_w)
    int edje_edit_state_min_h_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    void edje_edit_state_min_h_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, int min_h)
    int edje_edit_state_max_w_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    void edje_edit_state_max_w_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, int max_w)
    int edje_edit_state_max_h_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    void edje_edit_state_max_h_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, int max_h)
    double edje_edit_state_aspect_min_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    double edje_edit_state_aspect_max_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    void edje_edit_state_aspect_min_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, double aspect)
    void edje_edit_state_aspect_max_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, double aspect)
    unsigned char edje_edit_state_aspect_pref_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    void edje_edit_state_aspect_pref_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, unsigned char pref)
    double edje_edit_state_fill_origin_relative_x_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    double edje_edit_state_fill_origin_relative_y_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    int edje_edit_state_fill_origin_offset_x_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    int edje_edit_state_fill_origin_offset_y_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    void edje_edit_state_fill_origin_relative_x_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, double x)
    void edje_edit_state_fill_origin_relative_y_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, double x)
    void edje_edit_state_fill_origin_offset_x_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, double x)
    void edje_edit_state_fill_origin_offset_y_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, double y)
    double edje_edit_state_fill_size_relative_x_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    double edje_edit_state_fill_size_relative_y_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    int edje_edit_state_fill_size_offset_x_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    int edje_edit_state_fill_size_offset_y_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    void edje_edit_state_fill_size_relative_x_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, double x)
    void edje_edit_state_fill_size_relative_y_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, double x)
    void edje_edit_state_fill_size_offset_x_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, double x)
    void edje_edit_state_fill_size_offset_y_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, double y)
    evas.c_evas.Eina_Bool edje_edit_state_visible_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    void edje_edit_state_visible_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, evas.c_evas.Eina_Bool visible)
    char* edje_edit_state_color_class_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    void edje_edit_state_color_class_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, char *color_class)
    evas.c_evas.Eina_List * edje_edit_state_external_params_list_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    evas.c_evas.Eina_Bool edje_edit_state_external_param_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, char *param, edje.c_edje.Edje_External_Param_Type *type, void **val)
    evas.c_evas.Eina_Bool edje_edit_state_external_param_int_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, char *param, int *val)
    evas.c_evas.Eina_Bool edje_edit_state_external_param_bool_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, char *param, evas.c_evas.Eina_Bool *val)
    evas.c_evas.Eina_Bool edje_edit_state_external_param_double_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, char *param, double *val)
    evas.c_evas.Eina_Bool edje_edit_state_external_param_string_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, char *param, char **val)
    evas.c_evas.Eina_Bool edje_edit_state_external_param_choice_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, char *param, char **val)
    edje.c_edje.Edje_External_Param_Type edje_object_part_external_param_type_get(evas.c_evas.Evas_Object *obj, char *part, char *param)
    char *edje_external_param_type_str(edje.c_edje.Edje_External_Param_Type type)
    evas.c_evas.Eina_Bool edje_edit_state_external_param_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, char *param, edje.c_edje.Edje_External_Param_Type type, ...)
    evas.c_evas.Eina_Bool edje_edit_state_external_param_int_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, char *param, int val)
    evas.c_evas.Eina_Bool edje_edit_state_external_param_bool_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, char *param, evas.c_evas.Eina_Bool val)
    evas.c_evas.Eina_Bool edje_edit_state_external_param_double_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, char *param, double val)
    evas.c_evas.Eina_Bool edje_edit_state_external_param_string_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, char *param, char *val)
    evas.c_evas.Eina_Bool edje_edit_state_external_param_choice_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, char *param, char *val)

    # TEXT API
    char * edje_edit_state_text_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    void edje_edit_state_text_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, char *text)
    int edje_edit_state_text_size_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    void edje_edit_state_text_size_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, int size)
    double edje_edit_state_text_align_x_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    double edje_edit_state_text_align_y_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    void edje_edit_state_text_align_x_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, double align)
    void edje_edit_state_text_align_y_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, double align)
    double edje_edit_state_text_elipsis_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    void edje_edit_state_text_elipsis_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, double balance)
    evas.c_evas.Eina_Bool edje_edit_state_text_fit_x_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    void edje_edit_state_text_fit_x_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, evas.c_evas.Eina_Bool fit)
    evas.c_evas.Eina_Bool edje_edit_state_text_fit_y_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    void edje_edit_state_text_fit_y_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, evas.c_evas.Eina_Bool fit)
    evas.c_evas.Eina_List * edje_edit_fonts_list_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_Bool edje_edit_font_add(evas.c_evas.Evas_Object *obj, char* path, char* alias)
    evas.c_evas.Eina_Bool edje_edit_font_del(evas.c_evas.Evas_Object *obj, char* alias)
    char * edje_edit_state_font_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    void edje_edit_state_font_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, char *font)

    # IMAGES API
    evas.c_evas.Eina_List * edje_edit_images_list_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_Bool edje_edit_image_add(evas.c_evas.Evas_Object *obj, char* path)
    evas.c_evas.Eina_Bool edje_edit_image_del(evas.c_evas.Evas_Object *obj, char* name)
    evas.c_evas.Eina_Bool edje_edit_image_data_add(evas.c_evas.Evas_Object *obj, char *name, int id)
    char * edje_edit_state_image_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    void edje_edit_state_image_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, char *image)
    int edje_edit_image_id_get(evas.c_evas.Evas_Object *obj, char *image_name)
    Edje_Edit_Image_Comp edje_edit_image_compression_type_get(evas.c_evas.Evas_Object *obj, char *image)
    int edje_edit_image_compression_rate_get(evas.c_evas.Evas_Object *obj, char *image)
    void edje_edit_state_image_border_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, int *l, int *r, int *t, int *b)
    void edje_edit_state_image_border_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, int l, int r, int t, int b)
    unsigned char edje_edit_state_image_border_fill_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    void edje_edit_state_image_border_fill_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, unsigned char fill)
    evas.c_evas.Eina_List * edje_edit_state_tweens_list_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    evas.c_evas.Eina_Bool edje_edit_state_tween_add(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, char *tween)
    evas.c_evas.Eina_Bool edje_edit_state_tween_del(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, char *tween)

    # SPECTRUM API
    evas.c_evas.Eina_List * edje_edit_spectrum_list_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_Bool edje_edit_spectra_add(evas.c_evas.Evas_Object *obj, char* name)
    evas.c_evas.Eina_Bool edje_edit_spectra_del(evas.c_evas.Evas_Object *obj, char* spectra)
    evas.c_evas.Eina_Bool edje_edit_spectra_name_set(evas.c_evas.Evas_Object *obj, char* spectra, char* name)
    int edje_edit_spectra_stop_num_get(evas.c_evas.Evas_Object *obj, char* spectra)
    evas.c_evas.Eina_Bool edje_edit_spectra_stop_num_set(evas.c_evas.Evas_Object *obj, char* spectra, int num)
    evas.c_evas.Eina_Bool edje_edit_spectra_stop_color_get(evas.c_evas.Evas_Object *obj, char* spectra, int stop_number, int *r, int *g, int *b, int *a, int *d)
    evas.c_evas.Eina_Bool edje_edit_spectra_stop_color_set(evas.c_evas.Evas_Object *obj, char* spectra, int stop_number, int r, int g, int b, int a, int d)

    # GRADIENT API
    char * edje_edit_state_gradient_type_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    evas.c_evas.Eina_Bool edje_edit_state_gradient_type_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, char *type)
    evas.c_evas.Eina_Bool edje_edit_state_gradient_use_fill_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    char * edje_edit_state_gradient_spectra_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    evas.c_evas.Eina_Bool edje_edit_state_gradient_spectra_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, char* spectra)
    int edje_edit_state_gradient_angle_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    void edje_edit_state_gradient_angle_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, int angle)
    double edje_edit_state_gradient_rel1_relative_x_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    double edje_edit_state_gradient_rel1_relative_y_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    double edje_edit_state_gradient_rel2_relative_x_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    double edje_edit_state_gradient_rel2_relative_y_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    evas.c_evas.Eina_Bool edje_edit_state_gradient_rel1_relative_x_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, double val)
    evas.c_evas.Eina_Bool edje_edit_state_gradient_rel1_relative_y_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, double val)
    evas.c_evas.Eina_Bool edje_edit_state_gradient_rel2_relative_x_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, double val)
    evas.c_evas.Eina_Bool edje_edit_state_gradient_rel2_relative_y_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, double val)
    int edje_edit_state_gradient_rel1_offset_x_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    int edje_edit_state_gradient_rel1_offset_y_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    int edje_edit_state_gradient_rel2_offset_x_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    int edje_edit_state_gradient_rel2_offset_y_get(evas.c_evas.Evas_Object *obj, char *part, char *state, double value)
    evas.c_evas.Eina_Bool edje_edit_state_gradient_rel1_offset_x_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, int val)
    evas.c_evas.Eina_Bool edje_edit_state_gradient_rel1_offset_y_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, int val)
    evas.c_evas.Eina_Bool edje_edit_state_gradient_rel2_offset_x_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, int val)
    evas.c_evas.Eina_Bool edje_edit_state_gradient_rel2_offset_y_set(evas.c_evas.Evas_Object *obj, char *part, char *state, double value, int val)

    # PROGRAMS API
    evas.c_evas.Eina_List* edje_edit_programs_list_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_Bool edje_edit_program_add(evas.c_evas.Evas_Object *obj, char *name)
    evas.c_evas.Eina_Bool edje_edit_program_del(evas.c_evas.Evas_Object *obj, char *prog)
    evas.c_evas.Eina_Bool edje_edit_program_exist(evas.c_evas.Evas_Object *obj, char *prog)
    evas.c_evas.Eina_Bool edje_edit_program_run(evas.c_evas.Evas_Object *obj, char *prog)
    evas.c_evas.Eina_Bool edje_edit_program_name_set(evas.c_evas.Evas_Object *obj, char *prog, char *new_name)
    char * edje_edit_program_source_get(evas.c_evas.Evas_Object *obj, char *prog)
    evas.c_evas.Eina_Bool edje_edit_program_source_set(evas.c_evas.Evas_Object *obj, char *prog, char *source)
    char * edje_edit_program_signal_get(evas.c_evas.Evas_Object *obj, char *prog)
    evas.c_evas.Eina_Bool edje_edit_program_signal_set(evas.c_evas.Evas_Object *obj, char *prog, char *signal)
    double edje_edit_program_in_from_get(evas.c_evas.Evas_Object *obj, char *prog)
    evas.c_evas.Eina_Bool edje_edit_program_in_from_set(evas.c_evas.Evas_Object *obj, char *prog, double seconds)
    double edje_edit_program_in_range_get(evas.c_evas.Evas_Object *obj, char *prog)
    evas.c_evas.Eina_Bool edje_edit_program_in_range_set(evas.c_evas.Evas_Object *obj, char *prog, double seconds)
    edje.c_edje.Edje_Action_Type edje_edit_program_action_get(evas.c_evas.Evas_Object *obj, char *prog)
    evas.c_evas.Eina_Bool edje_edit_program_action_set(evas.c_evas.Evas_Object *obj, char *prog, edje.c_edje.Edje_Action_Type action)
    evas.c_evas.Eina_List* edje_edit_program_targets_get(evas.c_evas.Evas_Object *, char *prog)
    evas.c_evas.Eina_Bool edje_edit_program_target_add(evas.c_evas.Evas_Object *obj, char *prog, char *target)
    evas.c_evas.Eina_Bool edje_edit_program_target_del(evas.c_evas.Evas_Object *obj, char *prog, char *target)
    evas.c_evas.Eina_Bool edje_edit_program_targets_clear(evas.c_evas.Evas_Object *obj, char *prog)
    evas.c_evas.Eina_List* edje_edit_program_afters_get(evas.c_evas.Evas_Object *obj, char *prog)
    evas.c_evas.Eina_Bool edje_edit_program_after_add(evas.c_evas.Evas_Object *obj, char *prog, char *after)
    evas.c_evas.Eina_Bool edje_edit_program_after_del(evas.c_evas.Evas_Object *obj, char *prog, char *after)
    evas.c_evas.Eina_Bool edje_edit_program_afters_clear(evas.c_evas.Evas_Object *obj, char *prog)
    char* edje_edit_program_state_get(evas.c_evas.Evas_Object *obj, char *prog)
    evas.c_evas.Eina_Bool edje_edit_program_state_set(evas.c_evas.Evas_Object *obj, char *prog, char *state)
    double edje_edit_program_value_get(evas.c_evas.Evas_Object *obj, char *prog)
    evas.c_evas.Eina_Bool edje_edit_program_value_set(evas.c_evas.Evas_Object *obj, char *prog, double value)
    char* edje_edit_program_state2_get(evas.c_evas.Evas_Object *obj, char *prog)
    evas.c_evas.Eina_Bool edje_edit_program_state2_set(evas.c_evas.Evas_Object *obj, char *prog, char *state2)
    double edje_edit_program_value2_get(evas.c_evas.Evas_Object *obj, char *prog)
    evas.c_evas.Eina_Bool edje_edit_program_value2_set(evas.c_evas.Evas_Object *obj, char *prog, double value)
    edje.c_edje.Edje_Tween_Mode edje_edit_program_transition_get(evas.c_evas.Evas_Object *obj, char *prog)
    evas.c_evas.Eina_Bool edje_edit_program_transition_set(evas.c_evas.Evas_Object *obj, char *prog, edje.c_edje.Edje_Tween_Mode transition)
    double edje_edit_program_transition_time_get(evas.c_evas.Evas_Object *obj, char *prog)
    evas.c_evas.Eina_Bool edje_edit_program_transition_time_set(evas.c_evas.Evas_Object *obj, char *prog, double seconds)

    # SCRIPTS API
    char* edje_edit_script_get(evas.c_evas.Evas_Object *obj)

cdef public class EdjeEdit(edje.c_edje.Edje) [object PyEdjeEdit, type PyEdjeEdit_Type]:
    pass
#    cdef object _text_change_cb
#    cdef object _message_handler_cb
#    cdef object _signal_callbacks
#
#    cdef void message_send_int(self, int id, int data)
#    cdef void message_send_float(self, int id, float data)
#    cdef void message_send_str(self, int id, char *data)
#    cdef void message_send_str_set(self, int id, data)
#    cdef void message_send_str_int(self, int id, char *s, int i)
#    cdef void message_send_str_float(self, int id, char *s, float f)
#    cdef void message_send_str_int_set(self, int id, char *s, data)
#    cdef void message_send_str_float_set(self, int id, char *s, data)
#    cdef void message_send_int_set(self, int id, data)
#    cdef void message_send_float_set(self, int id, data)
#    cdef message_send_set(self, int id, data)

