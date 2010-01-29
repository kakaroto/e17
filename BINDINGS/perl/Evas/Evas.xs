#include "common.h"

#include <Evas.h>

#include "evas-const-c.inc"

#ifndef DEBUG
#define DEBUG 0
#endif

MODULE = EFL::Evas		PACKAGE = EFL::Evas

INCLUDE: evas-const-xs.inc

PROTOTYPES: ENABLE

# ------------------------------------------------------------------------------

# From #define's

void
evas_object_size_hint_fill_set(Evas_Object *obj, double x, double y)

void
evas_object_size_hint_fill_get(obj, x, y)
    Evas_Object *obj
    double x = NO_INIT
    double y = NO_INIT
    CODE:
        evas_object_size_hint_fill_get(obj, &x, &y);
    OUTPUT:
        x
        y


# Evas functions

int
evas_alloc_error()

int
evas_init()

int
evas_shutdown()

Evas *
evas_new()

void
evas_free(Evas *e)

int
evas_render_method_lookup(const char *name)

Eina_List *
evas_render_method_list()

void
evas_render_method_list_free(Eina_List *list)

void
evas_output_method_set(Evas *e, int render_method)

int
evas_output_method_get(Evas *e)

Evas_Engine_Info *
evas_engine_info_get(Evas *e)

int
evas_engine_info_set(Evas *e, Evas_Engine_Info *info)

void
evas_output_size_set(Evas *e, int w, int h)

void
evas_output_size_get(Evas *e, int *w, int *h)

void
evas_output_viewport_set(Evas *e, Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h)

void
evas_output_viewport_get(Evas *e, Evas_Coord *x, Evas_Coord *y, Evas_Coord *w, Evas_Coord *h)

Evas_Coord
evas_coord_screen_x_to_world(Evas *e, int x)

Evas_Coord
evas_coord_screen_y_to_world(Evas *e, int y)

int
evas_coord_world_x_to_screen(Evas *e, Evas_Coord x)

int
evas_coord_world_y_to_screen(Evas *e, Evas_Coord y)

void
evas_pointer_output_xy_get(Evas *e, int *x, int *y)

void
evas_pointer_canvas_xy_get(Evas *e, Evas_Coord *x, Evas_Coord *y)

int
evas_pointer_button_down_mask_get(Evas *e)

Eina_Bool
evas_pointer_inside_get(Evas *e)

void
evas_data_attach_set(Evas *e, void *data)

void *
evas_data_attach_get(Evas *e)

void
evas_damage_rectangle_add(Evas *e, int x, int y, int w, int h)

void
evas_obscured_rectangle_add(Evas *e, int x, int y, int w, int h)

void
evas_obscured_clear(Evas *e)

Eina_List *
evas_render_updates(Evas *e)

void
evas_render_updates_free(Eina_List *updates)

void
evas_render(Evas *e)

void
evas_norender(Evas *e)

void
evas_render_idle_flush(Evas *e)

Evas_Object *
evas_object_rectangle_add(Evas *e)

Evas_Object *
evas_object_line_add(Evas *e)

void
evas_object_line_xy_set(Evas_Object *obj, Evas_Coord x1, Evas_Coord y1, Evas_Coord x2, Evas_Coord y2)

void
evas_object_line_xy_get(Evas_Object *obj, Evas_Coord *x1, Evas_Coord *y1, Evas_Coord *x2, Evas_Coord *y2)

Evas_Object *
evas_object_gradient_add(Evas *e)

void
evas_object_gradient_color_stop_add(Evas_Object *obj, int r, int g, int b, int a, int delta)

void
evas_object_gradient_alpha_stop_add(Evas_Object *obj, int a, int delta)

void
evas_object_gradient_color_data_set(Evas_Object *obj, void *color_data, int len, Eina_Bool has_alpha)

void
evas_object_gradient_alpha_data_set(Evas_Object *obj, void *alpha_data, int len)

void
evas_object_gradient_clear(Evas_Object *obj)

void
evas_object_gradient_type_set(Evas_Object *obj, char *type, char *instance_params)

=pod

void
evas_object_gradient_type_get(Evas_Object *obj, char **type, char **instance_params)

=cut

void
evas_object_gradient_fill_set(Evas_Object *obj, Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h)

void
evas_object_gradient_fill_get(Evas_Object *obj, Evas_Coord *x, Evas_Coord *y, Evas_Coord *w, Evas_Coord *h)

void
evas_object_gradient_fill_angle_set(Evas_Object *obj, Evas_Angle angle)

Evas_Angle
evas_object_gradient_fill_angle_get(Evas_Object *obj)

void
evas_object_gradient_fill_spread_set(Evas_Object *obj, int tile_mode)

int
evas_object_gradient_fill_spread_get(Evas_Object *obj)

void
evas_object_gradient_angle_set(Evas_Object *obj, Evas_Angle angle)

Evas_Angle
evas_object_gradient_angle_get(Evas_Object *obj)

void
evas_object_gradient_direction_set(Evas_Object *obj, int direction)

int
evas_object_gradient_direction_get(Evas_Object *obj)

void
evas_object_gradient_offset_set(Evas_Object *obj, float offset)

float
evas_object_gradient_offset_get(Evas_Object *obj)

void
evas_object_gradient2_color_np_stop_insert(Evas_Object *obj, int r, int g, int b, int a, float pos)

void
evas_object_gradient2_fill_spread_set(Evas_Object *obj, int tile_mode)

int
evas_object_gradient2_fill_spread_get(Evas_Object *obj)

void
evas_object_gradient2_fill_transform_set(Evas_Object *obj, Evas_Transform *t)

void
evas_object_gradient2_fill_transform_get(Evas_Object *obj, Evas_Transform *t)

Evas_Object *
evas_object_gradient2_linear_add(Evas *e)

void
evas_object_gradient2_linear_fill_set(Evas_Object *obj, float x0, float y0, float x1, float y1)

void
evas_object_gradient2_linear_fill_get(Evas_Object *obj, float *x0, float *y0, float *x1, float *y1)

Evas_Object *
evas_object_gradient2_radial_add(Evas *e)

