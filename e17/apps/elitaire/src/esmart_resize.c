/* vim: set sw=4 ts=4 sts=4 expandtab: */
/**************************************************************************
 * draggies.c : a rectangle to add to your borderless/shaped ecore_evas app
 * that will handle moving the ecore evas, as if it were a border.
 ***************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include "Esmart_Resize.h"


static Evas_Smart *_esmart_resize_object_smart_get();
static Evas_Object *esmart_resize_object_new(Evas * evas);
static void _esmart_resize_object_add(Evas_Object * o);
static void _esmart_resize_object_del(Evas_Object * o);
static void _esmart_resize_object_layer_set(Evas_Object * o, int l);
static void _esmart_resize_object_raise(Evas_Object * o);
static void _esmart_resize_object_lower(Evas_Object * o);
static void _esmart_resize_object_stack_above(Evas_Object * o,
                                              Evas_Object * above);
static void _esmart_resize_object_stack_below(Evas_Object * o,
                                              Evas_Object * below);
static void _esmart_resize_object_move(Evas_Object * o, Evas_Coord x,
                                       Evas_Coord y);
static void _esmart_resize_object_resize(Evas_Object * o, Evas_Coord w,
                                         Evas_Coord h);
static void _esmart_resize_object_show(Evas_Object * o);
static void _esmart_resize_object_hide(Evas_Object * o);
static void _esmart_resize_object_color_set(Evas_Object * o, int r, int g,
                                            int b, int a);
static void _esmart_resize_object_clip_set(Evas_Object * o,
                                           Evas_Object * clip);
static void _esmart_resize_object_clip_unset(Evas_Object * o);

/*==========================================================================
 * The actual code ethat handles the moving of the window
 *========================================================================*/
static void _mouse_up_cb(void *data, Evas * evas, Evas_Object * obj, void *ev)
{
    Esmart_Resize *drag = NULL;
    Evas_Event_Mouse_Up *e = NULL;

    if ((drag = (Esmart_Resize *) data)) {
        if ((e = (Evas_Event_Mouse_Up *) ev)) {
            if (e->button == drag->button) {
                drag->clicked = 0;
                drag->dx = drag->dy = 0;

            }
        }
    }
}
static void
_mouse_down_cb(void *data, Evas * evas, Evas_Object * obj, void *ev)
{
    Esmart_Resize *drag = NULL;
    Evas_Event_Mouse_Down *e = NULL;
    Ecore_X_Event_Mouse_Button_Down *evx = NULL;

    if ((drag = (Esmart_Resize *) data)) {
        if (ecore_event_current_type_get() == ECORE_X_EVENT_MOUSE_BUTTON_DOWN) {
            if ((evx =
                 (Ecore_X_Event_Mouse_Button_Down *)
                 ecore_event_current_event_get())) {
                if ((e = (Evas_Event_Mouse_Down *) ev)) {
                    if (e->button == drag->button) {
                        ecore_evas_geometry_get(drag->ee, &(drag->x),
                                                &(drag->y), &(drag->w),
                                                &(drag->h));
                        drag->clicked = 1;
                        drag->dx = evx->root.x;
                        drag->dy = evx->root.y;
                    }
                }
            }
        }
    }
}
static void
_mouse_move_cb(void *data, Evas * evas, Evas_Object * obj, void *ev)
{
    Esmart_Resize *drag = NULL;
    Ecore_X_Event_Mouse_Move *evx = NULL;

    if ((drag = (Esmart_Resize *) data)) {
        if (ecore_event_current_type_get() == ECORE_X_EVENT_MOUSE_MOVE) {
            if ((evx =
                 (Ecore_X_Event_Mouse_Move *)
                 ecore_event_current_event_get())) {
                if (drag->clicked) {

                    int h, w, x, y;
                    int minw, minh, maxw, maxh;

                    w = drag->w + drag->resize_x * (evx->root.x - drag->dx);
                    h = drag->h + drag->resize_y * (evx->root.y - drag->dy);

                    ecore_evas_size_min_get(drag->ee, &minw, &minh);
                    ecore_evas_size_max_get(drag->ee, &maxw, &maxh);

                    w = w < minw ? minw : w;
                    h = h < minh ? minh : h;
                    w = w > maxw ? maxw : w;
                    h = h > maxh ? maxh : h;

                    if (drag->resize_x < 0)
                        x = drag->x - w + drag->w;
                    else
                        x = drag->x;
                    if (drag->resize_y < 0)
                        y = drag->y - h + drag->h;
                    else
                        y = drag->y;

                    ecore_evas_move_resize(drag->ee, x, y, w, h);

                }
            }
        }
    }
}

