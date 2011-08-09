#include <stdlib.h>

#include "eenvaders_evas_smart.h"

#define BG 0xff002b36
#define FG 0xff839496

#define MIN(a,b) (((a)<(b))?(a):(b))

/* Prototypes -{{{-*/
static Evas_Smart *_eenvaders_object_smart_get(void);
static Evas_Object *eenvaders_object_new(Evas *evas);
static void _eenvaders_object_del(Evas_Object *o);
static void _eenvaders_object_move(Evas_Object *o, Evas_Coord x, Evas_Coord y);
static void _eenvaders_object_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h);
static void _eenvaders_object_show(Evas_Object *o);
static void _eenvaders_object_hide(Evas_Object *o);
static void _eenvaders_object_color_set(Evas_Object *o, int r, int g, int b, int a);
static void _eenvaders_object_clip_set(Evas_Object *o, Evas_Object *clip);
static void _eenvaders_object_clip_unset(Evas_Object *o);

static void
draw_eenvaders(Evas_Object *smart_obj,
               int x, int y, int w, int h);
/* }}} */
/* Globals -{{{-*/

static struct {
    Evas_Smart_Class klass;
} eenvaders_evas_smart_g = {
    .klass = {
        .name = "eenvaders_object",
        .version = EVAS_SMART_CLASS_VERSION,
        .add = NULL,
        .del = _eenvaders_object_del,
        .move = _eenvaders_object_move,
        .resize = _eenvaders_object_resize,
        .show = NULL,
        .hide = NULL,
        .color_set = NULL,
        .clip_set = NULL,
        .clip_unset = NULL,
        .calculate = NULL,
        .member_add = NULL,
        .member_del = NULL,
        .parent = NULL,
        .callbacks = NULL,
        .interfaces = NULL,
        .data = NULL,
    },
#define _G eenvaders_evas_smart_g
};

/* }}} */
/* Eenvaders functions -{{{-*/

static void
eenvaders_mouse_down(void *data,
                     Evas *evas,
                     Evas_Object *child,
                     void *event_info)
{
    Evas_Coord x, y, w, h;
    Evas_Event_Mouse_Up *evt = event_info;
    Evas_Object *parent = data;
    void *mem;

    x = evt->canvas.x;
    y = evt->canvas.y;

    mem = evas_object_data_del(child, "m");
    if (!mem)
        return;

    free(mem);
    evas_object_geometry_get(child, &x, &y, &w, &h);
    evas_object_smart_member_del(child);
    evas_object_del(child);
    draw_eenvaders(parent, x+3, y+3, w-3, h-3);
}

static Evas_Object*
new_eenvader(Evas *evas, Evas_Object *smart_obj)
{
    Evas_Object *o = NULL;
    uint16_t u = rand();
    int *mem = calloc(7 * 7, sizeof(int));

    if (!mem) {
        perror(NULL);
        exit(1);
    }

    for (int i = 0; i < 15; i++) {
        if (u & (1 << i)) {
            mem[7 + 7*(i/3) + 1 + i%3] = FG;
            mem[7 + 7*(i/3) + 5 - i%3] = FG;
        }
    }

    o = evas_object_image_filled_add(evas);
    evas_object_image_alpha_set(o, EINA_TRUE);
    evas_object_image_fill_set(o, 0, 0, 7, 7);
    evas_object_image_smooth_scale_set(o, EINA_FALSE);
    evas_object_image_size_set (o, 7, 7);
    evas_object_image_data_set(o, (void *) mem);
    evas_object_data_set(o, "m", (void *) mem);

    evas_object_event_callback_add(o,
                                   EVAS_CALLBACK_MOUSE_DOWN,
                                   eenvaders_mouse_down,
                                   smart_obj);

    return o;
}

static int
square_ceil_7(int n)
{
    /* XXX: considering n >= 7 */
    int r = 1;

    n /= 7;

    while (n >>= 1) {
        r <<= 1;
    }
    return r * 7;
}

