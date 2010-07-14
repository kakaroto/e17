
#include "grid_object.h"

typedef struct _Smart_Data Smart_Data;

typedef struct Item Item;

struct Item
{
    Evas_Object *obj;
    //position inside the object (not inside the window)
    int x,y;
    //if -1, the default size is used
    int w,h;
    int show;

    Evas_Object *parent;
};

struct _Smart_Data
{
    Evas_Object *obj;
    Evas_Object *catch_events;
    //list of Item
    Eina_List *objs;

    int obj_size_w;
    int obj_size_h;

    int freeze;

    Grid_Object_Region_CB region_cb;
    void *region_data;

    Grid_Object_Move_CB move_cb;
    void *move_data;

    int resize;

    Item *item_selected;

    //next and previous item when we move an item
    Item *move_previous;
    Item *move_next;
};

#define E_SMART_OBJ_GET_RETURN(smart, o, type, ret) \
   { \
      char *_e_smart_str; \
      \
      if (!o) return ret; \
      smart = evas_object_smart_data_get(o); \
      if (!smart) return ret; \
      _e_smart_str = (char *)evas_object_type_get(o); \
      if (!_e_smart_str) return ret; \
      if (strcmp(_e_smart_str, type)) return ret; \
   }

#define E_SMART_OBJ_GET(smart, o, type) \
     { \
	char *_e_smart_str; \
	\
	if (!o) return; \
	smart = evas_object_smart_data_get(o); \
	if (!smart) return; \
	_e_smart_str = (char *)evas_object_type_get(o); \
	if (!_e_smart_str) return; \
	if (strcmp(_e_smart_str, type)) return; \
     }

#define E_OBJ_NAME "grid_object"
static Evas_Smart  *smart = NULL;

static void _smart_init(void);
static void _smart_add(Evas_Object * obj);
static void _smart_del(Evas_Object * obj);
static void _smart_move(Evas_Object * obj, Evas_Coord x, Evas_Coord y);
static void _smart_resize(Evas_Object * obj, Evas_Coord w, Evas_Coord h);
static void _smart_show(Evas_Object * obj);
static void _smart_hide(Evas_Object * obj);
static void _smart_clip_set(Evas_Object * obj, Evas_Object * clip);
static void _smart_clip_unset(Evas_Object * obj);

static void _item_mouse_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _item_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _mouse_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);

void _update(Evas_Object *obj);


Evas_Object *grid_object_add(Evas *evas)
{
    _smart_init();
    return evas_object_smart_add(evas, smart);
}

void grid_object_freeze(Evas_Object *obj, int freeze)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd) return;

    sd->freeze = freeze;

    if(!freeze)
        _update(obj);
}

void grid_object_items_size_set(Evas_Object *obj, int w, int h)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd) return;

    sd->obj_size_w = w;
    sd->obj_size_h = h;

    sd->resize = 1;
    _update(obj);
}

void grid_object_region_cb_set(Evas_Object *obj, Grid_Object_Region_CB cb, void *data)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd) return;

    sd->region_cb = cb;
    sd->region_data = data;
}

void grid_object_move_cb_set(Evas_Object *obj, Grid_Object_Move_CB cb, void *data)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd) return;

    sd->move_cb = cb;
    sd->move_data = data;
}

void grid_object_item_append(Evas_Object *obj, Evas_Object *subobj)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd) return;

    Item *item = calloc(1,sizeof(Item));
    item->obj = subobj;
    item->w = -1;
    item->h = -1;
    item->parent = obj;

    sd->objs = eina_list_append(sd->objs, item);
    evas_object_smart_member_add(subobj, obj);
    evas_object_clip_set(subobj, sd->obj);

    evas_object_repeat_events_set(subobj, 1);
    evas_object_event_callback_add(subobj, EVAS_CALLBACK_MOUSE_DOWN, _item_mouse_down_cb, item);
    evas_object_event_callback_add(subobj, EVAS_CALLBACK_MOUSE_UP, _item_mouse_up_cb, item);

    _update(obj);

    evas_object_raise(sd->catch_events);
}

void grid_object_item_append_relative(Evas_Object *obj, Evas_Object *subobj, Evas_Object *relative)
{
    Smart_Data *sd;
    Eina_List *l;
    Item *item_prev;

    sd = evas_object_smart_data_get(obj);
    if (!sd) return;

    Item *item = calloc(1,sizeof(Item));
    item->obj = subobj;
    item->w = -1;
    item->h = -1;
    item->parent = obj;

    if(!relative)
        sd->objs = eina_list_prepend(sd->objs, item);
    else
    {
        EINA_LIST_FOREACH(sd->objs, l, item_prev)
        {
            if(item_prev->obj == relative)
                break;
        }

        sd->objs = eina_list_append_relative(sd->objs, item, item_prev);
    }
    evas_object_smart_member_add(subobj, obj);
    evas_object_clip_set(subobj, sd->obj);

    evas_object_repeat_events_set(subobj, 1);
    evas_object_event_callback_add(subobj, EVAS_CALLBACK_MOUSE_DOWN, _item_mouse_down_cb, item);
    evas_object_event_callback_add(subobj, EVAS_CALLBACK_MOUSE_UP, _item_mouse_up_cb, item);

    _update(obj);

    evas_object_raise(sd->catch_events);
}

