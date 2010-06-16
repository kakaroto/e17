/******************************************************************************
**  Copyright (c) 2006-2010, Calaos. All Rights Reserved.
**
**  This is free software; you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation; either version 3 of the License, or
**  (at your option) any later version.
**
**  This is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this; if not, write to the Free Software
**  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
**
******************************************************************************/

#include "draw_object.h"
#include "physic_object.h"

//list of callbacks
static Eina_List *callback_list = NULL;

typedef struct _CallbackDrawType
{
        void (*func) (Evas_Object *new_obj, Eina_List *point_list, void *data);
        void *data;
} CallbackDrawType;

//handle evas mouse events
static Evas_Object *mouse_catcher = NULL;
static Eina_Bool is_clicked = EINA_FALSE;

static Eina_Bool is_enabled = EINA_TRUE;

//clipper to change the brush color
static Evas_Object *brush_color_clip = NULL;

static Eina_List *current_path_list = NULL;

static Evas *evas = NULL;

//Buffer evas for new object creation
static Ecore_Evas *ee_buffer = NULL;
static Evas_Object *image_buffer = NULL;

//private functions
static void _add_point(int x, int y);
static void _draw_line(int x1, int y1, int x2, int y2);
static void _mouse_down_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info);
static void _mouse_up_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info);
static void _mouse_move_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info);
static void _find_bouding_box(Eina_List *point_list, int brush_size, int *box_x, int *box_y, int *box_w, int *box_h);

BrushColor default_colors[] =
{
        BRUSH_RED,
        BRUSH_BLUE,
        BRUSH_GREEN,
        BRUSH_YELLOW,
        BRUSH_PINK,
        BRUSH_CYAN,
        BRUSH_ORANGE,
        BRUSH_VIOLET
};

/**
 * @brief Add a new mouse event catcher and init all functions for drawing on the canvas
 * @note Can be added only once.
 */
void draw_object_add(Evas *_evas)
{
        if (mouse_catcher)
        {
                WARN("already added to canvas !");

                return;
        }

        evas = _evas;

        Ecore_Evas *ee = ecore_evas_ecore_evas_get(evas);

        int bg_width = 0, bg_height = 0;
        ecore_evas_geometry_get(ee, NULL, NULL, &bg_width, &bg_height);

        mouse_catcher = evas_object_rectangle_add(evas);
        evas_object_move(mouse_catcher, 0, 0);
        evas_object_resize(mouse_catcher, bg_width, bg_height);
        evas_object_layer_set(mouse_catcher, 1000);
        evas_object_repeat_events_set(mouse_catcher, EINA_TRUE);
        evas_object_color_set(mouse_catcher, 0, 0, 0, 0);
        evas_object_show(mouse_catcher);

        brush_color_clip = evas_object_rectangle_add(evas);
        evas_object_move(brush_color_clip, 0, 0);
        evas_object_resize(brush_color_clip, bg_width, bg_height);
        evas_object_layer_set(brush_color_clip, 999);
        evas_object_pass_events_set(brush_color_clip, EINA_TRUE);
        evas_object_color_set(brush_color_clip, 0, 0, 0, 255);

        evas_object_event_callback_add(mouse_catcher, EVAS_CALLBACK_MOUSE_DOWN, _mouse_down_cb, NULL);
        evas_object_event_callback_add(mouse_catcher, EVAS_CALLBACK_MOUSE_UP, _mouse_up_cb, NULL);
        evas_object_event_callback_add(mouse_catcher, EVAS_CALLBACK_MOUSE_MOVE, _mouse_move_cb, NULL);

        INF("Added drawing layer");
}

/**
 * @brief Enable/Disable drawing
 * @param en true if enabled
 */
void draw_object_enabled_set(Eina_Bool en)
{
        is_enabled = en;
}

/**
 * @brief Get enabled state
 * @return true if enabled
 */
Eina_Bool draw_object_enabled_get(void)
{
        return is_enabled;
}

/**
 * @brief Change color of the brush
 */
