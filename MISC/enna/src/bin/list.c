/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include "enna.h"
#include "list.h"

#define thumbscroll_friction 1.0
#define thumbscroll_momentum_threshhold 100
#define thumbscroll_threshhold 16

#define SMART_NAME "enna_list"

#define API_ENTRY \
   E_Smart_Data *sd; \
   sd = evas_object_smart_data_get(obj); \
   if ((!obj) || (!sd) || \
      (evas_object_type_get(obj) \
      && strcmp(evas_object_type_get(obj), SMART_NAME)))

#define INTERNAL_ENTRY \
   E_Smart_Data *sd; \
   sd = evas_object_smart_data_get(obj); \
   if (!sd) return;

typedef struct _E_Smart_Data E_Smart_Data;
struct _E_Smart_Data
{
    Evas_Coord x, y, w, h, iw, ih;
    Evas_Object *o_smart;
    Evas_Object *o_edje;
    Evas_Object *o_box;
    Evas_Object *o_scroll;
    Evas_List *items;
    int selected;
    unsigned char selector : 1;
    unsigned char on_hold : 1;
    unsigned int letter_mode;
    Ecore_Timer *letter_timer;
    unsigned int letter_event_nbr;
    char letter_key;
};

static void _e_smart_init(void);
static void _e_smart_add(Evas_Object *obj);
static void _e_smart_del(Evas_Object *obj);
static void _e_smart_show(Evas_Object *obj);
static void _e_smart_hide(Evas_Object *obj);
static void _e_smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y);
static void _e_smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h);
static void _e_smart_color_set(Evas_Object *obj, int r, int g, int b, int a);
static void _e_smart_clip_set(Evas_Object *obj, Evas_Object *clip);
static void _e_smart_clip_unset(Evas_Object *obj);
static void _e_smart_reconfigure(E_Smart_Data *sd);
static void _e_smart_event_mouse_down(void *data, Evas *evas, Evas_Object *obj,
        void *event_info);
static void _e_smart_event_mouse_up(void *data, Evas *evas, Evas_Object *obj,
        void *event_info);
static void _e_smart_event_key_down(E_Smart_Data *sd, void *event_info);
static int _letter_timer_cb(void *data);
static Evas_Smart *_e_smart = NULL;

EAPI Evas_Object *
enna_list_add(Evas *evas)
{
    _e_smart_init();
    return evas_object_smart_add(evas, _e_smart);
}
EAPI void enna_list_append(Evas_Object *obj, Evas_Object *item, void (*func) (void *data, void *data2), void (*func_hilight) (void *data, void *data2), void *data, void *data2)
{
    Enna_List_Item *si;
    Evas_Coord mw = 0, mh = 0;

    API_ENTRY
    return;
    si = ENNA_NEW(Enna_List_Item, 1);
    si->sd = sd;
    si->o_base = item;

    si->func = func;
    si->func_hilight = func_hilight;
    si->data = data;
    si->data2 = data2;
    sd->items = evas_list_append(sd->items, si);

    enna_listitem_min_size_get(si->o_base, &mw, &mh);
    enna_box_freeze(sd->o_box);
    enna_box_pack_end(sd->o_box, si->o_base);

    enna_box_pack_options_set(si->o_base, 1, 0, 1, 0, 0, 0.5, mw, mh, 99999,
            99999);
    enna_box_min_size_get(sd->o_box, NULL, &mh);
    evas_object_resize(sd->o_box, sd->w, mh);
    enna_box_thaw(sd->o_box);

    evas_object_lower(sd->o_box);
    evas_object_event_callback_add(si->o_base, EVAS_CALLBACK_MOUSE_DOWN,
            _e_smart_event_mouse_down, si);
    evas_object_event_callback_add(si->o_base, EVAS_CALLBACK_MOUSE_UP,
            _e_smart_event_mouse_up, si);

    evas_object_show(si->o_base);
}

EAPI Evas_Object *
enna_list_edje_object_get(Evas_Object *obj)
{
    API_ENTRY return NULL;
    return sd->o_edje;
}