void
evas_object_gradient2_radial_fill_set(Evas_Object *obj, float cx, float cy, float rx, float ry)

void
evas_object_gradient2_radial_fill_get(Evas_Object *obj, float *cx, float *cy, float *rx, float *ry)

Evas_Object *
evas_object_polygon_add(Evas *e)

void
evas_object_polygon_point_add(Evas_Object *obj, Evas_Coord x, Evas_Coord y)

void
evas_object_polygon_points_clear(Evas_Object *obj)

Evas_Object *
evas_object_image_add(Evas *e)

Evas_Object *
evas_object_image_filled_add(Evas *e)

void
evas_object_image_file_set(Evas_Object *obj, char *file, char *key)

=pod

void
evas_object_image_file_get(Evas_Object *obj, char **file, char **key)

=cut

void
evas_object_image_border_set(Evas_Object *obj, int l, int r, int t, int b)

void
evas_object_image_border_get(Evas_Object *obj, int *l, int *r, int *t, int *b)

void
evas_object_image_border_center_fill_set(Evas_Object *obj, Evas_Border_Fill_Mode fill)

Evas_Border_Fill_Mode
evas_object_image_border_center_fill_get(Evas_Object *obj)

void
evas_object_image_filled_set(Evas_Object *obj, Eina_Bool setting)

Eina_Bool
evas_object_image_filled_get(Evas_Object *obj)

void
evas_object_image_fill_set(Evas_Object *obj, Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h)

void
evas_object_image_fill_get(Evas_Object *obj, Evas_Coord *x, Evas_Coord *y, Evas_Coord *w, Evas_Coord *h)

void
evas_object_image_fill_spread_set(Evas_Object *obj, int tile_mode)

int
evas_object_image_fill_spread_get(Evas_Object *obj)

void
evas_object_image_fill_transform_set(Evas_Object *obj, Evas_Transform *t)

void
evas_object_image_fill_transform_get(Evas_Object *obj, Evas_Transform *t)

void
evas_object_image_size_set(Evas_Object *obj, int w, int h)

void
evas_object_image_size_get(Evas_Object *obj, int *w, int *h)

int
evas_object_image_stride_get(Evas_Object *obj)

int
evas_object_image_load_error_get(Evas_Object *obj)

void
evas_object_image_data_set(Evas_Object *obj, void *data)

void *
evas_object_image_data_convert(Evas_Object *obj, Evas_Colorspace to_cspace)

void *
evas_object_image_data_get(Evas_Object *obj, Eina_Bool for_writing)

void
evas_object_image_data_copy_set(Evas_Object *obj, void *data)

void
evas_object_image_data_update_add(Evas_Object *obj, int x, int y, int w, int h)

void
evas_object_image_alpha_set(Evas_Object *obj, Eina_Bool has_alpha)

Eina_Bool
evas_object_image_alpha_get(Evas_Object *obj)

void
evas_object_image_smooth_scale_set(Evas_Object *obj, Eina_Bool smooth_scale)

Eina_Bool
evas_object_image_smooth_scale_get(Evas_Object *obj)

void
evas_object_image_preload(Evas_Object *obj, Eina_Bool cancel)

void
evas_object_image_reload(Evas_Object *obj)

Eina_Bool
evas_object_image_save(Evas_Object *obj, char *file, char *key, char *flags)

Eina_Bool
evas_object_image_pixels_import(Evas_Object *obj, Evas_Pixel_Import_Source *pixels)

=pod

void
evas_object_image_pixels_get_callback_set(Evas_Object *obj, void (*func) (void *data, Evas_Object *o), void *data)

=cut

void
evas_object_image_pixels_dirty_set(Evas_Object *obj, Eina_Bool dirty)

Eina_Bool
evas_object_image_pixels_dirty_get(Evas_Object *obj)

void
evas_object_image_load_dpi_set(Evas_Object *obj, double dpi)

double
evas_object_image_load_dpi_get(Evas_Object *obj)

void
evas_object_image_load_size_set(Evas_Object *obj, int w, int h)

void
evas_object_image_load_size_get(Evas_Object *obj, int *w, int *h)

void
evas_object_image_load_scale_down_set(Evas_Object *obj, int scale_down)

int
evas_object_image_load_scale_down_get(Evas_Object *obj)

void
evas_object_image_load_region_set(Evas_Object *obj, int x, int y, int w, int h)

void
evas_object_image_load_region_get(Evas_Object *obj, int *x, int *y, int *w, int *h)

void
evas_object_image_colorspace_set(Evas_Object *obj, Evas_Colorspace cspace)

Evas_Colorspace
evas_object_image_colorspace_get(Evas_Object *obj)

void
evas_object_image_native_surface_set(Evas_Object *obj, Evas_Native_Surface *surf)

Evas_Native_Surface *
evas_object_image_native_surface_get  (Evas_Object *obj)

void
evas_object_image_scale_hint_set(Evas_Object *obj, Evas_Image_Scale_Hint hint)

Evas_Image_Scale_Hint
evas_object_image_scale_hint_get(Evas_Object *obj)

void
evas_image_cache_flush(Evas *e)

void
evas_image_cache_reload(Evas *e)

void
evas_image_cache_set(Evas *e, int size)

int
evas_image_cache_get(Evas *e)

Evas_Object *
evas_object_text_add(Evas *e)

void
evas_object_text_font_source_set(Evas_Object *obj, char *font)

const char *
evas_object_text_font_source_get(Evas_Object *obj)

void
evas_object_text_font_set(Evas_Object *obj, char *font, Evas_Font_Size size)

=pod

void
evas_object_text_font_get(Evas_Object *obj, char **font, Evas_Font_Size *size)

=cut

void
evas_object_text_text_set(Evas_Object *obj, char *text)

const char *
evas_object_text_text_get(Evas_Object *obj)

Evas_Coord
evas_object_text_ascent_get(Evas_Object *obj)

Evas_Coord
evas_object_text_descent_get(Evas_Object *obj)

Evas_Coord
evas_object_text_max_ascent_get(Evas_Object *obj)

Evas_Coord
evas_object_text_max_descent_get(Evas_Object *obj)