/*==========================================================================
 * The three external functions, new,  button set and type set
 *========================================================================*/
Evas_Object *esmart_resize_new(Ecore_Evas * ee)
{
    Evas_Object *o = NULL;
    Evas_Object *result = NULL;
    Esmart_Resize *data = NULL;

    if (!ee)
        return (NULL);
    if ((result = esmart_resize_object_new(ecore_evas_get(ee)))) {
        if ((data = (Esmart_Resize *) evas_object_smart_data_get(result))) {
            data->ee = ee;
            o = evas_object_rectangle_add(ecore_evas_get(ee));
            evas_object_color_set(o, 255, 255, 255, 0);
            evas_object_repeat_events_set(o, 1);
            evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN,
                                           _mouse_down_cb, data);
            evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_UP,
                                           _mouse_up_cb, data);
            evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_MOVE,
                                           _mouse_move_cb, data);
            data->obj = o;
        }
    }
    return (result);
}

void esmart_resize_button_set(Evas_Object * o, int button)
{
    Esmart_Resize *data = NULL;

    if ((data = (Esmart_Resize *) evas_object_smart_data_get(o))) {
        data->button = button;
    }
}

void esmart_resize_type_set(Evas_Object * o, Esmart_Resize_Type type)
{
    Esmart_Resize *data = NULL;

    if ((data = (Esmart_Resize *) evas_object_smart_data_get(o))) {
        switch (type) {
        case ESMART_RESIZE_LEFT:
        case ESMART_RESIZE_LEFT_BOTTOM:
        case ESMART_RESIZE_LEFT_TOP:
            data->resize_x = -1;
            break;
        case ESMART_RESIZE_TOP:
        case ESMART_RESIZE_BOTTOM:
            data->resize_x = 0;
            break;
        default:
            data->resize_x = 1;
        }
        switch (type) {
        case ESMART_RESIZE_TOP:
        case ESMART_RESIZE_LEFT_TOP:
        case ESMART_RESIZE_RIGHT_TOP:
            data->resize_y = -1;
            break;
        case ESMART_RESIZE_LEFT:
        case ESMART_RESIZE_RIGHT:
            data->resize_y = 0;
            break;
        default:
            data->resize_y = 1;
        }
    }
}

void
esmart_resize_event_callback_add(Evas_Object * o, Evas_Callback_Type type,
                                 void (*func) (void *data, Evas * e,
                                               Evas_Object * obj,
                                               void *ev),
                                 const void *user_data)
{
    Esmart_Resize *data = NULL;

    if ((data = (Esmart_Resize *) evas_object_smart_data_get(o))) {
        evas_object_event_callback_add(data->obj, type, func, user_data);
    }
}

/*==========================================================================
 * Smart Object Code, Go Away
 *========================================================================*/

static Evas_Object *esmart_resize_object_new(Evas * evas)
{
    Evas_Object *esmart_resize_object;

    esmart_resize_object =
        evas_object_smart_add(evas, _esmart_resize_object_smart_get());

    return esmart_resize_object;
}

static Evas_Smart *_esmart_resize_object_smart_get()
{
    static Evas_Smart *smart = NULL;

    if (smart)
        return smart;

    smart = evas_smart_new("esmart_resize_object",
                           _esmart_resize_object_add,
                           _esmart_resize_object_del,
                           _esmart_resize_object_layer_set,
                           _esmart_resize_object_raise,
                           _esmart_resize_object_lower,
                           _esmart_resize_object_stack_above,
                           _esmart_resize_object_stack_below,
                           _esmart_resize_object_move,
                           _esmart_resize_object_resize,
                           _esmart_resize_object_show,
                           _esmart_resize_object_hide,
                           _esmart_resize_object_color_set,
                           _esmart_resize_object_clip_set,
                           _esmart_resize_object_clip_unset, NULL);

    return smart;
}