EAPI void enna_list_min_size_get(Evas_Object *obj, Evas_Coord *w, Evas_Coord *h)
{
    API_ENTRY
    return;
    enna_box_min_size_get(sd->o_edje, w, h);
}

EAPI void enna_list_unselect(Evas_Object *obj)
{
    Evas_List *l;

    API_ENTRY
    return;
    if (!sd->items)
        return;
    if (sd->selected < 0)
        return;
    for (l = sd->items; l; l = l->next)
    {
        Enna_List_Item *si = NULL;

        if (!(si = l->data))
            continue;
        if (!si->selected)
            continue;
        enna_listitem_select(si->o_base);
        si->selected = 0;
    }
    sd->selected = -1;
}

EAPI void enna_list_selected_set(Evas_Object *obj, int n)
{
    Enna_List_Item *si = NULL;
    Evas_List *l = NULL;
    int i;

    API_ENTRY
    return;
    if (!sd->items)
        return;

    i = evas_list_count(sd->items);
    if (n >= i)
        n = i - 1;
    else if (n < 0)
        n = 0;

    for (l = sd->items; l; l = l->next)
    {
        if (!(si = l->data))
            continue;
        if (!si->selected)
            continue;
        enna_listitem_unselect(si->o_base);
        si->selected = 0;
    }
    sd->selected = -1;
    if (!(si = evas_list_nth(sd->items, n)))
        return;

    si->selected = 1;
    sd->selected = n;
    evas_object_raise(si->o_base);
    enna_listitem_select(si->o_base);
    if (si->func_hilight)
        si->func_hilight(si->data, si->data2);
    if (sd->selector)
        return;
    if (!sd->on_hold)
    {
        //mif (si->func) si->func(si->data, si->data2);
    }

}

EAPI int enna_list_jump_label(Evas_Object *obj, const char *label)
{
    Enna_List_Item *si = NULL;
    Evas_List *l = NULL;
    int i;
    Evas_Coord x, y, h;
    enna_log(ENNA_MSG_EVENT, NULL, "enna_list_jump_label %s", label);

    API_ENTRY
    return -1;
    enna_log(ENNA_MSG_EVENT, NULL, "return 1");
    if (!sd->items || !label)
        return -1;

    sd->selected = -1;
    for (l = sd->items, i = 0; l; l = l->next, i++)
    {
        if (!(si = l->data))
            continue;
        if (si->selected)
            enna_listitem_unselect(si->o_base);

        if (!strcmp(enna_listitem_label_get(si->o_base), label))
        {
            enna_listitem_select(si->o_base);
            si->selected = 1;
            sd->selected = i;
            enna_log(ENNA_MSG_EVENT, NULL, "label get : %s",
                    enna_listitem_label_get(si->o_base));
            break;
        }

    }

    if (!(si = evas_list_nth(sd->items, sd->selected)))
        return -1;

    enna_list_selected_set(sd->o_smart, sd->selected);
    evas_object_geometry_get(sd->o_box, &x, NULL, NULL, &h);
    y = h/evas_list_count(sd->items) * (i-3);
    enna_scrollframe_child_pos_set(sd->o_scroll, x, y);
    return sd->selected;

}

EAPI void enna_list_jump_nth(Evas_Object *obj, int n)
{
    Enna_List_Item *si = NULL;
    Evas_List *l = NULL;
    int i;
    Evas_Coord x, y, w, h;

    API_ENTRY
    return;
    if (!sd->items)
        return;

    i = evas_list_count(sd->items);
    if (n >= i)
        n = i - 1;
    else if (n < 0)
        n = 0;

    for (l = sd->items; l; l = l->next)
    {
        if (!(si = l->data))
            continue;
        if (!si->selected)
            continue;
        enna_listitem_unselect(si->o_base);
        si->selected = 0;
    }
    sd->selected = -1;
    if (!(si = evas_list_nth(sd->items, n)))
        return;

    si->selected = 1;
    sd->selected = n;
    evas_object_raise(si->o_base);
    enna_listitem_select(si->o_base);
    evas_object_geometry_get(si->o_base, &x, &y, &w, &h);
    enna_scrollframe_child_region_show(sd->o_scroll, x, y, w, h);
    if (si->func_hilight)
        si->func_hilight(si->data, si->data2);

}