void grid_object_item_prepend(Evas_Object *obj, Evas_Object *subobj)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd) return;

    Item *item = calloc(1,sizeof(Item));
    item->obj = subobj;
    item->w = -1;
    item->h = -1;
    item->parent = obj;

    sd->objs = eina_list_prepend(sd->objs, item);
    evas_object_smart_member_add(subobj, obj);
    evas_object_clip_set(subobj, sd->obj);

    evas_object_repeat_events_set(subobj, 1);
    evas_object_event_callback_add(subobj, EVAS_CALLBACK_MOUSE_DOWN, _item_mouse_down_cb, item);
    evas_object_event_callback_add(subobj, EVAS_CALLBACK_MOUSE_UP, _item_mouse_up_cb, item);

    _update(obj);

    evas_object_raise(sd->catch_events);
}

void grid_object_item_remove(Evas_Object *obj, Evas_Object *subobj)
{
    Smart_Data *sd;
    Eina_List *l;
    Item *item = NULL;

    sd = evas_object_smart_data_get(obj);
    if (!sd) return;

    EINA_LIST_FOREACH(sd->objs, l, item)
    {
        if(item->obj == subobj)
            break;
    }

    if(item)
    {
        sd->objs = eina_list_remove(sd->objs, item);
        evas_object_del(item->obj);
        free(item);
    }
    _update(obj);
}

void grid_object_clear(Evas_Object *obj)
{
    Smart_Data *sd;
    Item *item;

    sd = evas_object_smart_data_get(obj);
    if (!sd) return;

    EINA_LIST_FREE(sd->objs, item)
    {
        evas_object_del(item->obj);
        free(item);
    }
    sd->resize = 0;
    _update(obj);
}

void grid_object_item_replace(Evas_Object *obj, int id, Evas_Object *subobj)
{
    Smart_Data *sd;
    Eina_List *l;
    Item *item;
    int i = 0;

    sd = evas_object_smart_data_get(obj);
    if (!sd) return;

    EINA_LIST_FOREACH(sd->objs, l, item)
    {
        if(i == id)
            break;
        else
            i++;
    }

    if(!item) return ;

    evas_object_del(item->obj);
    item->obj = subobj;
    item->show = 0;
    evas_object_event_callback_add(subobj, EVAS_CALLBACK_MOUSE_DOWN, _item_mouse_down_cb, item);
    evas_object_event_callback_add(subobj, EVAS_CALLBACK_MOUSE_UP, _item_mouse_up_cb, item);

    evas_object_smart_member_add(subobj, obj);
    evas_object_clip_set(subobj, sd->obj);
    sd->resize = 1;
    _update(obj);

    evas_object_raise(sd->catch_events);
}

void _update(Evas_Object *obj)
{
    Smart_Data *sd;
    Eina_List *l;
    Item *item;
    int x,y,w,h;
    int x_cumul = 0;
    int y_cumul = 0;
    int i = 0;
    int w_obj, h_obj;
    int h_total = 0;
    int region_x, region_y, region_w, region_h;

    sd = evas_object_smart_data_get(obj);
    if (!sd) return;

    if(sd->freeze) return;

    evas_object_geometry_get(sd->obj, &x, &y, &w, &h);
    if(sd->region_cb)
    {
        // we  freeze because in some case we can have an infinite loop
        // example : region_cb() -> elm_scroll_region_get() -> edje_resize() -> grid move -> _update ...
        sd->freeze = 1;
        sd->region_cb(obj, &region_x, &region_y, &region_w, &region_h, sd->region_data);
        sd->freeze = 0;
    }

    evas_object_move(sd->catch_events, x, y);
    evas_object_resize(sd->catch_events, w, h);
    EINA_LIST_FOREACH(sd->objs, l, item)
    {
        if(item->w!=-1)
            w_obj = item->w;
        else
            w_obj = sd->obj_size_w;

        if(item->h!=-1)
            h_obj = item->h;
        else
            h_obj = sd->obj_size_h;


        if(w_obj>w)
            continue;

        item->x = x_cumul;
        if(item->x + w_obj > w)
        {
            item->x = 0;
            item->y = h_obj + y_cumul;
            y_cumul += h_obj;
            x_cumul = w_obj;
        }
        else
        {
            item->y = y_cumul;
            x_cumul += w_obj;
        }

        if(item->show && sd->item_selected != item &&
                (item->x+w_obj < region_x
                || item->x > region_w + region_x
                || item->y+h_obj < region_y
                || item->y > region_h + region_y) )
        {
            evas_object_hide(item->obj);
            item->show = 0;
        }
        else
        {
            if(sd->item_selected != item) //else the item is under the mouse cursor
            {
                evas_object_move(item->obj, item->x + x, item->y + y);

                if(sd->resize)
                    evas_object_resize(item->obj, w_obj, h_obj);
                if(!item->show)
                    evas_object_show(item->obj);
                item->show = 1;
            }
        }
        if(y_cumul + h_obj > h_total)
            h_total = y_cumul + h_obj;

        i++;
    }
    sd->resize = 0;
    evas_object_size_hint_min_set(obj, -1, h_total);

    if(sd->item_selected) evas_object_raise(sd->item_selected->obj);
    evas_object_raise(sd->catch_events);
}