static void _esmart_resize_object_add(Evas_Object * o)
{
    Esmart_Resize *data = NULL;

    data = (Esmart_Resize *) malloc(sizeof(Esmart_Resize));
    memset(data, 0, sizeof(Esmart_Resize));
    evas_object_smart_data_set(o, data);
}

static void _esmart_resize_object_del(Evas_Object * o)
{
    Esmart_Resize *data;

    if ((data = (Esmart_Resize *) evas_object_smart_data_get(o))) {
        data->ee = NULL;
        if (data->obj)
            evas_object_del(data->obj);
        free(data);
    }
}

static void _esmart_resize_object_layer_set(Evas_Object * o, int l)
{
    Esmart_Resize *data;

    if ((data = (Esmart_Resize *) evas_object_smart_data_get(o))) {
        evas_object_layer_set(data->obj, l);
    }
}

static void _esmart_resize_object_raise(Evas_Object * o)
{
    Esmart_Resize *data;

    if ((data = (Esmart_Resize *) evas_object_smart_data_get(o))) {
        evas_object_raise(data->obj);
    }
}

static void _esmart_resize_object_lower(Evas_Object * o)
{
    Esmart_Resize *data;

    if ((data = (Esmart_Resize *) evas_object_smart_data_get(o))) {
        evas_object_lower(data->obj);
    }
}

static void
_esmart_resize_object_stack_above(Evas_Object * o, Evas_Object * above)
{
    Esmart_Resize *data;

    if ((data = (Esmart_Resize *) evas_object_smart_data_get(o))) {
        evas_object_stack_above(data->obj, above);
    }
}

static void
_esmart_resize_object_stack_below(Evas_Object * o, Evas_Object * below)
{
    Esmart_Resize *data;

    if ((data = (Esmart_Resize *) evas_object_smart_data_get(o))) {
        evas_object_stack_below(data->obj, below);
    }
}

static void
_esmart_resize_object_move(Evas_Object * o, Evas_Coord x, Evas_Coord y)
{
    Esmart_Resize *data;

    if ((data = (Esmart_Resize *) evas_object_smart_data_get(o))) {
        evas_object_move(data->obj, x, y);
    }
}

static void
_esmart_resize_object_resize(Evas_Object * o, Evas_Coord w, Evas_Coord h)
{
    Esmart_Resize *data;

    if ((data = (Esmart_Resize *) evas_object_smart_data_get(o))) {
        evas_object_resize(data->obj, w, h);
    }
}

static void _esmart_resize_object_show(Evas_Object * o)
{
    Esmart_Resize *data;

    if ((data = (Esmart_Resize *) evas_object_smart_data_get(o))) {
        evas_object_show(data->obj);
    }
}

static void _esmart_resize_object_hide(Evas_Object * o)
{
    Esmart_Resize *data;

    if ((data = (Esmart_Resize *) evas_object_smart_data_get(o))) {
        evas_object_hide(data->obj);
    }
}

static void
_esmart_resize_object_color_set(Evas_Object * o, int r, int g, int b, int a)
{
    Esmart_Resize *data;

    if ((data = (Esmart_Resize *) evas_object_smart_data_get(o))) {
        evas_object_color_set(data->obj, r, g, b, a);
    }
}

static void
_esmart_resize_object_clip_set(Evas_Object * o, Evas_Object * clip)
{
    Esmart_Resize *data;

    if ((data = (Esmart_Resize *) evas_object_smart_data_get(o))) {
        evas_object_clip_set(data->obj, clip);
    }
}

static void _esmart_resize_object_clip_unset(Evas_Object * o)
{
    Esmart_Resize *data;

    if ((data = (Esmart_Resize *) evas_object_smart_data_get(o))) {
        evas_object_clip_unset(data->obj);
    }
}