void draw_object_color_set(int r, int g, int b, int a)
{
        evas_object_color_set(brush_color_clip, r, g, b, a);
}

/**
 * @brief Change color of the brush
 * @note same as draw_object_color_set but takes a BrushColor as parameter
 */
void draw_object_brush_set(BrushColor brush)
{
        draw_object_color_set(brush.r, brush.g, brush.b, brush.a);
}

/**
 * @brief Resize the drawing rect
 */
void draw_object_resize(int w, int h)
{
        if (mouse_catcher)
        {
                evas_object_resize(mouse_catcher, w, h);
                evas_object_resize(brush_color_clip, w, h);
        }
}

/**
 * @brief Add a callback called when a new object is drawn
 */
void draw_object_new_callback_add(void (*func) (Evas_Object *new_obj, Eina_List *point_list, void *data), void *data)
{
        CallbackDrawType *cb = calloc(1, sizeof(CallbackDrawType));

        cb->func = func;
        cb->data = data;

        callback_list = eina_list_append(callback_list, cb);
}

/**
 * @brief Delete a callback
 */
void draw_object_new_callback_del(void (*func) (Evas_Object *new_obj, Eina_List *point_list, void *data), void *data)
{
        Eina_List *l, *lnext;
        CallbackDrawType *cb;
        EINA_LIST_FOREACH_SAFE(callback_list, l, lnext, cb)
        {
                if (cb->func == func && cb->data == data)
                {
                        free(cb);
                        callback_list = eina_list_remove_list(callback_list, l);
                }
        }
}

/**
 * @brief Remove the drawing system from the canvas
 */
void draw_object_del(void)
{
        CallbackDrawType *cb;
        EINA_LIST_FREE(callback_list, cb);
                free(cb);

        evas_object_del(mouse_catcher);
        mouse_catcher = NULL;
        is_clicked = EINA_FALSE;
}

/**
 * @brief Create an Evas_Object from a list of point
 * @param e the evas canvas
 * @param point_list list of points to draw
 * @param color brush color to draw with
 * @return The corresponding Evas_Object
 */
Evas_Object *draw_object_create(Evas *e, Eina_List *point_list, BrushColor color)
{
        if (!point_list)
        {
                ERR("point_list is NULL !");
                return NULL;
        }

        int bg_width = 0, bg_height = 0;
        ecore_evas_geometry_get(ecore_evas_ecore_evas_get(e), NULL, NULL, &bg_width, &bg_height);

        int ox, oy;
        Eina_List *l = NULL;
        DrawPoint *first_obj = (DrawPoint *)eina_list_data_get(point_list);

        //Get brush size...
        Evas_Object *tmp_brush;
        tmp_brush = evas_object_image_add(e);
        evas_object_image_file_set(tmp_brush, PACKAGE_DATA_DIR "/brushes/brush01.png", NULL);
        int iw, ih;
        evas_object_image_size_get(tmp_brush, &iw, &ih);
        evas_object_del(tmp_brush);

        ox = first_obj->x - iw / 2;
        oy = first_obj->y - ih / 2;
        oy = bg_height - oy;

        //find bounding box
        int bx, by, bw, bh;
        _find_bouding_box(point_list, iw / 2, &bx, &by, &bw, &bh);

        ee_buffer = ecore_evas_buffer_new(bw, bh);
        image_buffer = ecore_evas_object_image_new(ee_buffer);

        evas_object_move(image_buffer, 0, 0);
        evas_object_resize(image_buffer, bw, bh);
        evas_object_image_fill_set(image_buffer, 0, 0, bw, bh);
        evas_object_image_size_set(image_buffer, bw, bh);
        evas_object_show(image_buffer);

        Ecore_Evas *ee_im = (Ecore_Evas *)evas_object_data_get(image_buffer, "Ecore_Evas");
        Evas *evas_im = ecore_evas_get(ee_im);

        //create clipper
        Evas_Object *color_clip = evas_object_rectangle_add(evas_im);
        evas_object_move(color_clip, 0, 0);
        evas_object_resize(color_clip, bw, bh);
        evas_object_color_set(color_clip, color.r, color.g, color.b, color.a);
        evas_object_show(color_clip);

        DrawPoint *point;
        EINA_LIST_FOREACH(point_list, l, point)
        {
                int ox2, oy2;

                ox2 = point->x - iw / 2;
                oy2 = point->y - ih / 2;

                //Draw on a buffer
                char file[128];
                sprintf(file, PACKAGE_DATA_DIR "/brushes/brush0%d.png", (rand() % 4) + 1);

                Evas_Object *image = evas_object_image_add(evas_im);
                evas_object_image_file_set(image, file, NULL);

                evas_object_image_fill_set(image, 0, 0, iw, ih);
                evas_object_move(image, ox2 - bx, oy2 - by);
                evas_object_resize(image, iw, ih);
                evas_object_clip_set(image, color_clip);
                evas_object_show(image);
        }

        //ecore_evas_buffer_pixels_get(ee_buffer);
        //evas_object_image_save(image_buffer, "tmp.png", NULL, NULL);

        //it's time to save our buffer image to a new one on the current Evas surface
        Evas_Object *image = evas_object_image_add(e);

        evas_object_move(image, bx, by);
        evas_object_image_fill_set(image, 0, 0, bw, bh);
        evas_object_image_size_set(image, bw, bh);
        evas_object_resize(image, bw, bh);
        evas_object_image_alpha_set(image, 1);
        evas_object_image_data_copy_set(image, (void *)ecore_evas_buffer_pixels_get(ee_buffer));
        evas_object_show(image);

        evas_object_del(image_buffer);
        ecore_evas_free(ee_buffer);

        INF("New object added to Evas");

        return image;
}