static void _item_mouse_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
    Item *item =(Item*) data;
    Smart_Data *sd;
    sd = evas_object_smart_data_get(item->parent);
    if (!sd) return;
    sd->item_selected = item;

    evas_object_raise(obj);
    evas_object_raise(sd->catch_events);

    edje_object_signal_emit(obj, "select", "eyelight");
}

static void _item_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
    Item *item =(Item*) data;
    Smart_Data *sd;
    Eina_List *l;
    Item *_item;
    int id = 0, id_after = -1;

    sd = evas_object_smart_data_get(item->parent);
    if (!sd) return;

    edje_object_signal_emit(item->obj, "unselect", "eyelight");

    EINA_LIST_FOREACH(sd->objs, l, _item)
    {
        if(item == _item)
            break;
        else
            id++;
    }

    if(sd->move_next || sd->move_previous)
    {
        //maybe we only have a next, or only a previous
        if(sd->move_previous)
        {
            id_after = 0;
            EINA_LIST_FOREACH(sd->objs, l, _item)
            {
                if(sd->move_previous == _item)
                    break;
                else
                    id_after++;
            }
            sd->objs = eina_list_remove(sd->objs, sd->item_selected);
            sd->objs = eina_list_append_relative(sd->objs, sd->item_selected, sd->move_previous);
        }
        else
        {
            id_after = -1;
            EINA_LIST_FOREACH(sd->objs, l, _item)
            {
                if(_item == sd->move_next)
                    break;
                else
                    id_after++;
            }
            sd->objs = eina_list_remove(sd->objs, sd->item_selected);
            sd->objs = eina_list_prepend_relative(sd->objs, sd->item_selected, sd->move_next);
        }
        if(sd->move_cb)
            sd->move_cb(item->parent, id, id_after, sd->move_data);
    }

    if(sd->move_previous) edje_object_signal_emit(sd->move_previous->obj, "move,center", "eyelight");
    if(sd->move_next) edje_object_signal_emit(sd->move_next->obj, "move,center", "eyelight");
    sd->move_previous = sd->move_next = NULL;
    sd->item_selected = NULL;

    _update(item->parent);
}