EAPI int enna_list_selected_get(Evas_Object *obj)
{
    API_ENTRY
    return -1;
    if (!sd->items)
        return -1;
    return sd->selected;
}

EAPI void * enna_list_selected_data_get(Evas_Object *obj)
{
    Enna_List_Item *si = NULL;

    API_ENTRY
    return NULL;
    if (!sd->items)
        return NULL;
    if (sd->selected < 0)
        return NULL;
    si = evas_list_nth(sd->items, sd->selected);
    if (si)
        return si->data;
    return NULL;
}

EAPI void * enna_list_selected_data2_get(Evas_Object *obj)
{
    Enna_List_Item *si = NULL;

    API_ENTRY
    return NULL;
    if (!sd->items)
        return NULL;
    if (sd->selected < 0)
        return NULL;
    si = evas_list_nth(sd->items, sd->selected);
    if (si)
        return si->data2;
    return NULL;
}

EAPI void enna_list_selected_geometry_get(Evas_Object *obj, Evas_Coord *x,
        Evas_Coord *y, Evas_Coord *w, Evas_Coord *h)
{
    Enna_List_Item *si = NULL;

    API_ENTRY
    return;
    if (!sd->items)
        return;
    if (sd->selected < 0)
        return;
    if (!(si = evas_list_nth(sd->items, sd->selected)))
        return;
    evas_object_geometry_get(si->o_base, x, y, w, h);
    if (x)
        *x -= sd->x;
    if (y)
        *y -= sd->y;
}

EAPI int enna_list_selected_count_get(Evas_Object *obj)
{
    Evas_List *l = NULL;
    int count = 0;

    API_ENTRY
    return 0;
    if (!sd->items)
        return 0;
    for (l = sd->items; l; l = l->next)
    {
        Enna_List_Item *si = NULL;

        if (!(si = l->data))
            continue;
        if (si->selected)
            count++;
    }
    return count;
}

EAPI void enna_list_remove_num(Evas_Object *obj, int n)
{
    Enna_List_Item *si = NULL;

    API_ENTRY
    return;
    if (!sd->items)
        return;
    if (!(si = evas_list_nth(sd->items, n)))
        return;
    sd->items = evas_list_remove(sd->items, si);
    if (sd->selected == n)
        sd->selected = -1;
    evas_object_del(si->o_base);
    ENNA_FREE(si);
}

EAPI void enna_list_icon_size_set(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
    Evas_List *l = NULL;

    API_ENTRY
    return;
    if ((sd->iw == w) && (sd->ih == h))
        return;
    sd->iw = w;
    sd->ih = h;
    for (l = sd->items; l; l = l->next)
    {
        Enna_List_Item *si = NULL;
        Evas_Coord mw = 0, mh = 0;

        if (!(si = l->data))
            continue;
        enna_listitem_min_size_set(si->o_base, w, h);
        enna_listitem_min_size_get(si->o_base, &mw, &mh);
        enna_box_pack_options_set(si->o_base, 1, 0, 1, 0, 0, 0.5, mw, mh,
                99999, 99999);
    }
}

EAPI void enna_list_clear(Evas_Object *obj)
{
    API_ENTRY
    return;

    enna_list_freeze(obj);
    while (sd->items)
    {
        Enna_List_Item *si;
        si = sd->items->data;
        sd->items = evas_list_remove_list(sd->items, sd->items);
        evas_object_del(si->o_base);
        ENNA_FREE(si);
    }
    enna_list_thaw(obj);
    sd->selected = -1;
}

EAPI void enna_list_freeze(Evas_Object *obj)
{
    API_ENTRY
    return;
    enna_box_freeze(sd->o_box);
}

