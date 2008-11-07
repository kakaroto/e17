/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include "enna.h"
#include "list.h"

#define thumbscroll_friction 1.0
#define thumbscroll_momentum_threshhold 100
#define thumbscroll_threshhold 16

#define SMART_NAME "enna_list"

typedef struct _Smart_Data Smart_Data;
struct _Smart_Data
{
    Evas_Coord x, y, w, h, iw, ih;
    Evas_Object *o_smart;
    Evas_Object *o_edje;
    Evas_Object *o_box;
    Evas_Object *o_scroll;
    Eina_List *items;
    int selected;
    unsigned char selector : 1;
    unsigned char on_hold : 1;
    unsigned int letter_mode;
    Ecore_Timer *letter_timer;
    unsigned int letter_event_nbr;
    char letter_key;
};

static void _smart_init(void);
static void _smart_add(Evas_Object *obj);
static void _smart_del(Evas_Object *obj);
static void _smart_show(Evas_Object *obj);
static void _smart_hide(Evas_Object *obj);
static void _smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y);
static void _smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h);
static void _smart_color_set(Evas_Object *obj, int r, int g, int b, int a);
static void _smart_clip_set(Evas_Object *obj, Evas_Object *clip);
static void _smart_clip_unset(Evas_Object *obj);
static void _smart_reconfigure(Smart_Data *sd);
static void _smart_event_mouse_down(void *data, Evas *evas, Evas_Object *obj,
        void *event_info);
static void _smart_event_mouse_up(void *data, Evas *evas, Evas_Object *obj,
        void *event_info);
static void _smart_event_key_down(Smart_Data *sd, void *event_info);
static int _letter_timer_cb(void *data);
static Evas_Smart *_e_smart = NULL;

Evas_Object *
enna_list_add(Evas *evas)
{
    _smart_init();
    return evas_object_smart_add(evas, _e_smart);
}
void enna_list_append(Evas_Object *obj, Evas_Object *item, void (*func) (void *data, void *data2), void (*func_hilight) (void *data, void *data2), void *data, void *data2)
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
    sd->items = eina_list_append(sd->items, si);

    enna_listitem_min_size_get(si->o_base, &mw, &mh);

    enna_box_pack_end(sd->o_box, item);
    evas_object_size_hint_min_set(si->o_base, mw, mh);
    evas_object_size_hint_align_set(si->o_base, 0, 0.5);
    evas_object_size_hint_weight_set(si->o_base, 1.0, 1.0);

    evas_object_event_callback_add(si->o_base, EVAS_CALLBACK_MOUSE_DOWN,
            _smart_event_mouse_down, si);
    evas_object_event_callback_add(si->o_base, EVAS_CALLBACK_MOUSE_UP,
            _smart_event_mouse_up, si);

    evas_object_show(si->o_base);
}

Evas_Object *
enna_list_edje_object_get(Evas_Object *obj)
{
    API_ENTRY return NULL;
    return sd->o_edje;
}

void enna_list_min_size_get(Evas_Object *obj, Evas_Coord *w, Evas_Coord *h)
{
    API_ENTRY return;

    evas_object_size_hint_min_get(sd->o_edje, w, h);
}