static void _mouse_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
    Evas_Object *smart =(Evas_Object*) data;
    Smart_Data *sd;
    int w,h;

    Evas_Event_Mouse_Move *ev = (Evas_Event_Mouse_Move*) event_info;

    sd = evas_object_smart_data_get(smart);
    if (!sd) return;
    if(!sd->item_selected) return ;

    evas_object_geometry_get(sd->item_selected->obj,NULL,NULL,&w,&h);
    evas_object_move(sd->item_selected->obj, ev->cur.output.x-w/2, ev->cur.output.y-h/2);

    //test if we are between 2 items
    Eina_List *l;
    Item *item;
    int x,y;
    int region_x, region_y, region_w, region_h;
    Item *previous = NULL, *next = NULL;

    evas_object_geometry_get(sd->obj, &x, &y, NULL, NULL);
    if(sd->region_cb)
    {
        // we  freeze because in some case we can have an infinite loop
        // example : region_cb() -> elm_scroll_region_get() -> edje_resize() -> grid move -> _update ...
        sd->freeze = 1;
        sd->region_cb(obj, &region_x, &region_y, &region_w, &region_h, sd->region_data);
        sd->freeze = 0;
    }
    int i = 0;
    int i_prev = -1;
    int i_next = -1;
    EINA_LIST_FOREACH(sd->objs, l, item)
    {
        int w_obj = sd->obj_size_w;
        int h_obj = sd->obj_size_h;

        if(item->w!=-1) w_obj = item->w;
        if(item->h!=-1) h_obj = item->h;

        //the next when the cursor is at the end of the line
        if(item->y+y > ev->cur.output.y)
            break;


        if(item->x+x+ w_obj*3/4 < ev->cur.output.x)
        {
            previous = item;
            i_prev = i;
        }

        if(item->x+x+ w_obj/4 > ev->cur.output.x
                && item->y+y < ev->cur.output.y
                && item->y+y+h_obj > ev->cur.output.y)
        {
            next = item;
            i_next = i;
            break;
        }

        i++;
    }

    if(!(i_prev == -1 && i_next == 1) &&
            (i_next - 1 == i_prev || i_prev == -1 || i_next == -1))
    {
        if(i_prev > -1 && sd->item_selected != previous)
        {
            if(sd->move_previous != previous)
            {
                if(sd->move_previous) edje_object_signal_emit(sd->move_previous->obj, "move,center", "eyelight");
                edje_object_signal_emit(previous->obj, "move,left", "eyelight");
                sd->move_previous = previous;
            }
        }
        else
        {
            if(sd->move_previous) edje_object_signal_emit(sd->move_previous->obj, "move,center", "eyelight");
            sd->move_previous = NULL;
        }

        if(i_next > -1 && sd->item_selected != next)
        {
            if(sd->move_next != next)
            {
                if(sd->move_next) edje_object_signal_emit(sd->move_next->obj, "move,center", "eyelight");
                edje_object_signal_emit(next->obj, "move,right", "eyelight");
                sd->move_next = next;
            }
        }
        else
        {
            if(sd->move_next) edje_object_signal_emit(sd->move_next->obj, "move,center", "eyelight");
            sd->move_next = NULL;
        }
    }
    else
    {
        if(sd->move_previous) edje_object_signal_emit(sd->move_previous->obj, "move,center", "eyelight");
        if(sd->move_next) edje_object_signal_emit(sd->move_next->obj, "move,center", "eyelight");
        sd->move_previous = NULL;
        sd->move_next = NULL;
    }
}

/*******************************************/
/* Internal smart object required routines */
/*******************************************/
    static void
_smart_init(void)
{
    if (smart) return;
    {
        static const Evas_Smart_Class sc =
        {
            E_OBJ_NAME,
            EVAS_SMART_CLASS_VERSION,
            _smart_add,
            _smart_del,
            _smart_move,
            _smart_resize,
            _smart_show,
            _smart_hide,
            NULL,
            _smart_clip_set,
            _smart_clip_unset,
            NULL,
            NULL,
            NULL,
            NULL
        };
        smart = evas_smart_class_new(&sc);
    }
}

    static void
_smart_add(Evas_Object * obj)
{
    Smart_Data *sd;

    sd = calloc(1, sizeof(Smart_Data));
    if (!sd) return;
    evas_object_smart_data_set(obj, sd);

    sd->obj = evas_object_rectangle_add(evas_object_evas_get(obj));
    evas_object_smart_member_add(sd->obj, obj);
    evas_object_color_set(sd->obj, 255, 255, 255, 255);

    sd->catch_events = evas_object_rectangle_add(evas_object_evas_get(obj));
    evas_object_smart_member_add(sd->catch_events, obj);
    evas_object_color_set(sd->catch_events, 255, 255, 255, 0);
    evas_object_show(sd->catch_events);
    evas_object_clip_set(sd->catch_events, sd->obj);
    evas_object_event_callback_add(sd->catch_events, EVAS_CALLBACK_MOUSE_MOVE, _mouse_move_cb, obj);
    evas_object_repeat_events_set(sd->catch_events, 1);

    evas_object_raise(sd->catch_events);
}

    static void
_smart_del(Evas_Object * obj)
{
    Smart_Data *sd;
    Item *item;

    sd = evas_object_smart_data_get(obj);
    if (!sd) return;

    evas_object_smart_data_set(obj, NULL);
    EINA_LIST_FREE(sd->objs, item)
        if(item) free(item);
    eina_list_free(sd->objs);
    free(sd);
}

    static void
_smart_move(Evas_Object * obj, Evas_Coord x, Evas_Coord y)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd) return;

    evas_object_move(sd->obj,x, y);
    _update(obj);
}

    static void
_smart_resize(Evas_Object * obj, Evas_Coord w, Evas_Coord h)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd) return;

    evas_object_resize(sd->obj, w, h);
    sd->resize = 1;
    _update(obj);
}

    static void
_smart_show(Evas_Object * obj)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd) return;
    evas_object_show(sd->obj);
}

    static void
_smart_hide(Evas_Object * obj)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd) return;
    evas_object_hide(sd->obj);
}

    static void
_smart_clip_set(Evas_Object * obj, Evas_Object * clip)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd) return;
    evas_object_clip_set(sd->obj, clip);
}

    static void
_smart_clip_unset(Evas_Object * obj)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd) return;
    evas_object_clip_unset(sd->obj);
}