Evas_Coord
evas_object_text_horiz_advance_get(Evas_Object *obj)

Evas_Coord
evas_object_text_vert_advance_get(Evas_Object *obj)

Evas_Coord
evas_object_text_inset_get(Evas_Object *obj)

Eina_Bool
evas_object_text_char_pos_get(Evas_Object *obj, int pos, Evas_Coord *cx, Evas_Coord *cy, Evas_Coord *cw, Evas_Coord *ch)

int
evas_object_text_char_coords_get(Evas_Object *obj, Evas_Coord x, Evas_Coord y, Evas_Coord *cx, Evas_Coord *cy, Evas_Coord *cw, Evas_Coord *ch)

int
evas_object_text_last_up_to_pos(Evas_Object *obj, Evas_Coord x, Evas_Coord y)

Evas_Text_Style_Type
evas_object_text_style_get(Evas_Object *obj)

void
evas_object_text_style_set(Evas_Object *obj, Evas_Text_Style_Type type)

void
evas_object_text_shadow_color_set(Evas_Object *obj, int r, int g, int b, int a)

void
evas_object_text_shadow_color_get(Evas_Object *obj, int *r, int *g, int *b, int *a)

void
evas_object_text_glow_color_set(Evas_Object *obj, int r, int g, int b, int a)

void
evas_object_text_glow_color_get(Evas_Object *obj, int *r, int *g, int *b, int *a)

void
evas_object_text_glow2_color_set(Evas_Object *obj, int r, int g, int b, int a)

void
evas_object_text_glow2_color_get(Evas_Object *obj, int *r, int *g, int *b, int *a)

void
evas_object_text_outline_color_set(Evas_Object *obj, int r, int g, int b, int a)

void
evas_object_text_outline_color_get(Evas_Object *obj, int *r, int *g, int *b, int *a)

void
evas_object_text_style_pad_get(Evas_Object *obj, int *l, int *r, int *t, int *b)

int
evas_string_char_next_get(char *str, int pos, int *decoded)

int
evas_string_char_prev_get(char *str, int pos, int *decoded)

int
evas_string_char_len_get(char *str)

void
evas_font_path_clear(Evas *e)

void
evas_font_path_append(Evas *e, char *path)

void
evas_font_path_prepend(Evas *e, char *path)

const Eina_List *
evas_font_path_list(Evas *e)

void
evas_font_hinting_set(Evas *e, Evas_Font_Hinting_Flags hinting)

Evas_Font_Hinting_Flags
evas_font_hinting_get(Evas *e)

Eina_Bool
evas_font_hinting_can_hint(Evas *e, Evas_Font_Hinting_Flags hinting)

void
evas_font_cache_flush(Evas *e)

void
evas_font_cache_set(Evas *e, int size)

int
evas_font_cache_get(Evas *e)

Eina_List *
evas_font_available_list(const Evas *e)

void
evas_font_available_list_free(Evas *e, Eina_List *available)

Evas_Object *
evas_object_textblock_add(Evas *e)

const char *
evas_textblock_escape_string_get(const char *escape)

const char *
evas_textblock_string_escape_get(const char *string, int *len_ret)

const char *
evas_textblock_escape_string_range_get(const char *escape_start, const char *escape_end)

Evas_Textblock_Style *
evas_textblock_style_new()

void
evas_textblock_style_free(Evas_Textblock_Style *ts)

void
evas_textblock_style_set(Evas_Textblock_Style *ts, const char *text)

const char *
evas_textblock_style_get(Evas_Textblock_Style *ts)

void
evas_object_textblock_style_set(Evas_Object *obj, Evas_Textblock_Style *ts)

=pod

const Evas_Textblock_Style *
evas_object_textblock_style_get(const Evas_Object *obj)

=cut

void
evas_object_textblock_replace_char_set(Evas_Object *obj, const char *ch)

const char *
evas_object_textblock_replace_char_get(Evas_Object *obj)

void
evas_object_textblock_text_markup_set(Evas_Object *obj, const char *text)

void
evas_object_textblock_text_markup_prepend(Evas_Textblock_Cursor *cur, char *text)

const char *
evas_object_textblock_text_markup_get(const Evas_Object *obj)

const Evas_Textblock_Cursor *
evas_object_textblock_cursor_get(const Evas_Object *obj)

Evas_Textblock_Cursor *
evas_object_textblock_cursor_new(Evas_Object *obj)

void
evas_textblock_cursor_free(Evas_Textblock_Cursor *cur)

void
evas_textblock_cursor_node_first(Evas_Textblock_Cursor *cur)

void
evas_textblock_cursor_node_last(Evas_Textblock_Cursor *cur)

Eina_Bool
evas_textblock_cursor_node_next(Evas_Textblock_Cursor *cur)

Eina_Bool
evas_textblock_cursor_node_prev(Evas_Textblock_Cursor *cur)

Eina_Bool
evas_textblock_cursor_char_next(Evas_Textblock_Cursor *cur)

Eina_Bool
evas_textblock_cursor_char_prev(Evas_Textblock_Cursor *cur)

void
evas_textblock_cursor_char_first(Evas_Textblock_Cursor *cur)

void
evas_textblock_cursor_char_last(Evas_Textblock_Cursor *cur)

void
evas_textblock_cursor_line_first(Evas_Textblock_Cursor *cur)

void
evas_textblock_cursor_line_last(Evas_Textblock_Cursor *cur)

int
evas_textblock_cursor_pos_get(Evas_Textblock_Cursor *cur)

void
evas_textblock_cursor_pos_set(Evas_Textblock_Cursor *cur, int pos)

Eina_Bool
evas_textblock_cursor_line_set(Evas_Textblock_Cursor *cur, int line)

int
evas_textblock_cursor_compare(Evas_Textblock_Cursor *cur1, Evas_Textblock_Cursor *cur2)

void
evas_textblock_cursor_copy(Evas_Textblock_Cursor *cur, Evas_Textblock_Cursor *cur_dest)

void
evas_textblock_cursor_text_append(Evas_Textblock_Cursor *cur, char *text)

void
evas_textblock_cursor_text_prepend(Evas_Textblock_Cursor *cur, char *text)