void enna_list_unselect(Evas_Object *obj)
{
    Eina_List *l;

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

void enna_list_selected_set(Evas_Object *obj, int n)
{
    Enna_List_Item *si = NULL;
    Eina_List *l = NULL;
    int i;

    API_ENTRY
    return;
    if (!sd->items)
        return;

    i = eina_list_count(sd->items);
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
    if (!(si = eina_list_nth(sd->items, n)))
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

int enna_list_jump_label(Evas_Object *obj, const char *label)
{
    Enna_List_Item *si = NULL;
    Eina_List *l = NULL;
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

    if (!(si = eina_list_nth(sd->items, sd->selected)))
        return -1;

    enna_list_selected_set(sd->o_smart, sd->selected);
    evas_object_geometry_get(sd->o_box, &x, NULL, NULL, &h);
    y = h/eina_list_count(sd->items) * (i-3);
    enna_scrollframe_child_pos_set(sd->o_scroll, x, y);
    return sd->selected;

}

void enna_list_jump_nth(Evas_Object *obj, int n)
{
    Enna_List_Item *si = NULL;
    Eina_List *l = NULL;
    int i;
    Evas_Coord x, y, w, h;

    API_ENTRY
    return;
    if (!sd->items)
        return;

    i = eina_list_count(sd->items);
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
    if (!(si = eina_list_nth(sd->items, n)))
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

int enna_list_selected_get(Evas_Object *obj)
{
    API_ENTRY
    return -1;
    if (!sd->items)
        return -1;
    return sd->selected;
}

void * enna_list_selected_data_get(Evas_Object *obj)
{
    Enna_List_Item *si = NULL;

    API_ENTRY
    return NULL;
    if (!sd->items)
        return NULL;
    if (sd->selected < 0)
        return NULL;
    si = eina_list_nth(sd->items, sd->selected);
    if (si)
        return si->data;
    return NULL;
}

void * enna_list_selected_data2_get(Evas_Object *obj)
{
    Enna_List_Item *si = NULL;

    API_ENTRY
    return NULL;
    if (!sd->items)
        return NULL;
    if (sd->selected < 0)
        return NULL;
    si = eina_list_nth(sd->items, sd->selected);
    if (si)
        return si->data2;
    return NULL;
}

void enna_list_selected_geometry_get(Evas_Object *obj, Evas_Coord *x,
        Evas_Coord *y, Evas_Coord *w, Evas_Coord *h)
{
    Enna_List_Item *si = NULL;

    API_ENTRY
    return;
    if (!sd->items)
        return;
    if (sd->selected < 0)
        return;
    if (!(si = eina_list_nth(sd->items, sd->selected)))
        return;
    evas_object_geometry_get(si->o_base, x, y, w, h);
    if (x)
        *x -= sd->x;
    if (y)
        *y -= sd->y;
}

int enna_list_selected_count_get(Evas_Object *obj)
{
    Eina_List *l = NULL;
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

void enna_list_remove_num(Evas_Object *obj, int n)
{
    Enna_List_Item *si = NULL;

    API_ENTRY
    return;
    if (!sd->items)
        return;
    if (!(si = eina_list_nth(sd->items, n)))
        return;
    sd->items = eina_list_remove(sd->items, si);
    if (sd->selected == n)
        sd->selected = -1;
    evas_object_del(si->o_base);
    ENNA_FREE(si);
}

void enna_list_icon_size_set(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
    Eina_List *l = NULL;

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
	evas_object_size_hint_min_set(si->o_base, mw, mh);
	evas_object_size_hint_align_set(si->o_base, 0, 0.5);
    }
}

void enna_list_clear(Evas_Object *obj)
{
    API_ENTRY
    return;

    while (sd->items)
    {
        Enna_List_Item *si;
        si = sd->items->data;
        sd->items = eina_list_remove_list(sd->items, sd->items);
        evas_object_del(si->o_base);
        ENNA_FREE(si);
    }
    sd->selected = -1;
}

void enna_list_event_key_down(Evas_Object *obj, void *event_info)
{
    API_ENTRY
    return;
    _smart_event_key_down(sd, event_info);
    //enna_scrollframe_event_key_down(sd->o_scroll, event_info);

}

/* SMART FUNCTIONS */
static void _smart_init(void)
{
    if (_e_smart)
        return;
    {
        static const Evas_Smart_Class sc =
        { SMART_NAME, EVAS_SMART_CLASS_VERSION, _smart_add, _smart_del,
                _smart_move, _smart_resize, _smart_show, _smart_hide,
                _smart_color_set, _smart_clip_set, _smart_clip_unset,
                NULL, NULL };
        _e_smart = evas_smart_class_new(&sc);
    }
}

static void _smart_add(Evas_Object *obj)
{
    Smart_Data *sd;

    sd = calloc(1, sizeof(Smart_Data));
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
    evas_object_size_hint_align_set(sd->o_box, 0.0, 0.0);
    enna_box_homogenous_set(sd->o_box, 1);
    enna_box_orientation_set(sd->o_box, 0);

    sd->o_scroll = enna_scrollframe_add(evas_object_evas_get(obj));
    enna_scrollframe_policy_set(sd->o_scroll, ENNA_SCROLLFRAME_POLICY_OFF,
            ENNA_SCROLLFRAME_POLICY_AUTO);
    enna_scrollframe_child_set(sd->o_scroll, sd->o_box);
    edje_object_part_swallow(sd->o_edje, "enna.swallow.content", sd->o_scroll);

    evas_object_smart_member_add(sd->o_edje, obj);

    evas_object_propagate_events_set(obj, 0);
}

static int _letter_timer_cb(void *data)
{
    Smart_Data *sd;
    int i;
    Eina_List *l;

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
            y = h/eina_list_count(sd->items) * (i-3);
            enna_scrollframe_child_pos_set(sd->o_scroll, x, y);
            break;
        }
    }
    return 0;
}
static void _smart_del(Evas_Object *obj)
{
    INTERNAL_ENTRY ;

    evas_object_del(sd->o_scroll);
    evas_object_del(sd->o_box);
    while (sd->items)
    {
	Enna_List_Item *si = sd->items->data;
	sd->items = eina_list_remove_list(sd->items, sd->items);
	evas_object_del(si->o_base);
	free(si);
    }

    evas_object_del(sd->o_edje);
    free(sd);
}

static void _smart_show(Evas_Object *obj)
{
    INTERNAL_ENTRY ;
    evas_object_show(sd->o_edje);
}

static void _smart_hide(Evas_Object *obj)
{
    INTERNAL_ENTRY;
    evas_object_hide(sd->o_edje);
}

static void _smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
    INTERNAL_ENTRY
    ;
    if ((sd->x == x) && (sd->y == y))
        return;
    sd->x = x;
    sd->y = y;
    _smart_reconfigure(sd);
}