EAPI void enna_list_thaw(Evas_Object *obj)
{
    API_ENTRY
    return;
    enna_box_thaw(sd->o_box);
}

EAPI void enna_list_event_key_down(Evas_Object *obj, void *event_info)
{
    API_ENTRY
    return;
    _e_smart_event_key_down(sd, event_info);
    //enna_scrollframe_event_key_down(sd->o_scroll, event_info);

}

/* SMART FUNCTIONS */
static void _e_smart_init(void)
{
    if (_e_smart)
        return;
    {
        static const Evas_Smart_Class sc =
        { SMART_NAME, EVAS_SMART_CLASS_VERSION, _e_smart_add, _e_smart_del,
                _e_smart_move, _e_smart_resize, _e_smart_show, _e_smart_hide,
                _e_smart_color_set, _e_smart_clip_set, _e_smart_clip_unset,
                NULL, NULL };
        _e_smart = evas_smart_class_new(&sc);
    }
}

static void _e_smart_add(Evas_Object *obj)
{
    E_Smart_Data *sd;
    Evas_Coord mw, mh;
    sd = calloc(1, sizeof(E_Smart_Data));
    if (!sd)
        return;
    evas_object_smart_data_set(obj, sd);

    sd->o_smart = obj;
    sd->x = sd->y = sd->w = sd->h = 0;
    sd->iw = sd->ih = 48;
    sd->selected = -1;

    sd->o_edje = edje_object_add(evas_object_evas_get(obj));
    edje_object_file_set(sd->o_edje, enna_config_theme_get(), "enna/list");
    sd->o_box = enna_box_add(evas_object_evas_get(obj));
    //enna_box_align_set(sd->o_box, 0.0, 0.0);
    enna_box_homogenous_set(sd->o_box, 0);
    enna_box_orientation_set(sd->o_box, 0);

    sd->o_scroll = enna_scrollframe_add(evas_object_evas_get(obj));
    enna_scrollframe_policy_set(sd->o_scroll, ENNA_SCROLLFRAME_POLICY_OFF,
            ENNA_SCROLLFRAME_POLICY_AUTO);
    enna_scrollframe_child_set(sd->o_scroll, sd->o_box);
    edje_object_part_swallow(sd->o_edje, "enna.swallow.content", sd->o_scroll);
    enna_box_min_size_get(sd->o_box, &mw, &mh);
    evas_object_resize(sd->o_box, mw, mh);
    evas_object_smart_member_add(sd->o_edje, obj);

    evas_object_propagate_events_set(obj, 0);
}

static int _letter_timer_cb(void *data)
{
    E_Smart_Data *sd;
    int i;
    Evas_List *l;

    sd = data;

    edje_object_signal_emit(sd->o_edje, "letter,hide", "enna");
    sd->letter_mode = 0;
    for (i = 0, l = sd->items; l; l = l->next, i++)
    {
        char *label;
        char *letter;
        Enna_List_Item *si;
        si = l->data;
        label = (char*)enna_listitem_label_get(si->o_base);
        letter = (char*)edje_object_part_text_get(sd->o_edje,
                "enna.text.letter");
        if ((letter && label) && (letter[0] == label[0] || letter[0]
                == label[0] + 32 || letter[0] + 32 == label[0] ))
        {
            Evas_Coord x, y, h;
            enna_list_selected_set(sd->o_smart, i);
            evas_object_geometry_get(sd->o_box, &x, NULL, NULL, &h);
            y = h/evas_list_count(sd->items) * (i-3);
            enna_scrollframe_child_pos_set(sd->o_scroll, x, y);
            break;
        }
    }
    return 0;
}
static void _e_smart_del(Evas_Object *obj)
{
    INTERNAL_ENTRY
    ;
    /*
     * FIXME if sd->o_box is deleting
     * Segv in enna ?!!
     */

    /*evas_object_del(sd->o_box);*/
    enna_list_clear(obj);
    evas_object_del(sd->o_edje);

    evas_object_del(sd->o_scroll);
    free(sd);
}