static void
draw_eenvaders(Evas_Object *smart_obj,
               int x, int y, int w, int h)
{
    Evas_Object *o;
    int d;

    if (w < 7 || h < 7)
        return;

    d = square_ceil_7(MIN(w,h));

    o = new_eenvader(evas_object_evas_get(smart_obj), smart_obj);
    evas_object_resize(o, d, d);
    evas_object_smart_member_add(o, smart_obj);

    switch(rand() & 3) {
      case 0:
        /* top-left */
        evas_object_move(o, x, y);
        evas_object_show(o);

        draw_eenvaders(smart_obj, x+d, y, w-d, h); /* right */
        draw_eenvaders(smart_obj, x, y+d, d, h-d); /* bottom */
        break;
      case 1:
        /* top-right */
        evas_object_move(o, x+w-d, y);
        evas_object_show(o);

        draw_eenvaders(smart_obj, x, y+d, w, h-d); /* bottom */
        draw_eenvaders(smart_obj, x, y, w-d, d); /* left */
        break;
      case 2:
        /* bottom-right */
        evas_object_move(o, x+w-d, y+h-d);
        evas_object_show(o);

        draw_eenvaders(smart_obj, x, y, w-d, h); /* left */
        draw_eenvaders(smart_obj, x+w-d, y, d, h-d); /* top */
        break;
      case 3:
        /* bottom-left */
        evas_object_move(o, x, y+h-d);
        evas_object_show(o);

        draw_eenvaders(smart_obj, x, y, w, h-d); /* top */
        draw_eenvaders(smart_obj, x+d, y+h-d, w-d, d); /* right */
        break;
    }
}

/* }}} */
/* Smart Object -{{{-*/

Evas_Object *
eenvaders_smart_new(Evas *e)
{
    return eenvaders_object_new(e);
}

static void
eenvaders_on_refresh(void *data, Evas_Object *o, void *event_info)
{
    Evas_Coord x, y, w, h;
    Evas_Object *child;
    void *mem;
    Eina_List *list;

    list = evas_object_smart_members_get(o);
    EINA_LIST_FREE(list, child) {
        void *mem;

        mem = evas_object_data_del(child, "m");
        free(mem);
        evas_object_event_callback_del(o, EVAS_CALLBACK_MOUSE_DOWN,
                                       eenvaders_mouse_down);
        evas_object_smart_member_del(child);
        evas_object_del(child);
    }

    evas_object_geometry_get(o, &x, &y, &w, &h);
    draw_eenvaders(o, x, y, w, h);
}

static Evas_Object *
eenvaders_object_new(Evas *evas)
{
    Evas_Object *eenvaders_object;

    eenvaders_object = evas_object_smart_add(evas,
                                _eenvaders_object_smart_get());
    evas_object_smart_callback_add(eenvaders_object,
                                   "refresh",
                                   eenvaders_on_refresh,
                                   NULL);

    return eenvaders_object;
}

static Evas_Smart *
_eenvaders_object_smart_get(void)
{
    static Evas_Smart *smart = NULL;

    if (smart)
        return smart;

    smart = evas_smart_class_new(&_G.klass);
    return smart;
}

static void
_eenvaders_object_del(Evas_Object *o)
{
    Evas_Object *child;
    void *mem;
    Eina_List *list;

    list = evas_object_smart_members_get(o);
    EINA_LIST_FREE(list, child) {
        void *mem;

        evas_object_smart_member_del(child);
        evas_object_del(child);
        mem = evas_object_data_del(child, "m");
        free(mem);

    }
}

static void
_eenvaders_object_move(Evas_Object *o, Evas_Coord x, Evas_Coord y)
{
    Evas_Coord orig_x, orig_y, dx, dy;
    Eina_List *lst;
    void *data;

    evas_object_geometry_get(o, &orig_x, &orig_y, NULL, NULL);
    dx = x - orig_x;
    dy = y - orig_y;

    lst = evas_object_smart_members_get(o);
    EINA_LIST_FREE(lst, data) {
        Evas_Object *child = data;

        evas_object_geometry_get(child, &orig_x, &orig_y, NULL, NULL);
        evas_object_move(child, orig_x + dx, orig_y + dy);
    }
}

static void
_eenvaders_object_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h)
{
    Evas_Coord x, y;
    Evas_Object *child;
    void *mem;
    Eina_List *list;

    list = evas_object_smart_members_get(o);
    EINA_LIST_FREE(list, child) {
        void *mem;

        mem = evas_object_data_del(child, "m");
        free(mem);
        evas_object_smart_member_del(child);
        evas_object_del(child);
    }

    evas_object_geometry_get(o, &x, &y, NULL, NULL);
    draw_eenvaders(o, x, y, w, h);
}

/* }}} */