static void _smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
    INTERNAL_ENTRY
    ;
    if ((sd->w == w) && (sd->h == h))
        return;
    sd->w = w;
    sd->h = h;
    _smart_reconfigure(sd);
}

static void _smart_color_set(Evas_Object *obj, int r, int g, int b, int a)
{
    INTERNAL_ENTRY
    ;
    evas_object_color_set(sd->o_edje, r, g, b, a);
}

static void _smart_clip_set(Evas_Object *obj, Evas_Object *clip)
{
    INTERNAL_ENTRY
    ;
    evas_object_clip_set(sd->o_edje, clip);
}

static void _smart_clip_unset(Evas_Object *obj)
{
    INTERNAL_ENTRY
    ;
    evas_object_clip_unset(sd->o_edje);
}

static void _smart_reconfigure(Smart_Data *sd)
{
    Evas_Coord mh, mw,  h = 0;
    Evas_List *l;

    evas_object_move(sd->o_edje, sd->x, sd->y);
    evas_object_resize(sd->o_edje, sd->w, sd->h);

    for (l = sd->items; l; l = l->next)
    {
        Enna_List_Item *si = l->data;
        enna_listitem_min_size_get(si->o_base, &mw, &mh);
        evas_object_size_hint_min_set(si->o_base, sd->w, mh);
        evas_object_size_hint_align_set(si->o_base, 0, 0.5);
        evas_object_size_hint_weight_set(si->o_base, 1.0, 1.0);
        h += mh;
    }
    evas_object_size_hint_min_get(sd->o_box, NULL, &mh);
    evas_object_resize(sd->o_box, sd->w, h);

}

static void _smart_event_mouse_down(void *data, Evas *evas, Evas_Object *obj,
        void *event_info)
{
    Smart_Data *sd;
    Evas_Event_Mouse_Down *ev;
    Enna_List_Item *si;
    Eina_List *l = NULL;
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
static void _smart_event_mouse_up(void *data, Evas *evas, Evas_Object *obj,
        void *event_info)
{
    Smart_Data *sd;
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
    //if (!(si = eina_list_nth(sd->items, sd->selected))) return;
    if (sd->on_hold)
    {
        sd->on_hold = 0;
        return;
    }
    if (si->func)
        si->func(si->data, si->data2);
}

static void list_item_select(Smart_Data *sd, int n)
{
    /*Evas_Coord x, y, h;

    enna_list_selected_set(sd->o_smart, n);
    evas_object_geometry_get(sd->o_box, &x, NULL, NULL, &h);

    y = h / eina_list_count(sd->items) * n;

    enna_scrollframe_child_pos_set(sd->o_scroll, x, y);*/


    Evas_Coord x, y;
    Evas_Coord xedje, yedje, hedje, ybox;

    enna_list_selected_set(sd->o_smart, n);
    enna_scrollframe_child_pos_get(sd->o_scroll, &x, &y);
    enna_list_selected_geometry_get(sd->o_smart, &xedje, &yedje, NULL, &hedje);
    evas_object_geometry_get(sd->o_box, NULL, &ybox, NULL, NULL);
    y = (yedje + hedje / 2 - ybox - sd->h / 2 + sd->y);
    enna_scrollframe_child_pos_set(sd->o_scroll, x , y);

}

static void list_set_item(Smart_Data *sd, int start, int up, int step)
{
    Enna_List_Item *si;
    int n, ns;

    ns = start;
    n = ns;
    do
    {
        int i = up ? eina_list_count(sd->items) - 1 : 0;

        if (n == i)
        {
            n = ns;
            break;
        }
        n = up ? n + step : n - step;
        si = eina_list_nth(sd->items, n);
    } while (0);

    if (n != ns)
        list_item_select(sd, n);
}

static void list_jump_to_ascii(Smart_Data *sd, char k)
{
    Eina_List *l;
    int i = 0;

    l = sd->items;
    while (l)
    {
        Enna_List_Item *item;
        Enna_Vfs_File *file;
        item = eina_list_data_get(l);
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

    if (i != eina_list_count(sd->items))
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

static void list_get_alpha_from_digit(Smart_Data *sd, char key)
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

static void _smart_event_key_down(Smart_Data *sd, void *event_info)
{
    Evas_Event_Key_Down *ev;
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
            list_set_item(sd, eina_list_count(sd->items), 0, 1);
            break;
        case ENNA_KEY_OK:
        case ENNA_KEY_SPACE:
        {
            if (!sd->on_hold)
            {
                si = eina_list_nth(sd->items, sd->selected);
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
            char key = ev->key[strlen(ev->key) - 1];
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