void
evas_textblock_cursor_format_append(Evas_Textblock_Cursor *cur, char *format)

void
evas_textblock_cursor_format_prepend(Evas_Textblock_Cursor *cur, char *format)

void
evas_textblock_cursor_node_delete(Evas_Textblock_Cursor *cur)

void
evas_textblock_cursor_char_delete(Evas_Textblock_Cursor *cur)

void
evas_textblock_cursor_range_delete(Evas_Textblock_Cursor *cur1, Evas_Textblock_Cursor *cur2)

=pod

const char *
evas_textblock_cursor_node_text_get(const Evas_Textblock_Cursor *cur)

=cut

int
evas_textblock_cursor_node_text_length_get(Evas_Textblock_Cursor *cur)

=pod

const char *
evas_textblock_cursor_node_format_get(const Evas_Textblock_Cursor *cur)

=cut

Eina_Bool
evas_textblock_cursor_node_format_is_visible_get(Evas_Textblock_Cursor *cur)

char *
evas_textblock_cursor_range_text_get(Evas_Textblock_Cursor *cur1, Evas_Textblock_Cursor *cur2, Evas_Textblock_Text_Type format)

int
evas_textblock_cursor_char_geometry_get(Evas_Textblock_Cursor *cur, Evas_Coord *cx, Evas_Coord *cy, Evas_Coord *cw, Evas_Coord *ch)

int
evas_textblock_cursor_line_geometry_get(Evas_Textblock_Cursor *cur, Evas_Coord *cx, Evas_Coord *cy, Evas_Coord *cw, Evas_Coord *ch)

Eina_Bool
evas_textblock_cursor_char_coord_set(Evas_Textblock_Cursor *cur, Evas_Coord x, Evas_Coord y)

int
evas_textblock_cursor_line_coord_set(Evas_Textblock_Cursor *cur, Evas_Coord y)

Eina_List *
evas_textblock_cursor_range_geometry_get(Evas_Textblock_Cursor *cur1, Evas_Textblock_Cursor *cur2)

Eina_Bool
evas_textblock_cursor_eol_get(Evas_Textblock_Cursor *cur)

void
evas_textblock_cursor_eol_set(Evas_Textblock_Cursor *cur, Eina_Bool eol)

Eina_Bool
evas_object_textblock_line_number_geometry_get(Evas_Object *obj, int line, Evas_Coord *cx, Evas_Coord *cy, Evas_Coord *cw, Evas_Coord *ch)

void
evas_object_textblock_clear(Evas_Object *obj)

void
evas_object_textblock_size_formatted_get(Evas_Object *obj, Evas_Coord *w, Evas_Coord *h)

void
evas_object_textblock_size_native_get(Evas_Object *obj, Evas_Coord *w, Evas_Coord *h)

void
evas_object_textblock_style_insets_get(Evas_Object *obj, Evas_Coord *l, Evas_Coord *r, Evas_Coord *t, Evas_Coord *b)

void
evas_object_del(Evas_Object *obj)

const char *
evas_object_type_get(const Evas_Object *obj)

void
evas_object_layer_set(Evas_Object *obj, short l)

short
evas_object_layer_get(Evas_Object *obj)

void
evas_object_raise(Evas_Object *obj)

void
evas_object_lower(Evas_Object *obj)

void
evas_object_stack_above(Evas_Object *obj, Evas_Object *above)

void
evas_object_stack_below(Evas_Object *obj, Evas_Object *below)

Evas_Object *
evas_object_above_get(Evas_Object *obj)

Evas_Object *
evas_object_below_get(Evas_Object *obj)

Evas_Object *
evas_object_bottom_get(Evas *e)

Evas_Object *
evas_object_top_get(Evas *e)

void
evas_object_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y)

void
evas_object_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h)

void
evas_object_geometry_get(Evas_Object *obj, Evas_Coord *x, Evas_Coord *y, Evas_Coord *w, Evas_Coord *h)

void
evas_object_size_hint_min_get(Evas_Object *obj, Evas_Coord *w, Evas_Coord *h)

void
evas_object_size_hint_min_set(Evas_Object *obj, Evas_Coord w, Evas_Coord h)

void
evas_object_size_hint_max_get(Evas_Object *obj, Evas_Coord *w, Evas_Coord *h)

void
evas_object_size_hint_max_set(Evas_Object *obj, Evas_Coord w, Evas_Coord h)

void
evas_object_size_hint_request_get(Evas_Object *obj, Evas_Coord *w, Evas_Coord *h)

void
evas_object_size_hint_request_set(Evas_Object *obj, Evas_Coord w, Evas_Coord h)

void
evas_object_size_hint_aspect_get(Evas_Object *obj, Evas_Aspect_Control *aspect, Evas_Coord *w, Evas_Coord *h)

void
evas_object_size_hint_aspect_set(Evas_Object *obj, Evas_Aspect_Control aspect, Evas_Coord w, Evas_Coord h)

void
evas_object_size_hint_align_get(Evas_Object *obj, double *x, double *y)

void
evas_object_size_hint_align_set(Evas_Object *obj, double x, double y)

void
evas_object_size_hint_weight_get(Evas_Object *obj, double *x, double *y)

void
evas_object_size_hint_weight_set(Evas_Object *obj, double x, double y)

void
evas_object_size_hint_padding_get(Evas_Object *obj, Evas_Coord *l, Evas_Coord *r, Evas_Coord *t, Evas_Coord *b)

void
evas_object_size_hint_padding_set(Evas_Object *obj, Evas_Coord l, Evas_Coord r, Evas_Coord t, Evas_Coord b)

void
evas_object_show(Evas_Object *obj)

void
evas_object_hide(Evas_Object *obj)

Eina_Bool
evas_object_visible_get(Evas_Object *obj)

void
evas_object_render_op_set(Evas_Object *obj, Evas_Render_Op op)

Evas_Render_Op
evas_object_render_op_get(Evas_Object *obj)

void
evas_object_anti_alias_set(Evas_Object *obj, Eina_Bool antialias)

Eina_Bool
evas_object_anti_alias_get(Evas_Object *obj)