/* ------- Private here ------- */

//for drawing detection
static int last_pointx = -999, last_pointy = -999;
static int last_draw_pointx = -999, last_draw_pointy = -999;

//bounding box for the new drawing
int bbox_x, bbox_y, bbox_w, bbox_h;

static void _find_bouding_box(Eina_List *point_list, int brush_size, int *box_x, int *box_y, int *box_w, int *box_h)
{
        int bx = -999, by = -999, bw = 0, bh = 0;

        Eina_List *l;
        DrawPoint *point;
        EINA_LIST_FOREACH(point_list, l, point)
        {
                int x = point->x - brush_size;
                int y = point->y - brush_size;

                if (bx == -999) bx = x;
                if (by == -999) by = y;

                if (x < bx) { bw += bx - x; bx = x; }
                if (y < by) { bh += by - y; by = y; }
                if (x + brush_size > bx + bw) bw = x + brush_size - bx + 2 * brush_size;
                if (y + brush_size > by + bh) bh = y + brush_size - by + 2 * brush_size;
        }

        if (box_x) *box_x = bx;
        if (box_y) *box_y = by;
        if (box_w) *box_w = bw;
        if (box_h) *box_h = bh;
}

static void _add_point(int x, int y)
{
        //Only draw add a brush if the last one was far enough
        if (last_draw_pointx != -999 && last_draw_pointy != -999 &&
            abs(x - last_draw_pointx) <= 3 &&
            abs(y - last_draw_pointy) <= 3)
        {
                return;
        }

        last_draw_pointx = x;
        last_draw_pointy = y;

        Evas_Object *point = evas_object_image_add(evas);
        evas_object_move(point, x, y);

        char file[128];
        sprintf(file, PACKAGE_DATA_DIR "/brushes/brush0%d.png", (rand() % 4) + 1);
        evas_object_image_file_set(point, file, NULL);

        int iw, ih;
        evas_object_image_size_get(point, &iw, &ih);

        evas_object_resize(point, iw, ih);
        evas_object_image_fill_set(point, 0, 0, iw, ih);

        evas_object_show(point);

        //clip brush to change color
        evas_object_clip_set(point, brush_color_clip);
        evas_object_show(brush_color_clip);

        current_path_list = eina_list_append(current_path_list, point);

        if (x < bbox_x) { bbox_w += bbox_x - x; bbox_x = x; }
        if (y < bbox_y) { bbox_h += bbox_y - y; bbox_y = y; }
        if (x + iw > bbox_x + bbox_w) bbox_w = x + iw - bbox_x;
        if (y + ih > bbox_y + bbox_h) bbox_h = y + ih - bbox_y;
}