static void _e_smart_show(Evas_Object *obj)
{
    INTERNAL_ENTRY
    ;
    evas_object_show(sd->o_edje);
}

static void _e_smart_hide(Evas_Object *obj)
{
    INTERNAL_ENTRY
    ;
    evas_object_hide(sd->o_edje);
}

static void _e_smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
    INTERNAL_ENTRY
    ;
    if ((sd->x == x) && (sd->y == y))
        return;
    sd->x = x;
    sd->y = y;
    _e_smart_reconfigure(sd);
}

static void _e_smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
    INTERNAL_ENTRY
    ;
    if ((sd->w == w) && (sd->h == h))
        return;
    sd->w = w;
    sd->h = h;
    _e_smart_reconfigure(sd);
}

static void _e_smart_color_set(Evas_Object *obj, int r, int g, int b, int a)
{
    INTERNAL_ENTRY
    ;
    evas_object_color_set(sd->o_edje, r, g, b, a);
}

static void _e_smart_clip_set(Evas_Object *obj, Evas_Object *clip)
{
    INTERNAL_ENTRY
    ;
    evas_object_clip_set(sd->o_edje, clip);
}

static void _e_smart_clip_unset(Evas_Object *obj)
{
    INTERNAL_ENTRY
    ;
    evas_object_clip_unset(sd->o_edje);
}

static void _e_smart_reconfigure(E_Smart_Data *sd)
{
    Evas_Coord mh;
    evas_object_move(sd->o_edje, sd->x, sd->y);
    evas_object_resize(sd->o_edje, sd->w, sd->h);
    enna_box_min_size_get(sd->o_box, NULL, &mh);
    evas_object_resize(sd->o_box, sd->w, mh);
}

static void _e_smart_event_mouse_down(void *data, Evas *evas, Evas_Object *obj,
        void *event_info)
{
    E_Smart_Data *sd;
    Evas_Event_Mouse_Down *ev;
    Enna_List_Item *si;
    Evas_List *l = NULL;
    int i;

    ev = event_info;
    si = data;
    sd = si->sd;

    if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD)
        sd->on_hold = 1;
    else
        sd->on_hold = 0;

    if (!sd->items)
        return;
    for (i = 0, l = sd->items; l; l = l->next, i++)
    {
        if (l->data == si)
        {
            enna_list_selected_set(sd->o_smart, i);
            break;
        }
    }

}

/* FIXME remove do{}while(0) in this code ! */
static void _e_smart_event_mouse_up(void *data, Evas *evas, Evas_Object *obj,
        void *event_info)
{
    E_Smart_Data *sd;
    Evas_Event_Mouse_Up *ev;
    Enna_List_Item *si;

    ev = event_info;
    si = data;
    sd = si->sd;

    if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD)
        sd->on_hold = 1;
    else
        sd->on_hold = 0;

    if (!sd->items)
        return;
    //if (!sd->selector) return;
    //if (!(si = evas_list_nth(sd->items, sd->selected))) return;
    if (sd->on_hold)
    {
        sd->on_hold = 0;
        return;
    }
    if (si->func)
        si->func(si->data, si->data2);
}

static void list_item_select(E_Smart_Data *sd, int n)
{
    Evas_Coord x, y, h;
    enna_list_selected_set(sd->o_smart, n);
    evas_object_geometry_get(sd->o_box, &x, NULL, NULL, &h);
    y = h / evas_list_count(sd->items) * n;
    enna_scrollframe_child_pos_set(sd->o_scroll, x, y);
}

static void list_set_item(E_Smart_Data *sd, int start, int up, int step)
{
    Enna_List_Item *si;
    int n, ns;

    ns = start;
    n = ns;
    do
    {
        int i = up ? evas_list_count(sd->items) - 1 : 0;

        if (n == i)
        {
            n = ns;
            break;
        }
        n = up ? n + step : n - step;
        si = evas_list_nth(sd->items, n);
    } while (0);

    if (n != ns)
        list_item_select(sd, n);
}