void
evas_object_scale_set(Evas_Object *obj, double scale)

double
evas_object_scale_get(Evas_Object *obj)

void
evas_object_color_set(Evas_Object *obj, int r, int g, int b, int a)

void
evas_object_color_get(Evas_Object *obj, int *r, int *g, int *b, int *a)

void
evas_object_color_interpolation_set(Evas_Object *obj, int color_space)

int
evas_object_color_interpolation_get(Evas_Object *obj)

void
evas_object_clip_set(Evas_Object *obj, Evas_Object *clip)

Evas_Object *
evas_object_clip_get(Evas_Object *obj)

void
evas_object_clip_unset(Evas_Object *obj)

const Eina_List *
evas_object_clipees_get(const Evas_Object *obj)

void
evas_object_data_set(Evas_Object *obj, char *key, void *data)

void *
evas_object_data_get(Evas_Object *obj, char *key)

void *
evas_object_data_del(Evas_Object *obj, char *key)

void
evas_object_name_set(Evas_Object *obj, const char *name)

const char *
evas_object_name_get(const Evas_Object *obj)

Evas_Object *
evas_object_name_find(Evas *e, char *name)

Evas *
evas_object_evas_get(Evas_Object *obj)

Evas_Object *
evas_object_top_at_xy_get(Evas *e, Evas_Coord x, Evas_Coord y, Eina_Bool include_pass_events_objects, Eina_Bool include_hidden_objects)

Evas_Object *
evas_object_top_at_pointer_get(Evas *e)

Evas_Object *
evas_object_top_in_rectangle_get  (Evas *e, Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h, Eina_Bool include_pass_events_objects, Eina_Bool include_hidden_objects)

Eina_List *
evas_objects_at_xy_get(Evas *e, Evas_Coord x, Evas_Coord y, Eina_Bool include_pass_events_objects, Eina_Bool include_hidden_objects)

Eina_List *
evas_objects_in_rectangle_get(Evas *e, Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h, Eina_Bool include_pass_events_objects, Eina_Bool include_hidden_objects)

void
evas_object_map_enable_set(Evas_Object *obj, Eina_Bool enabled)

Eina_Bool
evas_object_map_enable_get(Evas_Object *obj)

void
evas_object_map_set(Evas_Object *obj, Evas_Map *map)

=pod

const Evas_Map *
evas_object_map_get(const Evas_Object *obj)

=cut

void
evas_map_util_points_populate_from_object_full(Evas_Map *m, Evas_Object *obj, Evas_Coord z)

void
evas_map_util_points_populate_from_object(Evas_Map *m, Evas_Object *obj)

void
evas_map_util_points_populate_from_geometry(Evas_Map *m, Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h, Evas_Coord z)

void
evas_map_util_points_color_set(Evas_Map *m, int r, int g, int b, int a)

void
evas_map_util_rotate(Evas_Map *m, double degrees, Evas_Coord cx, Evas_Coord cy)

void
evas_map_util_zoom(Evas_Map *m, double zoomx, double zoomy, Evas_Coord cx, Evas_Coord cy)

void
evas_map_util_3d_rotate(Evas_Map *m, double dx, double dy, double dz, Evas_Coord cx, Evas_Coord cy, Evas_Coord cz)

void
evas_map_util_3d_lighting(Evas_Map *m, Evas_Coord lx, Evas_Coord ly, Evas_Coord lz, int lr, int lg, int lb, int ar, int ab, int ag)

void
evas_map_util_3d_perspective(Evas_Map *m, Evas_Coord px, Evas_Coord py, Evas_Coord z0, Evas_Coord foc)

Eina_Bool
evas_map_util_clockwise_get(Evas_Map *m)

Evas_Map *
evas_map_new(int count)

void
evas_map_smooth_set(Evas_Map *m, Eina_Bool enabled)

Eina_Bool
evas_map_smooth_get(Evas_Map *m)

void
evas_map_alpha_set(Evas_Map *m, Eina_Bool enabled)

Eina_Bool
evas_map_alpha_get(Evas_Map *m)

Evas_Map *
evas_map_dup(Evas_Map *m)

void
evas_map_free(Evas_Map *m)

void
evas_map_point_coord_set(Evas_Map *m, int idx, Evas_Coord x, Evas_Coord y, Evas_Coord z)

void
evas_map_point_coord_get(Evas_Map *m, int idx, Evas_Coord *x, Evas_Coord *y, Evas_Coord *z)

void
evas_map_point_image_uv_set(Evas_Map *m, int idx, double u, double v)

void
evas_map_point_image_uv_get(Evas_Map *m, int idx, double *u, double *v)

void
evas_map_point_color_set(Evas_Map *m, int idx, int r, int g, int b, int a)

void
evas_map_point_color_get(Evas_Map *m, int idx, int *r, int *g, int *b, int *a)

void
evas_smart_free(Evas_Smart *s)

Evas_Smart *
evas_smart_class_new(Evas_Smart_Class *sc)

=pod

const Evas_Smart_Class *
evas_smart_class_get(const Evas_Smart *s)

=cut

void *
evas_smart_data_get(Evas_Smart *s)

Evas_Object *
evas_object_smart_add(Evas *e, Evas_Smart *s)

void
evas_object_smart_member_add(Evas_Object *obj, Evas_Object *smart_obj)

void
evas_object_smart_member_del(Evas_Object *obj)

Evas_Object *
evas_object_smart_parent_get(Evas_Object *obj)

Eina_List *
evas_object_smart_members_get(Evas_Object *obj)

Evas_Smart *
evas_object_smart_smart_get(Evas_Object *obj)

void *
evas_object_smart_data_get(Evas_Object *obj)

void
evas_object_smart_data_set(Evas_Object *obj, void *data)

void
evas_object_smart_callback_add(obj, event, func, data)
    Evas_Object *obj
    const char *event
    SV *func
    SV *data
    PREINIT:
        _saved_callback *sc = NULL;
    CODE:
        sc = perl_save_callback_new(func, data);

        if (DEBUG) {
            fprintf(stderr, "evas_object_smart_callback_add() func:%p, data:%p, sc:%p\n", func, data, sc);
        }

        evas_object_smart_callback_add(obj, event, call_perl_sub, sc);