//simple bresenham algo
void _draw_line(int x1, int y1, int x2, int y2)
{
        int d, dx, dy, aincr, bincr, xincr, yincr, x, y;

        if (abs(x2 - x1) < abs(y2 - y1))
        {
                if (y1 > y2)
                {
                        swap_int(x1, x2);
                        swap_int(y1, y2);
                }

                xincr = x2 > x1 ? 1 : -1;
                dy = y2 - y1;
                dx = abs(x2 - x1);
                d = 2 * dx - dy;
                aincr = 2 * (dx - dy);
                bincr = 2 * dx;
                x = x1;
                y = y1;

                _add_point(x, y);

                for (y = y1+1; y <= y2; ++y)
                {
                        if (d >= 0)
                        {
                                x += xincr;
                                d += aincr;
                        }
                        else
                                d += bincr;

                        _add_point(x, y);
                }

        }
        else
        {
                if (x1 > x2)
                {
                        swap_int(x1, x2);
                        swap_int(y1, y2);
                }

                yincr = y2 > y1 ? 1 : -1;
                dx = x2 - x1;
                dy = abs(y2 - y1);
                d = 2 * dy - dx;
                aincr = 2 * (dy - dx);
                bincr = 2 * dy;
                x = x1;
                y = y1;

                _add_point(x, y);

                for (x = x1+1; x <= x2; ++x)
                {
                        if (d >= 0)
                        {
                                y += yincr;
                                d += aincr;
                        }
                        else
                                d += bincr;

                        _add_point(x, y);
                }
        }
}

static void _mouse_down_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
        if (!is_enabled) return;

        Evas_Event_Mouse_Down *ev = (Evas_Event_Mouse_Down *)event_info;

        bbox_x = ev->canvas.x;
        bbox_y = ev->canvas.y;
        bbox_w = bbox_h = 0;

        is_clicked = EINA_TRUE;

        last_pointx = -999;
        last_pointy = -999;
        last_draw_pointx = -999;
        last_draw_pointy = -999;
}

static void _mouse_move_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
        if (!is_enabled) return;

        Evas_Event_Mouse_Move *ev = (Evas_Event_Mouse_Move *)event_info;

        int bg_width = 0, bg_height = 0;
        ecore_evas_geometry_get(ecore_evas_ecore_evas_get(evas), NULL, NULL, &bg_width, &bg_height);

        if (is_clicked == EINA_TRUE &&
            ev->cur.canvas.x >= 0 && ev->cur.canvas.x <= bg_width &&
            ev->cur.canvas.y >= 0 && ev->cur.canvas.y <= bg_height)
        {
                if (last_pointx == -999) last_pointx = ev->cur.canvas.x;
                if (last_pointy == -999) last_pointy = ev->cur.canvas.y;

                // Only draw a new "line" if new point is lower than the brush's size.
                // It uses less points, and less physics shapes
                if (abs(ev->cur.canvas.x - last_pointx) >= 7 ||
                    abs(ev->cur.canvas.y - last_pointy) >= 7)
                {
                        _draw_line(last_pointx, last_pointy, ev->cur.canvas.x, ev->cur.canvas.y);

                        last_pointx = ev->cur.canvas.x;
                        last_pointy = ev->cur.canvas.y;
                }
        }
}