static void list_jump_to_ascii(E_Smart_Data *sd, char k)
{
    Evas_List *l;
    int i = 0;

    l = sd->items;
    while (l)
    {
        Enna_List_Item *item;
        Enna_Vfs_File *file;
        item = evas_list_data(l);
        if (!item)
            continue;
        file = (Enna_Vfs_File *) item->data2;
        if (!file)
            continue;

        if (tolower(file->label[0]) == k)
            break;
        l = l->next;
        i++;
    }

    if (i != evas_list_count(sd->items))
        list_item_select(sd, i);
}

static char list_get_letter_from_key(char key)
{
    switch (key)
    {
        case '7':
            return 'P';
        case '8':
            return 'T';
        case '9':
            return 'W';
        default:
            return ((key - 50) * 3 + 65);
    }
}

static void list_get_alpha_from_digit(E_Smart_Data *sd, char key)
{
    char letter[2];

    letter[0] = list_get_letter_from_key(key);
    letter[1] = '\0';

    sd->letter_mode = 1;

    if (!sd->letter_mode)
    {
        sd->letter_event_nbr = 0;
        sd->letter_key = key;
    }
    else
    {
        int mod;

        ecore_timer_del(sd->letter_timer);
        mod = (key == '7' || key == '9') ? 4 : 3;

        if (sd->letter_key == key)
            sd->letter_event_nbr = (sd->letter_event_nbr + 1) % mod;
        else
        {
            sd->letter_event_nbr = 0;
            sd->letter_key = key;
        }

        letter[0] += sd->letter_event_nbr;
    }

    edje_object_signal_emit(sd->o_edje, "letter,show", "enna");
    enna_log(ENNA_MSG_EVENT, NULL, "letter : %s", letter);
    edje_object_part_text_set(sd->o_edje, "enna.text.letter", letter);
    sd->letter_timer = ecore_timer_add(1.5, _letter_timer_cb, sd);
    list_jump_to_ascii(sd, letter[0]);
}

static void _e_smart_event_key_down(E_Smart_Data *sd, void *event_info)
{
    Ecore_X_Event_Key_Down *ev;
    Enna_List_Item *si;
    enna_key_t keycode;
    int ns;

    ev = event_info;
    ns = sd->selected;
    keycode = enna_get_key(ev);

    //if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD) sd->on_hold = 1;
    //else sd->on_hold = 0;

    switch (keycode)
    {
        case ENNA_KEY_UP:
            list_set_item(sd, ns, 0, 1);
            break;
        case ENNA_KEY_PAGE_UP:
            list_set_item(sd, ns, 0, 5);
            break;
        case ENNA_KEY_DOWN:
            list_set_item(sd, ns, 1, 1);
            break;
        case ENNA_KEY_PAGE_DOWN:
            list_set_item(sd, ns, 1, 5);
            break;
        case ENNA_KEY_HOME:
            list_set_item(sd, -1, 1, 1);
            break;
        case ENNA_KEY_END:
            list_set_item(sd, evas_list_count(sd->items), 0, 1);
            break;
        case ENNA_KEY_OK:
        case ENNA_KEY_SPACE:
        {
            if (!sd->on_hold)
            {
                si = evas_list_nth(sd->items, sd->selected);
                if (si)
                {
                    if (si->func)
                        si->func(si->data, si->data2);
                }
            }
        }
            break;
        case ENNA_KEY_2:
        case ENNA_KEY_3:
        case ENNA_KEY_4:
        case ENNA_KEY_5:
        case ENNA_KEY_6:
        case ENNA_KEY_7:
        case ENNA_KEY_8:
        case ENNA_KEY_9:
        {
            char key = ev->keysymbol[strlen(ev->keysymbol) - 1];
            list_get_alpha_from_digit(sd, key);
        }
            break;
        default:
            if (enna_key_is_alpha(keycode))
            {
                char k = enna_key_get_alpha(keycode);
                list_jump_to_ascii(sd, k);
            }
            break;
    }

    sd->on_hold = 0;
}