void *
evas_object_smart_callback_del(Evas_Object *obj, char *event, Evas_Smart_Cb func)

void
evas_object_smart_callback_call(Evas_Object *obj, char *event, void *event_info)

void
evas_object_smart_changed(Evas_Object *obj)

void
evas_object_smart_need_recalculate_set(Evas_Object *obj, Eina_Bool value)

Eina_Bool
evas_object_smart_need_recalculate_get(Evas_Object *obj)

void
evas_object_smart_calculate(Evas_Object *obj)

void
evas_smart_objects_calculate(Evas *e)

void
evas_event_freeze(Evas *e)

void
evas_event_thaw(Evas *e)

int
evas_event_freeze_get(Evas *e)

void
evas_event_feed_mouse_down(Evas *e, int b, Evas_Button_Flags flags, unsigned int timestamp, void *data)

void
evas_event_feed_mouse_up(Evas *e, int b, Evas_Button_Flags flags, unsigned int timestamp, void *data)

void
evas_event_feed_mouse_move(Evas *e, int x, int y, unsigned int timestamp, void *data)

void
evas_event_feed_mouse_in(Evas *e, unsigned int timestamp, void *data)

void
evas_event_feed_mouse_out(Evas *e, unsigned int timestamp, void *data)

void
evas_event_feed_mouse_cancel(Evas *e, unsigned int timestamp, void *data)

void
evas_event_feed_mouse_wheel(Evas *e, int direction, int z, unsigned int timestamp, void *data)

void
evas_event_feed_key_down(Evas *e, char *keyname, char *key, char *string, char *compose, unsigned int timestamp, void *data)

void
evas_event_feed_key_up(Evas *e, char *keyname, char *key, char *string, char *compose, unsigned int timestamp, void *data)

void
evas_event_feed_hold(Evas *e, int hold, unsigned int timestamp, void *data)

void
evas_object_focus_set(Evas_Object *obj, Eina_Bool focus)

Eina_Bool
evas_object_focus_get(Evas_Object *obj)

Evas_Object *
evas_focus_get(Evas *e)

const Evas_Modifier *
evas_key_modifier_get(const Evas *e)

const Evas_Lock *
evas_key_lock_get(const Evas *e)

Eina_Bool
evas_key_modifier_is_set(const Evas_Modifier *m, const char *keyname)

Eina_Bool
evas_key_lock_is_set(const Evas_Lock *l, const char *keyname)

void
evas_key_modifier_add(Evas *e, const char *keyname)

void
evas_key_modifier_del(Evas *e, const char *keyname)

void
evas_key_lock_add(Evas *e, const char *keyname)

void
evas_key_lock_del(Evas *e, const char *keyname)

void
evas_key_modifier_on(Evas *e, const char *keyname)

void
evas_key_modifier_off(Evas *e, const char *keyname)

void
evas_key_lock_on(Evas *e, const char *keyname)

void
evas_key_lock_off(Evas *e, const char *keyname)

Evas_Modifier_Mask
evas_key_modifier_mask_get(Evas *e, const char *keyname)

Eina_Bool
evas_object_key_grab(Evas_Object *obj, char *keyname, Evas_Modifier_Mask modifiers, Evas_Modifier_Mask not_modifiers, Eina_Bool exclusive)

void
evas_object_key_ungrab(Evas_Object *obj, char *keyname, Evas_Modifier_Mask modifiers, Evas_Modifier_Mask not_modifiers)

void
evas_object_pass_events_set(Evas_Object *obj, Eina_Bool pass)

Eina_Bool
evas_object_pass_events_get(Evas_Object *obj)

void
evas_object_repeat_events_set(Evas_Object *obj, Eina_Bool repeat)

Eina_Bool
evas_object_repeat_events_get(Evas_Object *obj)

void
evas_object_propagate_events_set(Evas_Object *obj, Eina_Bool prop)

Eina_Bool
evas_object_propagate_events_get(Evas_Object *obj)

void
evas_object_pointer_mode_set(Evas_Object *obj, Evas_Object_Pointer_Mode setting)

Evas_Object_Pointer_Mode evas_object_pointer_mode_get(Evas_Object *obj)

void
evas_object_precise_is_inside_set(Evas_Object *obj, Eina_Bool precise)

Eina_Bool
evas_object_precise_is_inside_get(Evas_Object *obj)

void
evas_object_event_callback_add(obj, type, func, data)
    Evas_Object *obj
    Evas_Callback_Type type
    SV *func
    SV *data
    PREINIT:
        _saved_callback *sc = NULL;
    CODE:
        sc = perl_save_callback_new(func, data);

        if (DEBUG) {
            fprintf(stderr, "evas_object_event_callback_add() func:%p, data:%p, sc:%p\n", func, data, sc);
        }

        evas_object_event_callback_add(obj, type, (void *)call_perl_sub, sc);

=pod

void *
evas_object_event_callback_del(Evas_Object *obj, Evas_Callback_Type type, void (*func) (void *data, Evas *e, Evas_Object *obj, void *event_info))

void *
evas_object_event_callback_del_full(Evas_Object *obj, Evas_Callback_Type type, void (*func) (void *data, Evas *e, Evas_Object *obj, void *event_info), void *data)

=cut

int
evas_async_events_fd_get()

int
evas_async_events_process()

=pod

Eina_Bool
evas_async_events_put(void *target, Evas_Callback_Type type, void *event_info, void (*func)(void *target, Evas_Callback_Type type, void *event_info))

=cut

=pod

void
evas_object_intercept_show_callback_add(Evas_Object *obj, void (*func) (void *data, Evas_Object *obj), void *data)

void *
evas_object_intercept_show_callback_del(Evas_Object *obj, void (*func) (void *data, Evas_Object *obj))

void
evas_object_intercept_hide_callback_add(Evas_Object *obj, void (*func) (void *data, Evas_Object *obj), void *data)

void *
evas_object_intercept_hide_callback_del(Evas_Object *obj, void (*func) (void *data, Evas_Object *obj))