static void _mouse_up_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
        if (!is_enabled) return;

        //Evas_Event_Mouse_Up *ev = (Evas_Event_Mouse_Up *)event_info;
        is_clicked = EINA_FALSE;

        int bg_width = 0, bg_height = 0;
        ecore_evas_geometry_get(ecore_evas_ecore_evas_get(evas), NULL, NULL, &bg_width, &bg_height);

        if (current_path_list)
        {
                int ox, oy;
                Eina_List *l, *path_list = NULL;
                Evas_Object *first_obj = (Evas_Object *)eina_list_data_get(current_path_list);

                evas_object_geometry_get(first_obj, &ox, &oy, NULL, NULL);
                oy = bg_height - oy;

                ee_buffer = ecore_evas_buffer_new(bbox_w, bbox_h);
                image_buffer = ecore_evas_object_image_new(ee_buffer);

                evas_object_move(image_buffer, 0, 0);
                evas_object_resize(image_buffer, bbox_w, bbox_h);
                evas_object_image_fill_set(image_buffer, 0, 0, bbox_w, bbox_h);
                evas_object_image_size_set(image_buffer, bbox_w, bbox_h);
                evas_object_show(image_buffer);

                Ecore_Evas *ee_im = (Ecore_Evas *)evas_object_data_get(image_buffer, "Ecore_Evas");
                Evas *evas_im = ecore_evas_get(ee_im);

                //create clipper
                Evas_Object *color_clip = evas_object_rectangle_add(evas_im);
                evas_object_move(color_clip, 0, 0);
                evas_object_resize(color_clip, bbox_w, bbox_h);
                int r, g, b, a;
                evas_object_color_get(brush_color_clip, &r, &g, &b, &a);
                evas_object_color_set(color_clip, r, g, b, a);
                evas_object_show(color_clip);

                Evas_Object *obj;
                //EINA_LIST_FOREACH(current_path_list, l, obj)
                EINA_LIST_FREE(current_path_list, obj)
                {
                        int ox2, oy2;

                        evas_object_geometry_get(obj, &ox2, &oy2, NULL, NULL);

                        evas_object_del(obj);

                        //Draw on a buffer
                        char file[128];
                        sprintf(file, PACKAGE_DATA_DIR "/brushes/brush0%d.png", (rand() % 4) + 1);

                        Evas_Object *image = evas_object_image_add(evas_im);
                        evas_object_image_file_set(image, file, NULL);

                        int iw, ih;
                        evas_object_image_size_get(image, &iw, &ih);

                        evas_object_image_fill_set(image, 0, 0, iw, ih);
                        evas_object_move(image, ox2 - bbox_x, oy2 - bbox_y);
                        evas_object_resize(image, iw, ih);
                        evas_object_clip_set(image, color_clip);
                        evas_object_show(image);

                        DrawPoint *p = calloc(1, sizeof(DrawPoint));
                        p->x = ox2 + iw / 2;
                        p->y = oy2 + ih / 2;
                        path_list = eina_list_append(path_list, p);
                }

                //ecore_evas_buffer_pixels_get(ee_buffer);
                //evas_object_image_save(image_buffer, "tmp.png", NULL, NULL);

                //it's time to save our buffer image to a new one on the current Evas surface
                Evas_Object *image = evas_object_image_add(evas);

                evas_object_move(image, bbox_x, bbox_y);
                evas_object_image_fill_set(image, 0, 0, bbox_w, bbox_h);
                evas_object_image_size_set(image, bbox_w, bbox_h);
                evas_object_resize(image, bbox_w, bbox_h);
                evas_object_image_alpha_set(image, 1);
                evas_object_image_data_copy_set(image, (void *)ecore_evas_buffer_pixels_get(ee_buffer));
                evas_object_show(image);

                evas_object_del(image_buffer);
                ecore_evas_free(ee_buffer);

                //hide clipper
                evas_object_hide(brush_color_clip);

                eina_list_free(current_path_list);
                current_path_list = NULL;

                INF("New object added to Evas");

                EINA_LIST_FOREACH(callback_list, l, data)
                {
                        CallbackDrawType *cb = (CallbackDrawType *)data;

                        if (cb->func)
                                cb->func(image, path_list, cb->data);
                }

                //free memory after all callbacks are called
                DrawPoint *p;
                EINA_LIST_FREE(path_list, p)
                        free(p);
        }
}