void
evas_object_intercept_move_callback_add(Evas_Object *obj, void (*func) (void *data, Evas_Object *obj, Evas_Coord x, Evas_Coord y), void *data)

void *
evas_object_intercept_move_callback_del(Evas_Object *obj, void (*func) (void *data, Evas_Object *obj, Evas_Coord x, Evas_Coord y))

void
evas_object_intercept_resize_callback_add(Evas_Object *obj, void (*func) (void *data, Evas_Object *obj, Evas_Coord w, Evas_Coord h), void *data)

void *
evas_object_intercept_resize_callback_del      (Evas_Object *obj, void (*func) (void *data, Evas_Object *obj, Evas_Coord w, Evas_Coord h))

void
evas_object_intercept_raise_callback_add(Evas_Object *obj, void (*func) (void *data, Evas_Object *obj), void *data)

void *
evas_object_intercept_raise_callback_del(Evas_Object *obj, void (*func) (void *data, Evas_Object *obj))

void
evas_object_intercept_lower_callback_add(Evas_Object *obj, void (*func) (void *data, Evas_Object *obj), void *data)

void *
evas_object_intercept_lower_callback_del(Evas_Object *obj, void (*func) (void *data, Evas_Object *obj))

void
evas_object_intercept_stack_above_callback_add(Evas_Object *obj, void (*func) (void *data, Evas_Object *obj, Evas_Object *above), void *data)

void *
evas_object_intercept_stack_above_callback_del (Evas_Object *obj, void (*func) (void *data, Evas_Object *obj, Evas_Object *above))

void
evas_object_intercept_stack_below_callback_add(Evas_Object *obj, void (*func) (void *data, Evas_Object *obj, Evas_Object *below), void *data)

void *
evas_object_intercept_stack_below_callback_del (Evas_Object *obj, void (*func) (void *data, Evas_Object *obj, Evas_Object *below))

void
evas_object_intercept_layer_set_callback_add(Evas_Object *obj, void (*func) (void *data, Evas_Object *obj, int l), void *data)

void *
evas_object_intercept_layer_set_callback_del   (Evas_Object *obj, void (*func) (void *data, Evas_Object *obj, int l))

void
evas_object_intercept_color_set_callback_add(Evas_Object *obj, void (*func) (void *data, Evas_Object *obj, int r, int g, int b, int a), void *data)

void *
evas_object_intercept_color_set_callback_del   (Evas_Object *obj, void (*func) (void *data, Evas_Object *obj, int r, int g, int b, int a))

void
evas_object_intercept_clip_set_callback_add(Evas_Object *obj, void (*func) (void *data, Evas_Object *obj, Evas_Object *clip), void *data)

void *
evas_object_intercept_clip_set_callback_del    (Evas_Object *obj, void (*func) (void *data, Evas_Object *obj, Evas_Object *clip))

void
evas_object_intercept_clip_unset_callback_add(Evas_Object *obj, void (*func) (void *data, Evas_Object *obj), void *data)

void *
evas_object_intercept_clip_unset_callback_del  (Evas_Object *obj, void (*func) (void *data, Evas_Object *obj))

=cut

=pod

Eina_Bool
evas_cserve_want_get()

Eina_Bool
evas_cserve_connected_get()

Eina_Bool
evas_cserve_stats_get(Evas_Cserve_Stats *stats)

void
evas_cserve_image_cache_contents_clean(Evas_Cserve_Image_Cache *cache)

Eina_Bool
evas_cserve_config_get(Evas_Cserve_Config *config)

Eina_Bool
evas_cserve_config_set(Evas_Cserve_Config *config)

void
evas_cserve_disconnect()

=cut

void
evas_color_hsv_to_rgb(float h, float s, float v, int *r, int *g, int *b)

void
evas_color_rgb_to_hsv(int r, int g, int b, float *h, float *s, float *v)

void
evas_color_argb_premul(int a, int *r, int *g, int *b)

void
evas_color_argb_unpremul(int a, int *r, int *g, int *b)

void
evas_data_argb_premul(unsigned int *data, unsigned int len)

void
evas_data_argb_unpremul(unsigned int *data, unsigned int len)

void
evas_transform_identity_set(Evas_Transform *t)

void
evas_transform_rotate(double angle, Evas_Transform *t)

void
evas_transform_translate(float dx, float dy, Evas_Transform *t)

void
evas_transform_scale(float sx, float sy, Evas_Transform *t)

void
evas_transform_shear(float sh, float sv, Evas_Transform *t)

void
evas_transform_compose(Evas_Transform *l, Evas_Transform *t)

Evas_Imaging_Image *
evas_imaging_image_load(char *file, char *key)

void
evas_imaging_image_free(Evas_Imaging_Image *im)

void
evas_imaging_image_size_get(Evas_Imaging_Image *im, int *w, int *h)

Eina_Bool
evas_imaging_image_alpha_get(Evas_Imaging_Image *im)

void
evas_imaging_image_cache_set(int bytes)

int
evas_imaging_image_cache_get()

void
evas_imaging_font_hinting_set(Evas_Font_Hinting_Flags hinting)

Evas_Font_Hinting_Flags
evas_imaging_font_hinting_get()

Eina_Bool
evas_imaging_font_hinting_can_hint(Evas_Font_Hinting_Flags hinting)

Evas_Imaging_Font *
evas_imaging_font_load(char *file, char *key, int size)

void
evas_imaging_font_free(Evas_Imaging_Font *fn)

int
evas_imaging_font_ascent_get(Evas_Imaging_Font *fn)

int
evas_imaging_font_descent_get(Evas_Imaging_Font *fn)

int
evas_imaging_font_max_ascent_get(Evas_Imaging_Font *fn)

int
evas_imaging_font_max_descent_get(Evas_Imaging_Font *fn)

int
evas_imaging_font_line_advance_get(Evas_Imaging_Font *fn)

void
evas_imaging_font_string_advance_get(Evas_Imaging_Font *fn, char *str, int *x, int *y)

void
evas_imaging_font_string_size_query(Evas_Imaging_Font *fn, char *str, int *w, int *h)

int
evas_imaging_font_string_inset_get(Evas_Imaging_Font *fn, char *str)

int
evas_imaging_font_string_char_coords_get(Evas_Imaging_Font *fn, char *str, int pos, int *cx, int *cy, int *cw, int *ch)

int
evas_imaging_font_string_char_at_coords_get(Evas_Imaging_Font *fn, char *str, int x, int y, int *cx, int *cy, int *cw, int *ch)

void
evas_imaging_font_cache_set(int bytes)

int
evas_imaging_font_cache_get()

const char *
evas_load_error_str(int error)

Evas_Object *
evas_object_smart_clipped_clipper_get(Evas_Object *obj)

void
evas_object_smart_clipped_smart_set(Evas_Smart_Class *sc)

void
evas_object_smart_move_children_relative(Evas_Object *obj, Evas_Coord dx, Evas_Coord dy)

void
evas_object_box_smart_set(Evas_Object_Box_Api *api)

=pod

void
evas_object_box_layout_set(Evas_Object *o, Evas_Object_Box_Layout cb, void *data, void (*free_data)(void *data))

=cut

Evas_Object *
evas_object_box_add(Evas *evas)

Evas_Object *
evas_object_box_add_to(Evas_Object *parent)

void
evas_object_box_layout_horizontal(Evas_Object *o, Evas_Object_Box_Data *priv, void *data)

void
evas_object_box_layout_vertical(Evas_Object *o, Evas_Object_Box_Data *priv, void *data)

void
evas_object_box_layout_homogeneous_vertical(Evas_Object *o, Evas_Object_Box_Data *priv, void *data)

void
evas_object_box_layout_homogeneous_horizontal(Evas_Object *o, Evas_Object_Box_Data *priv, void *data)

void
evas_object_box_layout_homogeneous_max_size_horizontal(Evas_Object *o, Evas_Object_Box_Data *priv, void *data)

void
evas_object_box_layout_homogeneous_max_size_vertical(Evas_Object *o, Evas_Object_Box_Data *priv, void *data)

void
evas_object_box_layout_flow_horizontal(Evas_Object *o, Evas_Object_Box_Data *priv, void *data)

void
evas_object_box_layout_flow_vertical(Evas_Object *o, Evas_Object_Box_Data *priv, void *data)

void
evas_object_box_layout_stack(Evas_Object *o, Evas_Object_Box_Data *priv, void *data)

void
evas_object_box_align_set(Evas_Object *o, double horizontal, double vertical)

void
evas_object_box_align_get(Evas_Object *o, double *horizontal, double *vertical)

void
evas_object_box_padding_set(Evas_Object *o, Evas_Coord horizontal, Evas_Coord vertical)

void
evas_object_box_padding_get(Evas_Object *o, Evas_Coord *horizontal, Evas_Coord *vertical)

Evas_Object_Box_Option *
evas_object_box_append(Evas_Object *o, Evas_Object *child)

Evas_Object_Box_Option *
evas_object_box_prepend(Evas_Object *o, Evas_Object *child)

Evas_Object_Box_Option *
evas_object_box_insert_before(Evas_Object *o, Evas_Object *child, Evas_Object *reference)

Evas_Object_Box_Option *
evas_object_box_insert_after(Evas_Object *o, Evas_Object *child, Evas_Object *referente)

Evas_Object_Box_Option *
evas_object_box_insert_at(Evas_Object *o, Evas_Object *child, unsigned int pos)

Eina_Bool
evas_object_box_remove(Evas_Object *o, Evas_Object *child)

Eina_Bool
evas_object_box_remove_at(Evas_Object *o, unsigned int pos)

Eina_Bool
evas_object_box_remove_all(Evas_Object *o, Eina_Bool clear)

Eina_Iterator *
evas_object_box_iterator_new(Evas_Object *o)

Eina_Accessor *
evas_object_box_accessor_new(Evas_Object *o)

Eina_List *
evas_object_box_children_get(Evas_Object *o)

const char *
evas_object_box_option_property_name_get(Evas_Object *o, int property)

int
evas_object_box_option_property_id_get(Evas_Object *o, char *name)

Eina_Bool
evas_object_box_option_property_set(Evas_Object *o, Evas_Object_Box_Option *opt, int property, ...)

=pod

Eina_Bool
evas_object_box_option_property_vset(Evas_Object *o, Evas_Object_Box_Option *opt, int property, va_list args)

Eina_Bool
evas_object_box_option_property_get(Evas_Object *o, Evas_Object_Box_Option *opt, int property, ...)

Eina_Bool
evas_object_box_option_property_vget(Evas_Object *o, Evas_Object_Box_Option *opt, int property, va_list args)

=cut

Evas_Object *
evas_object_table_add(Evas *evas)

Evas_Object *
evas_object_table_add_to(Evas_Object *parent)

void
evas_object_table_homogeneous_set(Evas_Object *o, Evas_Object_Table_Homogeneous_Mode homogeneous)

Evas_Object_Table_Homogeneous_Mode
evas_object_table_homogeneous_get(Evas_Object *o)

void
evas_object_table_padding_set(Evas_Object *o, Evas_Coord horizontal, Evas_Coord vertical)

void
evas_object_table_padding_get(Evas_Object *o, Evas_Coord *horizontal, Evas_Coord *vertical)

void
evas_object_table_align_set(Evas_Object *o, double horizontal, double vertical)

void
evas_object_table_align_get(Evas_Object *o, double *horizontal, double *vertical)

Eina_Bool
evas_object_table_pack(Evas_Object *o, Evas_Object *child, unsigned short col, unsigned short row, unsigned short colspan, unsigned short rowspan)

Eina_Bool
evas_object_table_unpack(Evas_Object *o, Evas_Object *child)

void
evas_object_table_clear(Evas_Object *o, Eina_Bool clear)

void
evas_object_table_col_row_size_get(Evas_Object *o, int *cols, int *rows)

Eina_Iterator *
evas_object_table_iterator_new(Evas_Object *o)

Eina_Accessor *
evas_object_table_accessor_new(Evas_Object *o)

Eina_List *
evas_object_table_children_get(Evas_Object *o)
