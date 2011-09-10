#include "e_mod_tiling.h"

/* types {{{ */

#define TILING_OVERLAY_TIMEOUT 5.0
#define TILING_RESIZE_STEP 5
#define TILING_POPUP_LAYER 101
#define TILING_WRAP_SPEED 0.1
static const char TILING_KEYS[] = "asdfghkl;'qwertyuiop[]\\zxcvbnm,./`1234567890-=";

typedef enum {
    TILING_RESIZE,
    TILING_MOVE,
} tiling_change_t;

typedef enum {
    INPUT_MODE_NONE,
    INPUT_MODE_SWAPPING,
    INPUT_MODE_MOVING,
    INPUT_MODE_GOING,
    INPUT_MODE_TRANSITION,
} tiling_input_mode_t;

typedef enum {
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,

    MOVE_COUNT
} tiling_move_t;

typedef struct geom_t {
    int x, y, w, h;
} geom_t;

typedef struct overlay_t {
    E_Popup *popup;
    Evas_Object *obj;
} overlay_t;

typedef struct transition_overlay_t {
    overlay_t overlay;
    int col;
    E_Border *bd;
    char key[2];
} transition_overlay_t;

typedef struct Border_Extra {
    E_Border *border;
    geom_t expected, orig;
    overlay_t overlay;
    char key[2];
} Border_Extra;

struct tiling_g tiling_g = {
    .module = NULL,
    .config = NULL,
    .log_domain = -1,
};

static void
_add_border(E_Border *bd);

/* }}} */
/* Globals {{{ */

static struct
{
    char                  edj_path[PATH_MAX];
    E_Config_DD          *config_edd,
                         *vdesk_edd;
    E_Border_Hook        *hook;
    int                   currently_switching_desktop;
    Ecore_Event_Handler  *handler_hide,
                         *handler_desk_show,
                         *handler_desk_before_show,
                         *handler_mouse_move,
                         *handler_desk_set;

    Tiling_Info          *tinfo;
    /* This hash holds the Tiling_Info-pointers for each desktop */
    Eina_Hash            *info_hash;

    Eina_Hash            *border_extras;

    Eina_Hash            *overlays;

    E_Action             *act_togglefloat,
                         *act_addcolumn,
                         *act_removecolumn,
                         *act_swap,
                         *act_move,
                         *act_adjusttransitions,
                         *act_go;

    int                   warp_x,
                          warp_y,
                          old_warp_x,
                          old_warp_y,
                          warp_to_x,
                          warp_to_y;
    Ecore_Timer          *warp_timer;

    overlay_t             move_overlays[MOVE_COUNT];
    transition_overlay_t *transition_overlay;
    Ecore_X_Window        action_input_win;
    Ecore_Event_Handler  *handler_key;
    Ecore_Timer          *action_timer;
    E_Border             *focused_bd;
    void (*action_cb)(E_Border *bd, Border_Extra *extra);

    tiling_input_mode_t  input_mode;
} tiling_mod_main_g = {
#define _G tiling_mod_main_g
    .input_mode = INPUT_MODE_NONE,
};

/* }}} */
/* Utils {{{ */

/* I wonder why noone has implemented the following one yet? */
static E_Desk *
get_current_desk(void)
{
    E_Manager *m = e_manager_current_get();
    E_Container *c = e_container_current_get(m);
    E_Zone *z = e_zone_current_get(c);

    return e_desk_current_get(z);
}

static Tiling_Info *
_initialize_tinfo(const E_Desk *desk)
{
    Tiling_Info *tinfo;

    tinfo = E_NEW(Tiling_Info, 1);
    tinfo->desk = desk;
    eina_hash_direct_add(_G.info_hash, &tinfo->desk, tinfo);

    tinfo->conf = get_vdesk(tiling_g.config->vdesks, desk->x, desk->y,
                            desk->zone->num);

    return tinfo;
}

static void
check_tinfo(const E_Desk *desk)
{
    if (!_G.tinfo || _G.tinfo->desk != desk) {
        _G.tinfo = eina_hash_find(_G.info_hash, &desk);
        if (!_G.tinfo) {
            /* lazy init */
            _G.tinfo = _initialize_tinfo(desk);
        }
        if (!_G.tinfo->conf) {
            _G.tinfo->conf = get_vdesk(tiling_g.config->vdesks,
                                       desk->x, desk->y,
                                       desk->zone->num);
        }
    }
}

static int
is_floating_window(const E_Border *bd)
{
    return EINA_LIST_IS_IN(_G.tinfo->floating_windows, bd);
}

static int
is_untilable_dialog(const E_Border *bd)
{
    return (!tiling_g.config->tile_dialogs
            && ((bd->client.icccm.transient_for != 0)
                || (bd->client.netwm.type == ECORE_X_WINDOW_TYPE_DIALOG)));
}

static void
change_window_border(E_Border   *bd,
                     const char *bordername)
{
    eina_stringshare_replace(&bd->bordername, bordername);
    bd->client.border.changed = true;
    bd->changed = true;
}

static int
get_column(const E_Border *bd)
{
    for (int i = 0; i < TILING_MAX_COLUMNS; i++) {
        if (EINA_LIST_IS_IN(_G.tinfo->columns[i], bd))
            return i;
    }
    return -1;
}

static int
get_column_count(void)
{
    for (int i = 0; i < TILING_MAX_COLUMNS; i++) {
        if (!_G.tinfo->columns[i])
            return i;
    }
    return TILING_MAX_COLUMNS;
}

static int
get_window_count(void)
{
    int res = 0;

    for (int i = 0; i < TILING_MAX_COLUMNS; i++) {
        if (!_G.tinfo->columns[i])
            break;
        res += eina_list_count(_G.tinfo->columns[i]);
    }
    return res;
}

static void
_theme_edje_object_set_aux(Evas_Object *obj, const char *group)
{
    if (!e_theme_edje_object_set(obj, "base/theme/modules/e-tiling",
                                 group)) {
        edje_object_file_set(obj, _G.edj_path, group);
    }
}
#define _theme_edje_object_set(_obj, _group)                                 \
    if (e_config->use_composite)                                             \
        _theme_edje_object_set_aux(_obj, _group"/composite");                \
    else                                                                     \
        _theme_edje_object_set_aux(_obj, _group);

static Eina_Bool
_info_hash_update(const Eina_Hash *hash, const void *key,
                  void *data, void *fdata)
{
    Tiling_Info *tinfo = data;

    if (tinfo->desk) {
        tinfo->conf = get_vdesk(tiling_g.config->vdesks,
                                tinfo->desk->x, tinfo->desk->y,
                                tinfo->desk->zone->num);
    } else {
        tinfo->conf = NULL;
    }

    return true;
}

void
e_tiling_update_conf(void)
{
    eina_hash_foreach(_G.info_hash, _info_hash_update, NULL);
}

/* }}} */
/* Overlays {{{*/

static void
_overlays_free_cb(void *data)
{
    Border_Extra *extra = data;

    if (extra->overlay.obj) {
        evas_object_del(extra->overlay.obj);
        extra->overlay.obj = NULL;
    }
    if (extra->overlay.popup) {
        e_object_del(E_OBJECT(extra->overlay.popup));
        extra->overlay.popup = NULL;
    }

    extra->key[0] = '\0';
}

static void
end_special_input(void)
{
    if (_G.input_mode == INPUT_MODE_NONE)
        return;

    if (_G.overlays) {
        eina_hash_free(_G.overlays);
        _G.overlays = NULL;
    }

    if (_G.handler_key) {
        ecore_event_handler_del(_G.handler_key);
        _G.handler_key = NULL;
    }
    if (_G.action_input_win) {
        e_grabinput_release(_G.action_input_win, _G.action_input_win);
        ecore_x_window_free(_G.action_input_win);
        _G.action_input_win = 0;
    }
    if (_G.action_timer) {
        ecore_timer_del(_G.action_timer);
        _G.action_timer = NULL;
    }

    _G.focused_bd = NULL;
    _G.action_cb = NULL;

    switch(_G.input_mode) {
      case INPUT_MODE_MOVING:
        for (int i = 0; i < MOVE_COUNT; i++) {
            overlay_t *overlay = &_G.move_overlays[i];

            if (overlay->obj) {
                evas_object_del(overlay->obj);
                overlay->obj = NULL;
            }
            if (overlay->popup) {
                e_object_del(E_OBJECT(overlay->popup));
                overlay->popup = NULL;
            }
        }
        break;
      case INPUT_MODE_TRANSITION:
        if (_G.transition_overlay) {
            if (_G.transition_overlay->overlay.obj) {
                evas_object_del(_G.transition_overlay->overlay.obj);
            }
            if (_G.transition_overlay->overlay.popup) {
                e_object_del(E_OBJECT(_G.transition_overlay->overlay.popup));
            }
            E_FREE(_G.transition_overlay);
            _G.transition_overlay = NULL;
        }
        break;
      default:
        break;
    }

    _G.input_mode = INPUT_MODE_NONE;
}

static Eina_Bool
overlay_key_down(void *data,
          int type,
          void *event)
{
    Ecore_Event_Key *ev = event;
    Border_Extra *extra;

    if (ev->event_window != _G.action_input_win)
        return ECORE_CALLBACK_PASS_ON;

    if (strcmp(ev->key, "Return") == 0)
        goto stop;
    if (strcmp(ev->key, "Escape") == 0)
        goto stop;

    extra = eina_hash_find(_G.overlays, ev->key);
    if (extra) {
        _G.action_cb(_G.focused_bd, extra);
    }

stop:
    end_special_input();
    return ECORE_CALLBACK_DONE;
}

static Eina_Bool
_timeout_cb(void *data)
{
    end_special_input();
    return ECORE_CALLBACK_CANCEL;
}

static void
_do_overlay(E_Border *focused_bd,
            void (*action_cb)(E_Border *, Border_Extra *),
            tiling_input_mode_t input_mode)
{
    int nb_win;
    const char *keys = TILING_KEYS;
    const char *c = keys;
    Ecore_X_Window parent;

    end_special_input();

    nb_win = get_window_count();
    if (nb_win < 2) {
        return;
    }

    _G.input_mode = input_mode;

    _G.focused_bd = focused_bd;
    _G.action_cb = action_cb;

    _G.overlays = eina_hash_string_small_new(_overlays_free_cb);

    for (int i = 0; i < TILING_MAX_COLUMNS; i++) {
        Eina_List *l;
        E_Border *bd;

        if (!_G.tinfo->columns[i])
            break;
        EINA_LIST_FOREACH(_G.tinfo->columns[i], l, bd) {
            if (bd != focused_bd && *c) {
                Border_Extra *extra;
                Evas_Coord ew, eh;

                extra = eina_hash_find(_G.border_extras, &bd);
                if (!extra) {
                    ERR("No extra for %p", bd);
                    continue;
                }

                extra->overlay.popup = e_popup_new(bd->zone, 0, 0, 1, 1);
                if (!extra->overlay.popup)
                    continue;

                e_popup_layer_set(extra->overlay.popup, TILING_POPUP_LAYER);
                extra->overlay.obj =
                    edje_object_add(extra->overlay.popup->evas);
                e_theme_edje_object_set(extra->overlay.obj,
                                        "base/theme/borders",
                                        "e/widgets/border/default/resize");

                extra->key[0] = *c;
                extra->key[1] = '\0';
                c++;

                eina_hash_add(_G.overlays, extra->key, extra);
                edje_object_part_text_set(extra->overlay.obj,
                                          "e.text.label",
                                          extra->key);
                edje_object_size_min_calc(extra->overlay.obj, &ew, &eh);
                evas_object_move(extra->overlay.obj, 0, 0);
                evas_object_resize(extra->overlay.obj, ew, eh);
                evas_object_show(extra->overlay.obj);
                e_popup_edje_bg_object_set(extra->overlay.popup,
                                           extra->overlay.obj);

                evas_object_show(extra->overlay.obj);
                e_popup_show(extra->overlay.popup);

                e_popup_move_resize(extra->overlay.popup,
                                    (bd->x - extra->overlay.popup->zone->x) +
                                    ((bd->w - ew) / 2),
                                    (bd->y - extra->overlay.popup->zone->y) +
                                    ((bd->h - eh) / 2),
                                    ew, eh);

                e_popup_show(extra->overlay.popup);
            }
        }
    }

    /* Get input */
    parent = _G.tinfo->desk->zone->container->win;
    _G.action_input_win = ecore_x_window_input_new(parent, 0, 0, 1, 1);
    if (!_G.action_input_win) {
        end_special_input();
        return;
    }

    ecore_x_window_show(_G.action_input_win);
    if (!e_grabinput_get(_G.action_input_win, 0, _G.action_input_win)) {
        end_special_input();
        return;
    }
    _G.action_timer = ecore_timer_add(TILING_OVERLAY_TIMEOUT,
                                      _timeout_cb, NULL);

    _G.handler_key = ecore_event_handler_add(ECORE_EVENT_KEY_DOWN,
                                             overlay_key_down, NULL);
}

/* }}} */
/* Reorganize columns {{{*/

static void
_reorganize_column(int col)
{
    if (col < 0 || col >= TILING_MAX_COLUMNS
        || !_G.tinfo->columns[col])
        return;

    if (_G.tinfo->columns[col]->next) {
        int zx, zy, zw, zh, x, w, h, ch, i = 0, count;

        e_zone_useful_geometry_get(_G.tinfo->desk->zone, &zx, &zy, &zw, &zh);

        count = eina_list_count(_G.tinfo->columns[col]);

        x = _G.tinfo->x[col];
        ch = 0;
        w = _G.tinfo->w[col];
        h = zh / count;

        for (Eina_List *l = _G.tinfo->columns[col]; l; l = l->next, i++) {
            E_Border *bd = l->data;
            Border_Extra *extra;
            int d = (i * 2 * zh) % count
                - (2 * ch) % count;

            extra = eina_hash_find(_G.border_extras, &bd);
            if (!extra) {
                ERR("No extra for %p", bd);
                continue;
            }

            if ((bd->maximized & E_MAXIMIZE_VERTICAL) && count != 1) {
                e_border_unmaximize(bd, E_MAXIMIZE_VERTICAL);
            }
            /* let's use a bresenham here */

            extra->expected.x = x;
            extra->expected.y = ch + zy;
            extra->expected.w = w;
            extra->expected.h = h + d;
            ch += extra->expected.h;

            e_border_move_resize(bd,
                                 extra->expected.x,
                                 extra->expected.y,
                                 extra->expected.w,
                                 extra->expected.h);
        }
    } else {
        int y, h;
        Border_Extra *extra;
        E_Border *bd = _G.tinfo->columns[col]->data;

        extra = eina_hash_find(_G.border_extras, &bd);
        if (!extra) {
            ERR("No extra for %p", bd);
            return;
        }

        e_zone_useful_geometry_get(_G.tinfo->desk->zone, NULL, &y, NULL, &h);

        extra->expected.x = _G.tinfo->x[col];
        extra->expected.y = y;
        extra->expected.w = _G.tinfo->w[col];
        extra->expected.h = h;

        e_border_move_resize(bd,
                             extra->expected.x,
                             extra->expected.y,
                             extra->expected.w,
                             extra->expected.h);

        e_border_maximize(bd, E_MAXIMIZE_EXPAND | E_MAXIMIZE_VERTICAL);
    }
}

static void
_move_resize_column(int col, int delta_x, int delta_w)
{
    Eina_List *list = _G.tinfo->columns[col];

    for (Eina_List *l = list; l; l = l->next) {
        E_Border *bd = l->data;
        Border_Extra *extra;

        extra = eina_hash_find(_G.border_extras, &bd);
        if (!extra) {
            ERR("No extra for %p", bd);
            continue;
        }

        extra->expected.x += delta_x;
        extra->expected.w += delta_w;

        e_border_move_resize(bd, extra->expected.x,
                                 extra->expected.y,
                                 extra->expected.w,
                                 extra->expected.h);
    }

    _G.tinfo->x[col] += delta_x;
    _G.tinfo->w[col] += delta_w;
}

static void
_set_column_geometry(int col, int x, int w)
{
    for (Eina_List *l = _G.tinfo->columns[col]; l; l = l->next) {
        E_Border *bd = l->data;
        Border_Extra *extra;

        extra = eina_hash_find(_G.border_extras, &bd);
        if (!extra) {
            ERR("No extra for %p", bd);
            continue;
        }

        extra->expected.x = x;
        extra->expected.w = w;

        if (bd->maximized & E_MAXIMIZE_VERTICAL) {
            e_border_unmaximize(bd, E_MAXIMIZE_HORIZONTAL);
        }

        e_border_move_resize(bd, extra->expected.x,
                                 extra->expected.y,
                                 extra->expected.w,
                                 extra->expected.h);
    }
    _G.tinfo->x[col] = x;
    _G.tinfo->w[col] = w;
}

static void
_add_column(void)
{
    if (_G.tinfo->conf->nb_cols == TILING_MAX_COLUMNS)
        return;

    _G.tinfo->conf->nb_cols++;

    if (_G.tinfo->conf->nb_cols == 1) {
        for (Eina_List *l = e_border_focus_stack_get(); l; l = l->next) {
            E_Border *bd;

            bd = l->data;
            if (bd->desk == _G.tinfo->desk)
                _add_border(bd);
        }
    }
    if (_G.tinfo->columns[_G.tinfo->conf->nb_cols - 2]
    &&  _G.tinfo->borders >= _G.tinfo->conf->nb_cols)
    {
        int nb_cols = _G.tinfo->conf->nb_cols - 1;
        int x, y, w, h;
        int width = 0;
        /* Add column */

        e_zone_useful_geometry_get(_G.tinfo->desk->zone, &x, &y, &w, &h);

        for (int i = 0; i <= nb_cols; i++) {

            width = w / (nb_cols + 1 - i);

            _set_column_geometry(i, x, width);

            w -= width;
            x += width;
        }
        for (int i = nb_cols - 1; i >= 0; i--) {
            if (eina_list_count(_G.tinfo->columns[i]) == 1) {
                _G.tinfo->columns[i+1] = _G.tinfo->columns[i];
                _reorganize_column(i+1);
            } else {
                E_Border *bd = eina_list_last(_G.tinfo->columns[i])->data;

                EINA_LIST_REMOVE(_G.tinfo->columns[i], bd);
                _reorganize_column(i);

                _G.tinfo->columns[i+1] = NULL;
                EINA_LIST_APPEND(_G.tinfo->columns[i+1], bd);
                _reorganize_column(i+1);
                return;
            }
        }
    }
}

static void
_remove_column(void)
{
    if (!_G.tinfo->conf->nb_cols)
        return;

    _G.tinfo->conf->nb_cols--;

    if (!_G.tinfo->conf->nb_cols) {
        for (int i = 0; i < TILING_MAX_COLUMNS; i++) {
            for (Eina_List *l = _G.tinfo->columns[i]; l; l = l->next) {
                E_Border *bd = l->data;
                Border_Extra *extra;

                extra = eina_hash_find(_G.border_extras, &bd);
                if (!extra) {
                    ERR("No extra for %p", bd);
                    continue;
                }
                e_border_move_resize(bd, extra->orig.x,
                                         extra->orig.y,
                                         extra->orig.w,
                                         extra->orig.h);
            }
            eina_list_free(_G.tinfo->columns[i]);
            _G.tinfo->columns[i] = NULL;
        }
        e_place_zone_region_smart_cleanup(_G.tinfo->desk->zone);
    } else {
        int nb_cols = _G.tinfo->conf->nb_cols;
        int x, y, w, h;
        int width = 0;
        int col = _G.tinfo->conf->nb_cols;

        if (_G.tinfo->columns[col]) {
            _G.tinfo->columns[col-1] = eina_list_merge(
                _G.tinfo->columns[col-1], _G.tinfo->columns[col]);
            _reorganize_column(col-1);
        }

        e_zone_useful_geometry_get(_G.tinfo->desk->zone, &x, &y, &w, &h);
        for (int i = 0; i < nb_cols; i++) {

            width = w / (nb_cols - i);

            _set_column_geometry(i, x, width);

            w -= width;
            x += width;
        }
    }
}

void
change_column_number(struct _Config_vdesk *newconf)
{
    E_Manager *m;
    E_Container *c;
    E_Zone *z;
    E_Desk *d;
    int old_nb_cols = 0,
        new_nb_cols = newconf->nb_cols;

    m = e_manager_current_get();
    if (!m) return;
    c = e_container_current_get(m);
    if (!c) return;
    z = e_container_zone_number_get(c, newconf->zone_num);
    if (!z) return;
    d = e_desk_at_xy_get(z, newconf->x, newconf->y);
    if (!d) return;

    check_tinfo(d);
    if (_G.tinfo->conf) {
        old_nb_cols = _G.tinfo->conf->nb_cols;
    } else {
        newconf->nb_cols = 0;
    }
    _G.tinfo->conf = newconf;
    _G.tinfo->conf->nb_cols = old_nb_cols;

    if (new_nb_cols == old_nb_cols)
        return;

    if (new_nb_cols == 0) {
        for (int i = 0; i < TILING_MAX_COLUMNS; i++) {
            for (Eina_List *l = _G.tinfo->columns[i]; l; l = l->next) {
                E_Border *bd = l->data;
                Border_Extra *extra;

                extra = eina_hash_find(_G.border_extras, &bd);
                if (!extra) {
                    ERR("No extra for %p", bd);
                    continue;
                }
                e_border_move_resize(bd, extra->orig.x,
                                         extra->orig.y,
                                         extra->orig.w,
                                         extra->orig.h);
                if (!tiling_g.config->show_titles)
                    change_window_border(bd, "default");
            }
            eina_list_free(_G.tinfo->columns[i]);
            _G.tinfo->columns[i] = NULL;
        }
        e_place_zone_region_smart_cleanup(z);
    } else if (new_nb_cols > old_nb_cols) {
        for (int i = new_nb_cols; i > old_nb_cols; i--) {
            _add_column();
        }
    } else {
        for (int i = new_nb_cols; i < old_nb_cols; i++) {
            _remove_column();
        }
    }
    _G.tinfo->conf->nb_cols = new_nb_cols;
}

static void
_e_mod_action_add_column_cb(E_Object   *obj,
                            const char *params)
{
    E_Desk *desk = get_current_desk();

    end_special_input();

    check_tinfo(desk);

    _add_column();

    e_config_save_queue();
}

static void
_e_mod_action_remove_column_cb(E_Object   *obj,
                               const char *params)
{
    E_Desk *desk = get_current_desk();

    end_special_input();

    check_tinfo(desk);

    _remove_column();

    e_config_save_queue();
}

/* }}} */
/* Reorganize windows {{{*/

static void
_add_border(E_Border *bd)
{
    Border_Extra *extra;
    int col;

    if (!bd) {
        return;
    }
    if (is_floating_window(bd)) {
        return;
    }
    if (is_untilable_dialog(bd)) {
        return;
    }
    if (bd->fullscreen) {
         return;
    }

    if (!_G.tinfo->conf || !_G.tinfo->conf->nb_cols) {
        return;
    }

    extra = E_NEW(Border_Extra, 1);
    *extra = (Border_Extra) {
        .border = bd,
        .expected = {
            .x = bd->x,
            .y = bd->y,
            .w = bd->w,
            .h = bd->h,
        },
        .orig = {
            .x = bd->x,
            .y = bd->y,
            .w = bd->w,
            .h = bd->h,
        },
    };

    eina_hash_direct_add(_G.border_extras, &extra->border, extra);

    /* New Border! */

    if (!tiling_g.config->show_titles
        && ((bd->bordername && strcmp(bd->bordername, "pixel"))
            ||  !bd->bordername))
    {
        change_window_border(bd, "pixel");
    }

    if (_G.tinfo->columns[0]) {
        if (_G.tinfo->columns[_G.tinfo->conf->nb_cols - 1]) {
            col = _G.tinfo->conf->nb_cols - 1;

            if (!_G.tinfo->columns[col]->next) {
                e_border_unmaximize(_G.tinfo->columns[col]->data,
                                    E_MAXIMIZE_BOTH);
            }
            EINA_LIST_APPEND(_G.tinfo->columns[col], bd);
            _reorganize_column(col);
            e_border_unmaximize(bd, E_MAXIMIZE_BOTH);
        } else {
            /* Add column */
            int nb_cols = get_column_count();
            int x, y, w, h;
            int width = 0;

            e_zone_useful_geometry_get(bd->zone, &x, &y, &w, &h);

            for (int i = 0; i < nb_cols; i++) {

                width = w / (nb_cols + 1 - i);

                _set_column_geometry(i, x, width);

                w -= width;
                x += width;
            }

            _G.tinfo->x[nb_cols] = x;
            _G.tinfo->w[nb_cols] = width;
            extra->expected.x = x;
            extra->expected.y = y;
            extra->expected.w = width;
            extra->expected.h = h;
            e_border_move_resize(bd,
                                 extra->expected.x,
                                 extra->expected.y,
                                 extra->expected.w,
                                 extra->expected.h);
            e_border_maximize(bd, E_MAXIMIZE_EXPAND | E_MAXIMIZE_VERTICAL);

            EINA_LIST_APPEND(_G.tinfo->columns[nb_cols], bd);
            col = nb_cols;
        }
    } else {
        e_border_unmaximize(bd, E_MAXIMIZE_BOTH);
        e_border_maximize(bd, E_MAXIMIZE_EXPAND | E_MAXIMIZE_BOTH);
        EINA_LIST_APPEND(_G.tinfo->columns[0], bd);
        e_zone_useful_geometry_get(bd->zone,
                                   &_G.tinfo->x[0], NULL,
                                   &_G.tinfo->w[0], NULL);
        col = 0;
    }
    _G.tinfo->borders++;

}

static void
_remove_border(E_Border *bd)
{
    int col;
    int nb_cols;

    nb_cols = get_column_count();

    col = get_column(bd);
    if (col < 0)
        return;

    _G.tinfo->borders--;
    EINA_LIST_REMOVE(_G.tinfo->columns[col], bd);
    eina_hash_del(_G.border_extras, bd, NULL);

    if (_G.tinfo->columns[col]) {
        _reorganize_column(col);
    } else {
        if (nb_cols > _G.tinfo->borders) {
            int x, y, w, h;
            int width = 0;
            /* Remove column */

            nb_cols--;

            e_zone_useful_geometry_get(bd->zone, &x, &y, &w, &h);

            for (int i = col; i < nb_cols; i++) {
                _G.tinfo->columns[i] = _G.tinfo->columns[i+1];
            }
            _G.tinfo->columns[nb_cols] = NULL;
            for (int i = 0; i < nb_cols; i++) {

                width = w / (nb_cols - i);

                _set_column_geometry(i, x, width);

                w -= width;
                x += width;
            }
        } else {
            for (int i = col+1; i < nb_cols; i++) {
                if (eina_list_count(_G.tinfo->columns[i]) > 1) {
                    for (int j = col; j < i - 1; j++) {
                        _G.tinfo->columns[j] = _G.tinfo->columns[j+1];
                        _reorganize_column(j);
                    }
                    bd = _G.tinfo->columns[i]->data;
                    EINA_LIST_REMOVE(_G.tinfo->columns[i], bd);
                    _reorganize_column(i);

                    _G.tinfo->columns[i-1] = NULL;
                    EINA_LIST_APPEND(_G.tinfo->columns[i-1], bd);
                    _reorganize_column(i-1);
                    return;
                }
            }
            for (int i = col-1; i >= 0; i--) {
                if (eina_list_count(_G.tinfo->columns[i]) == 1) {
                    _G.tinfo->columns[i+1] = _G.tinfo->columns[i];
                    _reorganize_column(i+1);
                } else {
                    bd = eina_list_last(_G.tinfo->columns[i])->data;
                    EINA_LIST_REMOVE(_G.tinfo->columns[i], bd);
                    _reorganize_column(i);

                    _G.tinfo->columns[i+1] = NULL;
                    EINA_LIST_APPEND(_G.tinfo->columns[i+1], bd);
                    _reorganize_column(i+1);
                    return;
                }
            }
        }
    }
}

static void
_move_resize_border_column(E_Border *bd, Border_Extra *extra,
                           int col, tiling_change_t change)
{
    if (change == TILING_RESIZE) {
        if (col == TILING_MAX_COLUMNS || !_G.tinfo->columns[col + 1]) {
            /* You're not allowed to resize */
            bd->w = extra->expected.w;
        } else {
            int delta = bd->w - extra->expected.w;

            if (delta + 1 > _G.tinfo->w[col + 1])
                delta = _G.tinfo->w[col + 1] - 1;

            _move_resize_column(col, 0, delta);
            _move_resize_column(col+1, delta, -delta);
            extra->expected.w = bd->w;
        }
    } else {
        if (col == 0) {
            /* You're not allowed to move */
            bd->x = extra->expected.x;
        } else {
            int delta = bd->x - extra->expected.x;

            if (delta + 1 > _G.tinfo->w[col - 1])
                delta = _G.tinfo->w[col - 1] - 1;

            _move_resize_column(col, delta, -delta);
            _move_resize_column(col-1, 0, delta);
            extra->expected.x = bd->x;
        }
    }
}

static void
_move_resize_border_in_column(E_Border *bd, Border_Extra *extra,
                              int col, tiling_change_t change)
{
    Eina_List *l;

    l = eina_list_data_find_list(_G.tinfo->columns[col], bd);
    if (!l) {
        ERR("unable to bd %p in column %d", bd, col);
        return;
    }

    switch (change) {
      case TILING_RESIZE:
        if (!l->next) {
            if (l->prev) {
                int delta = bd->h - extra->expected.h;
                E_Border *prevbd = l->prev->data;
                Border_Extra *prevextra;

                prevextra = eina_hash_find(_G.border_extras, &prevbd);
                if (!prevextra) {
                    ERR("No extra for %p", prevbd);
                    return;
                }

                prevextra->expected.h -= delta;
                e_border_resize(prevbd,
                                prevextra->expected.w,
                                prevextra->expected.h);

                extra->expected.y -= delta;
                extra->expected.h = bd->h;

                e_border_move(bd,
                              extra->expected.x,
                              extra->expected.y);
            } else {
                /* You're not allowed to resize */
                e_border_resize(bd,
                                extra->expected.w,
                                extra->expected.h);
            }
        } else {
            int delta = bd->h - extra->expected.h;
            E_Border *nextbd = l->next->data;
            Border_Extra *nextextra;
            int min_height = MAX(nextbd->client.icccm.base_h, 1);

            nextextra = eina_hash_find(_G.border_extras, &nextbd);
            if (!nextextra) {
                ERR("No extra for %p", nextbd);
                return;
            }

            if (nextextra->expected.h - delta < min_height)
                delta = nextextra->expected.h - min_height;

            nextextra->expected.y += delta;
            nextextra->expected.h -= delta;
            e_border_move_resize(nextbd,
                                 nextextra->expected.x,
                                 nextextra->expected.y,
                                 nextextra->expected.w,
                                 nextextra->expected.h);

            extra->expected.h += delta;
            e_border_move_resize(bd,
                                 extra->expected.x,
                                 extra->expected.y,
                                 extra->expected.w,
                                 extra->expected.h);
        }
        break;
      case TILING_MOVE:
        if (!l->prev) {
            /* You're not allowed to move */
            bd->y = extra->expected.y;
            e_border_move(bd,
                          extra->expected.x,
                          extra->expected.y);
            DBG("trying to move %p, but !l->prev", bd);
        } else {
            int delta = bd->y - extra->expected.y;
            E_Border *prevbd = l->prev->data;
            Border_Extra *prevextra;
            int min_height = MAX(prevbd->client.icccm.base_h, 1);

            prevextra = eina_hash_find(_G.border_extras, &prevbd);
            if (!prevextra) {
                ERR("No extra for %p", prevbd);
                return;
            }

            if (prevextra->expected.h - delta < min_height)
                delta = prevextra->expected.h - min_height;

            prevextra->expected.h += delta;
            e_border_resize(prevbd,
                            prevextra->expected.w,
                            prevextra->expected.h);

            extra->expected.y += delta;
            extra->expected.h -= delta;

            e_border_move_resize(bd,
                                 extra->expected.x,
                                 extra->expected.y,
                                 extra->expected.w,
                                 extra->expected.h);
        }
        break;
      default:
        ERR("invalid tiling change: %d", change);
    }
}

/* }}} */
/* Toggle Floating {{{ */

static void
toggle_floating(E_Border *bd)
{
    if (!bd || !_G.tinfo)
        return;

    check_tinfo(bd->desk);

    if (EINA_LIST_IS_IN(_G.tinfo->floating_windows, bd)) {
        EINA_LIST_REMOVE(_G.tinfo->floating_windows, bd);

        _add_border(bd);
    } else {
        Border_Extra *extra;

        extra = eina_hash_find(_G.border_extras, &bd);
        if (extra) {
            e_border_move_resize(bd,
                                 extra->orig.x,
                                 extra->orig.y,
                                 extra->orig.w,
                                 extra->orig.h);
            e_border_unmaximize(bd, E_MAXIMIZE_BOTH);
        } else {
            e_border_maximize(bd, E_MAXIMIZE_EXPAND | E_MAXIMIZE_BOTH);
        }
        EINA_LIST_APPEND(_G.tinfo->floating_windows, bd);

        _remove_border(bd);

        /* To give the user a bit of feedback we restore the original border */
        /* TODO: save the original border, don't just restore the default one*/
        /* TODO: save maximized state */
        if (!tiling_g.config->show_titles)
            change_window_border(bd, "default");
    }
}

static void
_e_mod_action_toggle_floating_cb(E_Object   *obj,
                                 const char *params)
{
    end_special_input();

    toggle_floating(e_border_focused_get());
}

/* }}} */
/* {{{ Swap */

static void
_action_swap(E_Border *bd_1,
             Border_Extra *extra_2)
{
    Border_Extra *extra_1;
    E_Border *bd_2 = extra_2->border;
    Eina_List *l_1 = NULL,
              *l_2 = NULL;
    geom_t gt;
    unsigned int bd_2_maximized;

    extra_1 = eina_hash_find(_G.border_extras, &bd_1);
    if (!extra_1) {
        ERR("No extra for %p", bd_1);
        return;
    }

    for (int i = 0; i < TILING_MAX_COLUMNS; i++) {
        if ((l_1 = eina_list_data_find_list(_G.tinfo->columns[i], bd_1))) {
            break;
        }
    }
    for (int i = 0; i < TILING_MAX_COLUMNS; i++) {
        if ((l_2 = eina_list_data_find_list(_G.tinfo->columns[i], bd_2))) {
            break;
        }
    }

    if (!l_1 || !l_2) {
        return;
    }

    l_1->data = bd_2;
    l_2->data = bd_1;

    gt = extra_2->expected;
    extra_2->expected = extra_1->expected;
    extra_1->expected = gt;

    bd_2_maximized = bd_2->maximized;
    if (bd_2->maximized)
        e_border_unmaximize(bd_2, E_MAXIMIZE_BOTH);
    if (bd_1->maximized) {
        e_border_unmaximize(bd_1, E_MAXIMIZE_BOTH);
        e_border_maximize(bd_2, bd_1->maximized);
    }
    if (bd_2_maximized) {
        e_border_maximize(bd_1, bd_2_maximized);
    }
    e_border_move_resize(bd_1,
                         extra_1->expected.x,
                         extra_1->expected.y,
                         extra_1->expected.w,
                         extra_1->expected.h);
    e_border_move_resize(bd_2,
                         extra_2->expected.x,
                         extra_2->expected.y,
                         extra_2->expected.w,
                         extra_2->expected.h);
}

static void
_e_mod_action_swap_cb(E_Object   *obj,
                      const char *params)
{
    E_Desk *desk;
    E_Border *focused_bd;

    desk = get_current_desk();
    if (!desk)
        return;

    focused_bd = e_border_focused_get();
    if (!focused_bd || focused_bd->desk != desk)
        return;

    check_tinfo(desk);

    if (!_G.tinfo->conf || !_G.tinfo->conf->nb_cols) {
        return;
    }

    _do_overlay(focused_bd, _action_swap, INPUT_MODE_SWAPPING);
}

/* }}} */
/* Move {{{*/

static void
_check_moving_anims(E_Border *bd, Border_Extra *extra, int col)
{
    Eina_List *l = NULL;
    overlay_t *overlay;
    int nb_cols = get_column_count();

    if (col < 0) {
        col = get_column(bd);
        if (col < 0)
            return;
    }
    if (!extra) {
        extra = eina_hash_find(_G.border_extras, &bd);
        if (!extra) {
            ERR("No extra for %p", bd);
            return;
        }
    }
    l = eina_list_data_find_list(_G.tinfo->columns[col], bd);
    if (!l)
        return;

    /* move left */
    overlay = &_G.move_overlays[MOVE_LEFT];
    if (col > 0) {
        if (overlay->popup) {
            Evas_Coord ew, eh;

            edje_object_size_min_calc(overlay->obj, &ew, &eh);
            e_popup_move(_G.move_overlays[MOVE_LEFT].popup,
                         extra->expected.x - ew/2,
                         extra->expected.y + extra->expected.h/2 - eh/2);
        } else {
            Evas_Coord ew, eh;

            overlay->popup = e_popup_new(bd->zone, 0, 0, 1, 1);
            if (!overlay->popup)
                return;

            e_popup_layer_set(overlay->popup, TILING_POPUP_LAYER);
            overlay->obj = edje_object_add(overlay->popup->evas);
            _theme_edje_object_set(overlay->obj,
                                   "modules/e-tiling/move/left");
            edje_object_size_min_calc(overlay->obj, &ew, &eh);
            e_popup_edje_bg_object_set(overlay->popup,
                                       overlay->obj);
            evas_object_show(overlay->obj);
            e_popup_move_resize(overlay->popup,
                                extra->expected.x - ew/2
                                                  - overlay->popup->zone->x,
                                extra->expected.y + extra->expected.h/2
                                                  - eh/2
                                                  - overlay->popup->zone->y,
                                ew,
                                eh);
            evas_object_resize(overlay->obj, ew, eh);

            e_popup_show(overlay->popup);
        }
    } else if (overlay->popup) {
        if (overlay->obj) {
            evas_object_del(overlay->obj);
            overlay->obj = NULL;
        }
        if (overlay->popup) {
            e_object_del(E_OBJECT(overlay->popup));
            overlay->popup = NULL;
        }
    }

    /* move right */
    overlay = &_G.move_overlays[MOVE_RIGHT];
    if (col != TILING_MAX_COLUMNS - 1
    && ((col == nb_cols - 1 && _G.tinfo->columns[col]->next)
        || (col != nb_cols - 1))) {
        if (overlay->popup) {
            Evas_Coord ew, eh;

            edje_object_size_min_calc(overlay->obj, &ew, &eh);
            e_popup_move(_G.move_overlays[MOVE_RIGHT].popup,
                         extra->expected.x + extra->expected.w - ew/2,
                         extra->expected.y + extra->expected.h/2 - eh/2);
        } else {
            Evas_Coord ew, eh;

            overlay->popup = e_popup_new(bd->zone, 0, 0, 1, 1);
            if (!overlay->popup)
                return;

            e_popup_layer_set(overlay->popup, TILING_POPUP_LAYER);
            overlay->obj = edje_object_add(overlay->popup->evas);
            _theme_edje_object_set(overlay->obj,
                                   "modules/e-tiling/move/right");
            edje_object_size_min_calc(overlay->obj, &ew, &eh);
            e_popup_edje_bg_object_set(overlay->popup,
                                       overlay->obj);
            evas_object_show(overlay->obj);
            e_popup_move_resize(overlay->popup,
                                extra->expected.x + extra->expected.w - ew/2
                                                  - overlay->popup->zone->x,
                                extra->expected.y + extra->expected.h/2
                                                  - eh/2
                                                  - overlay->popup->zone->y,
                                ew,
                                eh);
            evas_object_resize(overlay->obj, ew, eh);

            e_popup_show(overlay->popup);
        }
    } else if (overlay->popup) {
        if (overlay->obj) {
            evas_object_del(overlay->obj);
            overlay->obj = NULL;
        }
        if (overlay->popup) {
            e_object_del(E_OBJECT(overlay->popup));
            overlay->popup = NULL;
        }
    }

    /* move up */
    overlay = &_G.move_overlays[MOVE_UP];
    if (l->prev) {
        if (overlay->popup) {
            Evas_Coord ew, eh;

            edje_object_size_min_calc(overlay->obj, &ew, &eh);
            e_popup_move(_G.move_overlays[MOVE_UP].popup,
                         extra->expected.x + extra->expected.w/2 - ew/2,
                         extra->expected.y - eh/2);
        } else {
            Evas_Coord ew, eh;

            overlay->popup = e_popup_new(bd->zone, 0, 0, 1, 1);
            if (!overlay->popup)
                return;

            e_popup_layer_set(overlay->popup, TILING_POPUP_LAYER);
            overlay->obj = edje_object_add(overlay->popup->evas);
            _theme_edje_object_set(overlay->obj, "modules/e-tiling/move/up");
            edje_object_size_min_calc(overlay->obj, &ew, &eh);
            e_popup_edje_bg_object_set(overlay->popup,
                                       overlay->obj);
            evas_object_show(overlay->obj);
            e_popup_move_resize(overlay->popup,
                                extra->expected.x + extra->expected.w/2
                                                  - ew/2
                                                  - overlay->popup->zone->x,
                                extra->expected.y - eh/2
                                                  - overlay->popup->zone->y,
                                ew,
                                eh);
            evas_object_resize(overlay->obj, ew, eh);

            e_popup_show(overlay->popup);
        }
    } else if (overlay->popup) {
        if (overlay->obj) {
            evas_object_del(overlay->obj);
            overlay->obj = NULL;
        }
        if (overlay->popup) {
            e_object_del(E_OBJECT(overlay->popup));
            overlay->popup = NULL;
        }
    }

    /* move down */
    overlay = &_G.move_overlays[MOVE_DOWN];
    if (l->next) {
        if (overlay->popup) {
            Evas_Coord ew, eh;

            edje_object_size_min_calc(overlay->obj, &ew, &eh);
            e_popup_move(_G.move_overlays[MOVE_DOWN].popup,
                         extra->expected.x + extra->expected.w/2 - ew/2,
                         extra->expected.y + extra->expected.h - eh/2);
        } else {
            Evas_Coord ew, eh;

            overlay->popup = e_popup_new(bd->zone, 0, 0, 1, 1);
            if (!overlay->popup)
                return;

            e_popup_layer_set(overlay->popup, TILING_POPUP_LAYER);
            overlay->obj = edje_object_add(overlay->popup->evas);
            _theme_edje_object_set(overlay->obj,
                                   "modules/e-tiling/move/down");
            edje_object_size_min_calc(overlay->obj, &ew, &eh);
            e_popup_edje_bg_object_set(overlay->popup,
                                       overlay->obj);
            evas_object_show(overlay->obj);
            e_popup_move_resize(overlay->popup,
                                extra->expected.x + extra->expected.w/2
                                                  - ew/2
                                                  - overlay->popup->zone->x,
                                extra->expected.y + extra->expected.h - eh/2
                                                  - overlay->popup->zone->y,
                                ew,
                                eh);
            evas_object_resize(overlay->obj, ew, eh);

            e_popup_show(overlay->popup);
        }
    } else if (overlay->popup) {
        if (overlay->obj) {
            evas_object_del(overlay->obj);
            overlay->obj = NULL;
        }
        if (overlay->popup) {
            e_object_del(E_OBJECT(overlay->popup));
            overlay->popup = NULL;
        }
    }
}

static void
_move_up(void)
{
    E_Border *bd_1 = _G.focused_bd,
             *bd_2 = NULL;
    Border_Extra *extra_1 = NULL,
                 *extra_2 = NULL;
    Eina_List *l_1 = NULL,
              *l_2 = NULL;
    int col;

    col = get_column(_G.focused_bd);
    if (col < 0)
        return;

    if (_G.tinfo->columns[col]->data == _G.focused_bd)
        return;

    l_1 = eina_list_data_find_list(_G.tinfo->columns[col], bd_1);
    if (!l_1 || !l_1->prev)
        return;
    l_2 = l_1->prev;
    bd_2 = l_2->data;

    extra_1 = eina_hash_find(_G.border_extras, &bd_1);
    if (!extra_1) {
        ERR("No extra for %p", bd_1);
        return;
    }
    extra_2 = eina_hash_find(_G.border_extras, &bd_2);
    if (!extra_2) {
        ERR("No extra for %p", bd_2);
        return;
    }

    l_1->data = bd_2;
    l_2->data = bd_1;

    extra_1->expected.y = extra_2->expected.y;
    extra_2->expected.y += extra_1->expected.h;

    e_border_move(bd_1,
                  extra_1->expected.x,
                  extra_1->expected.y);
    e_border_move(bd_2,
                  extra_2->expected.x,
                  extra_2->expected.y);

    _check_moving_anims(bd_1, extra_1, col);
    ecore_x_pointer_warp(_G.tinfo->desk->zone->container->win,
                         extra_1->expected.x + extra_1->expected.w/2,
                         extra_1->expected.y + extra_1->expected.h/2);
}

static void
_move_down(void)
{
    E_Border *bd_1 = _G.focused_bd,
             *bd_2 = NULL;
    Border_Extra *extra_1 = NULL,
                 *extra_2 = NULL;
    Eina_List *l_1 = NULL,
              *l_2 = NULL;
    int col;

    col = get_column(_G.focused_bd);
    if (col < 0)
        return;

    l_1 = eina_list_data_find_list(_G.tinfo->columns[col], bd_1);
    if (!l_1 || !l_1->next)
        return;
    l_2 = l_1->next;
    bd_2 = l_2->data;

    extra_1 = eina_hash_find(_G.border_extras, &bd_1);
    if (!extra_1) {
        ERR("No extra for %p", bd_1);
        return;
    }
    extra_2 = eina_hash_find(_G.border_extras, &bd_2);
    if (!extra_2) {
        ERR("No extra for %p", bd_2);
        return;
    }

    l_1->data = bd_2;
    l_2->data = bd_1;

    extra_2->expected.y = extra_1->expected.y;
    extra_1->expected.y += extra_2->expected.h;

    e_border_move(bd_1,
                  extra_1->expected.x,
                  extra_1->expected.y);
    e_border_move(bd_2,
                  extra_2->expected.x,
                  extra_2->expected.y);

    _check_moving_anims(bd_1, extra_1, col);
    ecore_x_pointer_warp(_G.tinfo->desk->zone->container->win,
                         extra_1->expected.x + extra_1->expected.w/2,
                         extra_1->expected.y + extra_1->expected.h/2);
}

static void
_move_left(void)
{
    E_Border *bd = _G.focused_bd;
    Border_Extra *extra;
    int col;

    col = get_column(_G.focused_bd);
    if (col <= 0)
        return;


    EINA_LIST_REMOVE(_G.tinfo->columns[col], bd);
    EINA_LIST_APPEND(_G.tinfo->columns[col - 1], bd);

    if (!_G.tinfo->columns[col]) {
        int x, y, w, h;
        int width = 0;
        int nb_cols;

        /* Remove column */
        nb_cols = get_column_count();

        e_zone_useful_geometry_get(bd->zone, &x, &y, &w, &h);

        for (int i = col; i < nb_cols; i++) {
            _G.tinfo->columns[i] = _G.tinfo->columns[i+1];
        }
        _G.tinfo->columns[nb_cols] = NULL;
        for (int i = 0; i < nb_cols; i++) {

            width = w / (nb_cols - i);

            _set_column_geometry(i, x, width);

            w -= width;
            x += width;
        }
        _reorganize_column(col - 1);
    } else {
        _reorganize_column(col);
        _reorganize_column(col - 1);
    }

    extra = eina_hash_find(_G.border_extras, &bd);
    if (!extra) {
        ERR("No extra for %p", bd);
        return;
    }

    _check_moving_anims(bd, extra, col - 1);
    ecore_x_pointer_warp(_G.tinfo->desk->zone->container->win,
                         extra->expected.x + extra->expected.w/2,
                         extra->expected.y + extra->expected.h/2);
}

static void
_move_right(void)
{
    E_Border *bd = _G.focused_bd;
    int col;
    int nb_cols;
    Border_Extra *extra;

    col = get_column(bd);
    if (col == TILING_MAX_COLUMNS - 1)
        return;

    nb_cols = get_column_count();
    if (col == nb_cols - 1 && !_G.tinfo->columns[col]->next)
        return;

    extra = eina_hash_find(_G.border_extras, &bd);
    if (!extra) {
        ERR("No extra for %p", bd);
        return;
    }

    EINA_LIST_REMOVE(_G.tinfo->columns[col], bd);
    EINA_LIST_APPEND(_G.tinfo->columns[col + 1], bd);

    if (_G.tinfo->columns[col] && _G.tinfo->columns[col + 1]->next) {
        _reorganize_column(col);
        _reorganize_column(col + 1);
        _check_moving_anims(bd, extra, col + 1);
    } else
    if (_G.tinfo->columns[col]) {
        /* Add column */
        int x, y, w, h;
        int width = 0;

        _reorganize_column(col);

        e_zone_useful_geometry_get(bd->zone, &x, &y, &w, &h);

        for (int i = 0; i < nb_cols; i++) {

            width = w / (nb_cols + 1 - i);

            _set_column_geometry(i, x, width);

            w -= width;
            x += width;
        }

        _G.tinfo->x[nb_cols] = x;
        _G.tinfo->w[nb_cols] = width;
        extra->expected.x = x;
        extra->expected.y = y;
        extra->expected.w = width;
        extra->expected.h = h;
        e_border_move_resize(bd,
                             extra->expected.x,
                             extra->expected.y,
                             extra->expected.w,
                             extra->expected.h);
        e_border_maximize(bd, E_MAXIMIZE_EXPAND | E_MAXIMIZE_VERTICAL);

        if (nb_cols + 1 > _G.tinfo->conf->nb_cols) {
            _G.tinfo->conf->nb_cols = nb_cols + 1;
            e_config_save_queue();
        }
        _check_moving_anims(bd, extra, col + 1);
    } else {
        int x, y, w, h;
        int width;

        e_zone_useful_geometry_get(_G.tinfo->desk->zone, &x, &y, &w, &h);
        for (int i = col; i < nb_cols; i++) {
             _G.tinfo->columns[i] = _G.tinfo->columns[i + 1];
        }
        nb_cols--;
        for (int i = 0; i < nb_cols; i++) {
            width = w / (nb_cols - i);

            _set_column_geometry(i, x, width);

            w -= width;
            x += width;
        }
        _G.tinfo->columns[nb_cols] = NULL;
        _G.tinfo->x[nb_cols] = 0;
        _G.tinfo->w[nb_cols] = 0;
        _reorganize_column(col);
        _check_moving_anims(bd, extra, col);
    }

    ecore_x_pointer_warp(_G.tinfo->desk->zone->container->win,
                         extra->expected.x + extra->expected.w/2,
                         extra->expected.y + extra->expected.h/2);
}

static Eina_Bool
move_key_down(void *data,
              int type,
              void *event)
{
    Ecore_Event_Key *ev = event;

    if (ev->event_window != _G.action_input_win)
        return ECORE_CALLBACK_PASS_ON;

    /* reset timer */
    ecore_timer_delay(_G.action_timer, TILING_OVERLAY_TIMEOUT
                      - ecore_timer_pending_get(_G.action_timer));

    if ((strcmp(ev->key, "Up") == 0)
    ||  (strcmp(ev->key, "k") == 0))
    {
        _move_up();
        return ECORE_CALLBACK_PASS_ON;
    } else if ((strcmp(ev->key, "Down") == 0)
           ||  (strcmp(ev->key, "j") == 0))
    {
        _move_down();
        return ECORE_CALLBACK_PASS_ON;
    } else if ((strcmp(ev->key, "Left") == 0)
           ||  (strcmp(ev->key, "h") == 0))
    {
        _move_left();
        return ECORE_CALLBACK_PASS_ON;
    } else if ((strcmp(ev->key, "Right") == 0)
           ||  (strcmp(ev->key, "l") == 0))
    {
        _move_right();
        return ECORE_CALLBACK_PASS_ON;
    }

    if (strcmp(ev->key, "Return") == 0)
        goto stop;
    if (strcmp(ev->key, "Escape") == 0)
        goto stop; /* TODO: fallback */

    return ECORE_CALLBACK_PASS_ON;
stop:
    end_special_input();
    return ECORE_CALLBACK_DONE;
}

static void
_e_mod_action_move_cb(E_Object   *obj,
                      const char *params)
{
    E_Desk *desk;
    E_Border *focused_bd;
    Ecore_X_Window parent;

    desk = get_current_desk();
    if (!desk)
        return;

    focused_bd = e_border_focused_get();
    if (!focused_bd || focused_bd->desk != desk)
        return;

    check_tinfo(desk);

    if (!_G.tinfo->conf || !_G.tinfo->conf->nb_cols) {
        return;
    }

    _G.focused_bd = focused_bd;

    _G.input_mode = INPUT_MODE_MOVING;

    /* Get input */
    parent = focused_bd->zone->container->win;
    _G.action_input_win = ecore_x_window_input_new(parent, 0, 0, 1, 1);
    if (!_G.action_input_win) {
        end_special_input();
        return;
    }

    ecore_x_window_show(_G.action_input_win);
    if (!e_grabinput_get(_G.action_input_win, 0, _G.action_input_win)) {
        end_special_input();
        return;
    }
    _G.action_timer = ecore_timer_add(TILING_OVERLAY_TIMEOUT,
                                      _timeout_cb, NULL);

    _G.handler_key = ecore_event_handler_add(ECORE_EVENT_KEY_DOWN,
                                             move_key_down, NULL);
    _check_moving_anims(focused_bd, NULL, -1);
}

/* }}} */
/* Adjust Transitions {{{ */

static void
_transition_overlays_free_cb(void *data)
{
    transition_overlay_t *trov = data;

    if (trov->overlay.obj) {
        evas_object_del(trov->overlay.obj);
        trov->overlay.obj = NULL;
    }
    if (trov->overlay.popup) {
        e_object_del(E_OBJECT(trov->overlay.popup));
        trov->overlay.popup = NULL;
    }
    if (trov != _G.transition_overlay) {
        E_FREE(trov);
    }
}

static void
_transition_move(tiling_move_t direction)
{
    int delta = TILING_RESIZE_STEP;
    int col;
    E_Popup *popup = NULL;

    if (!_G.transition_overlay)
        return;

    col = _G.transition_overlay->col;

    if (_G.transition_overlay->bd) {
        Eina_List *l = NULL;
        E_Border *bd = _G.transition_overlay->bd,
                 *nextbd = NULL;
        Border_Extra *extra = NULL,
                     *nextextra = NULL;
        int min_height = 0;

        l = eina_list_data_find_list(_G.tinfo->columns[col], bd);
        if (!l) {
            ERR("unable to bd %p in column %d", bd, col);
            return;
        }

        if (direction == MOVE_UP) {
            delta *= -1;
        }

        nextbd = l->next->data;
        min_height = MAX(nextbd->client.icccm.base_h, 1);

        extra = eina_hash_find(_G.border_extras, &bd);
        if (!extra) {
            ERR("No extra for %p", bd);
            return;
        }
        nextextra = eina_hash_find(_G.border_extras, &nextbd);
        if (!nextextra) {
            ERR("No extra for %p", nextbd);
            return;
        }

        if (nextextra->expected.h - delta < min_height)
            delta = nextextra->expected.h - min_height;

        nextextra->expected.y += delta;
        nextextra->expected.h -= delta;
        e_border_move_resize(nextbd,
                             nextextra->expected.x,
                             nextextra->expected.y,
                             nextextra->expected.w,
                             nextextra->expected.h);

        extra->expected.h += delta;
        e_border_move_resize(bd,
                             extra->expected.x,
                             extra->expected.y,
                             extra->expected.w,
                             extra->expected.h);

        popup = _G.transition_overlay->overlay.popup;
        e_popup_move(popup, popup->x, popup->y + delta);
    } else {

        if (col == TILING_MAX_COLUMNS || !_G.tinfo->columns[col + 1]) {
            return;
        }
        if (direction == MOVE_LEFT) {
            delta *= -1;
        }

        if (delta + 1 > _G.tinfo->w[col + 1])
            delta = _G.tinfo->w[col + 1] - 1;

        _move_resize_column(col, 0, delta);
        _move_resize_column(col+1, delta, -delta);

        popup = _G.transition_overlay->overlay.popup;
        e_popup_move(popup, popup->x + delta, popup->y);
    }
}

static Eina_Bool
_transition_overlay_key_down(void *data,
                             int type,
                             void *event)
{
    Ecore_Event_Key *ev = event;

    if (ev->event_window != _G.action_input_win)
        return ECORE_CALLBACK_PASS_ON;

    if (strcmp(ev->key, "Return") == 0)
        goto stop;
    if (strcmp(ev->key, "Escape") == 0)
        goto stop;

    /* reset timer */
    ecore_timer_delay(_G.action_timer, TILING_OVERLAY_TIMEOUT
                      - ecore_timer_pending_get(_G.action_timer));

    if (_G.transition_overlay) {
        if (_G.transition_overlay->bd) {
            if ((strcmp(ev->key, "Up") == 0)
            ||  (strcmp(ev->key, "k") == 0))
            {
                _transition_move(MOVE_UP);
                return ECORE_CALLBACK_PASS_ON;
            } else if ((strcmp(ev->key, "Down") == 0)
                   ||  (strcmp(ev->key, "j") == 0))
            {
                _transition_move(MOVE_DOWN);
                return ECORE_CALLBACK_PASS_ON;
            }
        } else {
            if ((strcmp(ev->key, "Left") == 0)
            ||  (strcmp(ev->key, "h") == 0))
            {
                _transition_move(MOVE_LEFT);
                return ECORE_CALLBACK_PASS_ON;
            } else if ((strcmp(ev->key, "Right") == 0)
                   ||  (strcmp(ev->key, "l") == 0))
            {
                _transition_move(MOVE_RIGHT);
                return ECORE_CALLBACK_PASS_ON;
            }
        }
        return ECORE_CALLBACK_RENEW;
    } else {
        transition_overlay_t *trov = NULL;

        trov = eina_hash_find(_G.overlays, ev->key);
        if (trov) {
            E_Border *bd = trov->bd;
            Border_Extra *extra;
            Evas_Coord ew, eh;

            _G.transition_overlay = trov;
            eina_hash_free(_G.overlays);
            _G.overlays = NULL;

            if (bd) {
                extra = eina_hash_find(_G.border_extras, &bd);
                if (!extra) {
                    ERR("No extra for %p", bd);
                    goto stop;
                }
            }
            if (!trov->overlay.popup) {
                trov->overlay.popup = e_popup_new(_G.tinfo->desk->zone,
                                                  0, 0, 1, 1);
                e_popup_layer_set(trov->overlay.popup, TILING_POPUP_LAYER);
            }
            if (!trov->overlay.obj) {
                trov->overlay.obj =
                    edje_object_add(trov->overlay.popup->evas);
            }
            _theme_edje_object_set(trov->overlay.obj,
                bd? "modules/e-tiling/transition/horizontal":
                    "modules/e-tiling/transition/vertical");

            edje_object_size_min_calc(trov->overlay.obj, &ew, &eh);
            e_popup_edje_bg_object_set(trov->overlay.popup,
                                       trov->overlay.obj);
            evas_object_show(trov->overlay.obj);
            if (bd) {
                e_popup_move_resize(trov->overlay.popup,
                                    extra->expected.x + extra->expected.w/2
                                            - ew/2
                                            - trov->overlay.popup->zone->x,
                                    extra->expected.y + extra->expected.h
                                            - eh/2
                                            - trov->overlay.popup->zone->y,
                                    ew,
                                    eh);
            } else {
                e_popup_move_resize(trov->overlay.popup,
                                    (_G.tinfo->x[trov->col]
                                     + _G.tinfo->w[trov->col]
                                     - trov->overlay.popup->zone->x - ew/2),
                                    (trov->overlay.popup->zone->h/2 - eh/2),
                                    ew, eh);
            }
            evas_object_resize(trov->overlay.obj, ew, eh);
            e_popup_show(trov->overlay.popup);

            return ECORE_CALLBACK_RENEW;
        }
    }

stop:
    end_special_input();
    return ECORE_CALLBACK_DONE;
}

static void
_do_transition_overlay(void)
{
    int nb_win;
    const char *keys = TILING_KEYS;
    const char *c = keys;
    Ecore_X_Window parent;

    end_special_input();

    nb_win = get_window_count();
    if (nb_win < 2) {
        return;
    }

    _G.input_mode = INPUT_MODE_TRANSITION;

    _G.overlays = eina_hash_string_small_new(_transition_overlays_free_cb);

    for (int i = 0; i < TILING_MAX_COLUMNS; i++) {
        Eina_List *l;
        E_Border *bd;

        if (!_G.tinfo->columns[i])
            break;
        EINA_LIST_FOREACH(_G.tinfo->columns[i], l, bd) {
            if (l->next && *c) {
                Border_Extra *extra;
                Evas_Coord ew, eh;
                transition_overlay_t *trov;

                extra = eina_hash_find(_G.border_extras, &bd);
                if (!extra) {
                    ERR("No extra for %p", bd);
                    continue;
                }

                trov = E_NEW(transition_overlay_t, 1);

                trov->overlay.popup = e_popup_new(bd->zone, 0, 0, 1, 1);
                if (!trov->overlay.popup)
                    continue;

                e_popup_layer_set(trov->overlay.popup, TILING_POPUP_LAYER);
                trov->overlay.obj = edje_object_add(trov->overlay.popup->evas);
                e_theme_edje_object_set(trov->overlay.obj,
                                        "base/theme/borders",
                                        "e/widgets/border/default/resize");

                trov->key[0] = *c;
                trov->key[1] = '\0';
                trov->col = i;
                trov->bd = bd;
                c++;

                eina_hash_add(_G.overlays, trov->key, trov);
                edje_object_part_text_set(trov->overlay.obj,
                                          "e.text.label",
                                          trov->key);
                edje_object_size_min_calc(trov->overlay.obj, &ew, &eh);
                evas_object_move(trov->overlay.obj, 0, 0);
                evas_object_resize(trov->overlay.obj, ew, eh);
                evas_object_show(trov->overlay.obj);
                e_popup_edje_bg_object_set(trov->overlay.popup,
                                           trov->overlay.obj);

                evas_object_show(trov->overlay.obj);
                e_popup_show(trov->overlay.popup);

                e_popup_move_resize(trov->overlay.popup,
                    (extra->expected.x - trov->overlay.popup->zone->x) +
                        ((extra->expected.w - ew) / 2),
                    (extra->expected.y - trov->overlay.popup->zone->y +
                        extra->expected.h - (eh / 2)),
                    ew, eh);

                e_popup_show(trov->overlay.popup);
            }
        }
        if (i != TILING_MAX_COLUMNS && _G.tinfo->columns[i+1] && *c) {
            Evas_Coord ew, eh;
            transition_overlay_t *trov;

            trov = E_NEW(transition_overlay_t, 1);

            trov->overlay.popup = e_popup_new(_G.tinfo->desk->zone,
                                              0, 0, 1, 1);
            if (!trov->overlay.popup)
                continue;

            e_popup_layer_set(trov->overlay.popup, TILING_POPUP_LAYER);
            trov->overlay.obj = edje_object_add(trov->overlay.popup->evas);
            e_theme_edje_object_set(trov->overlay.obj,
                                    "base/theme/borders",
                                    "e/widgets/border/default/resize");

            trov->key[0] = *c;
            trov->key[1] = '\0';
            trov->col = i;
            trov->bd = NULL;
            c++;

            eina_hash_add(_G.overlays, trov->key, trov);
            edje_object_part_text_set(trov->overlay.obj,
                                      "e.text.label",
                                      trov->key);
            edje_object_size_min_calc(trov->overlay.obj, &ew, &eh);
            evas_object_move(trov->overlay.obj, 0, 0);
            evas_object_resize(trov->overlay.obj, ew, eh);
            evas_object_show(trov->overlay.obj);
            e_popup_edje_bg_object_set(trov->overlay.popup,
                                       trov->overlay.obj);

            evas_object_show(trov->overlay.obj);
            e_popup_show(trov->overlay.popup);

            e_popup_move_resize(trov->overlay.popup,
                                (_G.tinfo->x[i] + _G.tinfo->w[i]
                                  - trov->overlay.popup->zone->x - ew / 2),
                                (trov->overlay.popup->zone->h / 2 - eh / 2),
                                ew, eh);

            e_popup_show(trov->overlay.popup);
        }
    }

    /* Get input */
    parent = _G.tinfo->desk->zone->container->win;
    _G.action_input_win = ecore_x_window_input_new(parent, 0, 0, 1, 1);
    if (!_G.action_input_win) {
        end_special_input();
        return;
    }

    ecore_x_window_show(_G.action_input_win);
    if (!e_grabinput_get(_G.action_input_win, 0, _G.action_input_win)) {
        end_special_input();
        return;
    }
    _G.action_timer = ecore_timer_add(TILING_OVERLAY_TIMEOUT,
                                      _timeout_cb, NULL);

    _G.handler_key = ecore_event_handler_add(ECORE_EVENT_KEY_DOWN,
                                             _transition_overlay_key_down,
                                             NULL);
}

static void
_e_mod_action_adjust_transitions(E_Object   *obj,
                                 const char *params)
{
    E_Desk *desk;

    desk = get_current_desk();
    if (!desk)
        return;

    check_tinfo(desk);

    if (!_G.tinfo->conf || !_G.tinfo->conf->nb_cols) {
        return;
    }

    _do_transition_overlay();
}

/* }}} */
/* Go {{{ */

static Eina_Bool
_warp_timer(void *_)
{
    if (_G.warp_timer) {
        double spd = TILING_WRAP_SPEED;

        _G.old_warp_x = _G.warp_x;
        _G.old_warp_y = _G.warp_y;
        _G.warp_x = (_G.warp_x * (1.0 - spd)) + (_G.warp_to_x * spd);
        _G.warp_y = (_G.warp_y * (1.0 - spd)) + (_G.warp_to_y * spd);

        ecore_x_pointer_warp(_G.tinfo->desk->zone->container->win,
                             _G.warp_x, _G.warp_y);

        if (abs(_G.warp_x - _G.old_warp_x) <= 1
        &&  abs(_G.warp_y - _G.old_warp_y) <= 1) {
            _G.warp_timer = NULL;
            return ECORE_CALLBACK_CANCEL;
        }

        return ECORE_CALLBACK_RENEW;
    }
    _G.warp_timer = NULL;
    return ECORE_CALLBACK_CANCEL;
}

static void
_action_go(E_Border *_,
           Border_Extra *extra_2)
{
    E_Border *bd = extra_2->border;

    _G.warp_to_x = bd->x + (bd->w / 2);
    _G.warp_to_y = bd->y + (bd->h / 2);
    ecore_x_pointer_xy_get(_G.tinfo->desk->zone->container->win,
                           &_G.warp_x, &_G.warp_y);
    e_border_focus_latest_set(bd);
    _G.warp_timer = ecore_timer_add(0.01, _warp_timer, NULL);
}

static void
_e_mod_action_go_cb(E_Object   *obj,
                    const char *params)
{
    E_Desk *desk;

    desk = get_current_desk();
    if (!desk)
        return;

    check_tinfo(desk);

    if (!_G.tinfo->conf || !_G.tinfo->conf->nb_cols) {
        return;
    }

    _do_overlay(NULL, _action_go, INPUT_MODE_GOING);
}

/* }}} */
/* Hooks {{{*/

static void
_e_module_tiling_cb_hook(void *data,
                         void *border)
{
    E_Border *bd = border;
    int col = -1;

    if (_G.input_mode != INPUT_MODE_NONE
    &&  _G.input_mode != INPUT_MODE_MOVING
    &&  _G.input_mode != INPUT_MODE_TRANSITION)
    {
        end_special_input();
    }

    if (!bd) {
        return;
    }

    check_tinfo(bd->desk);

    if (is_floating_window(bd)) {
        return;
    }
    if (is_untilable_dialog(bd)) {
        return;
    }
    if (bd->fullscreen) {
         return;
    }

    if (!_G.tinfo->conf || !_G.tinfo->conf->nb_cols) {
        return;
    }

    col = get_column(bd);
    if (!bd->changes.size && !bd->changes.pos && !bd->changes.border
    && col >= 0) {
        return;
    }

    DBG("Show: %p / '%s' / '%s', changes(size=%d, position=%d, border=%d)"
        " g:%dx%d+%d+%d bdname:'%s' (col:%d) maximized:%x fs:%d",
        bd, bd->client.icccm.title, bd->client.netwm.name,
        bd->changes.size, bd->changes.pos, bd->changes.border,
        bd->w, bd->h, bd->x, bd->y, bd->bordername,
        col, bd->maximized, bd->fullscreen);

    if (col < 0) {
        _add_border(bd);
    } else {
        Border_Extra *extra;

        /* Move or Resize */

        extra = eina_hash_find(_G.border_extras, &bd);
        if (!extra) {
            ERR("No extra for %p", bd);
            return;
        }

        DBG("expected: %dx%d+%d+%d",
            extra->expected.w,
            extra->expected.h,
            extra->expected.x,
            extra->expected.y);
        DBG("delta:%dx%d,%d,%d. step:%dx%d. base:%dx%d",
            bd->w - extra->expected.w, bd->h - extra->expected.h,
            bd->x - extra->expected.x, bd->y - extra->expected.y,
            bd->client.icccm.step_w, bd->client.icccm.step_h,
            bd->client.icccm.base_w, bd->client.icccm.base_h);

        if (col == 0 && !_G.tinfo->columns[1] && !_G.tinfo->columns[0]->next) {
            if (bd->maximized) {
                extra->expected.x = bd->x;
                extra->expected.y = bd->y;
                extra->expected.w = bd->w;
                extra->expected.h = bd->h;
            } else {
                /* TODO: what if a window doesn't want to be maximized? */
                e_border_unmaximize(bd, E_MAXIMIZE_BOTH);
                e_border_maximize(bd, E_MAXIMIZE_EXPAND | E_MAXIMIZE_BOTH);
            }
        }
        if (bd->x == extra->expected.x && bd->y == extra->expected.y
        &&  bd->w == extra->expected.w && bd->h == extra->expected.h)
        {
            return;
        }
        if (bd->maximized) {
            bool changed = false;

            if (col > 0 && bd->maximized & E_MAXIMIZE_HORIZONTAL) {
                 e_border_unmaximize(bd, E_MAXIMIZE_HORIZONTAL);
                 e_border_move_resize(bd,
                                      extra->expected.x,
                                      extra->expected.y,
                                      extra->expected.w,
                                      extra->expected.h);
                 changed = true;
            }
            if (bd->maximized & E_MAXIMIZE_VERTICAL
            && eina_list_count(_G.tinfo->columns[col]) > 1) {
                 e_border_unmaximize(bd, E_MAXIMIZE_VERTICAL);
                 e_border_move_resize(bd,
                                      extra->expected.x,
                                      extra->expected.y,
                                      extra->expected.w,
                                      extra->expected.h);
                 changed = true;
            }
            if (changed)
                return;
        }

        if (bd->changes.border && bd->changes.size) {
            e_border_move_resize(bd, extra->expected.x, extra->expected.y,
                                     extra->expected.w, extra->expected.h);
            return;
        }

        if (abs(extra->expected.w - bd->w) >= bd->client.icccm.step_w) {
            _move_resize_border_column(bd, extra, col, TILING_RESIZE);
        }
        if (abs(extra->expected.h - bd->h) >= bd->client.icccm.step_h) {
            _move_resize_border_in_column(bd, extra, col, TILING_RESIZE);
        }
        if (extra->expected.x != bd->x) {
            _move_resize_border_column(bd, extra, col, TILING_MOVE);
        }
        if (extra->expected.y != bd->y) {
            _move_resize_border_in_column(bd, extra, col, TILING_MOVE);
        }

        if (_G.input_mode == INPUT_MODE_MOVING
        &&  bd == _G.focused_bd) {
            _check_moving_anims(bd, extra, col);
        }
    }
}

static Eina_Bool
_e_module_tiling_hide_hook(void *data,
                           int   type,
                           void *event)
{
    E_Event_Border_Hide *ev = event;
    E_Border *bd = ev->border;

    end_special_input();

    if (_G.currently_switching_desktop)
        return EINA_TRUE;

    check_tinfo(bd->desk);

    if (EINA_LIST_IS_IN(_G.tinfo->floating_windows, bd)) {
        EINA_LIST_REMOVE(_G.tinfo->floating_windows, bd);
        return EINA_TRUE;
    }

    _remove_border(bd);

    return EINA_TRUE;
}

static Eina_Bool
_e_module_tiling_desk_show(void *data,
                           int   type,
                           void *event)
{
    _G.currently_switching_desktop = 0;

    end_special_input();

    return EINA_TRUE;
}

static Eina_Bool
_e_module_tiling_desk_before_show(void *data,
                                  int   type,
                                  void *event)
{
    end_special_input();

    _G.currently_switching_desktop = 1;

    return EINA_TRUE;
}

static Eina_Bool
_e_module_tiling_desk_set(void *data,
                          int   type,
                          void *event)
{
    E_Event_Border_Desk_Set *ev = event;

    DBG("Desk set for %p: from %p to %p",
        ev->border, ev->desk, ev->border->desk);

    end_special_input();

    check_tinfo(ev->desk);
    _remove_border(ev->border);

    check_tinfo(ev->border->desk);
    if (!_G.tinfo->conf || !_G.tinfo->conf->nb_cols) {
        Border_Extra *extra;

        extra = eina_hash_find(_G.border_extras, &ev->border);
        if (extra) {
            e_border_move_resize(ev->border,
                                 extra->orig.x,
                                 extra->orig.y,
                                 extra->orig.w,
                                 extra->orig.h);
        }
        e_border_unmaximize(ev->border, E_MAXIMIZE_BOTH);
        if (!tiling_g.config->show_titles)
            change_window_border(ev->border, "default");
    } else {
        if (get_column(ev->border) < 0)
            _add_border(ev->border);
    }

    return EINA_TRUE;
}

/* }}} */
/* Module setup {{{*/

static void
_clear_info_hash(void *data)
{
    Tiling_Info *ti = data;

    eina_list_free(ti->floating_windows);
    for (int i = 0; i < TILING_MAX_COLUMNS; i++) {
        eina_list_free(ti->columns[i]);
        ti->columns[i] = NULL;
    }
    E_FREE(ti);
}

static void
_clear_border_extras(void *data)
{
    Border_Extra *be = data;

    E_FREE(be);
}

EAPI E_Module_Api e_modapi =
{
    E_MODULE_API_VERSION,
    "E-Tiling"
};

EAPI void *
e_modapi_init(E_Module *m)
{
    char buf[PATH_MAX];
    E_Desk *desk;

    tiling_g.module = m;

    if (tiling_g.log_domain < 0) {
        tiling_g.log_domain = eina_log_domain_register("e-tiling", NULL);
        if (tiling_g.log_domain < 0) {
            EINA_LOG_CRIT("could not register log domain 'e-tiling'");
        }
    }


    snprintf(buf, sizeof(buf), "%s/locale", e_module_dir_get(m));
    bindtextdomain(PACKAGE, buf);
    bind_textdomain_codeset(PACKAGE, "UTF-8");

    _G.info_hash = eina_hash_pointer_new(_clear_info_hash);

    _G.border_extras = eina_hash_pointer_new(_clear_border_extras);

    /* Callback for new windows or changes */
    _G.hook = e_border_hook_add(E_BORDER_HOOK_EVAL_POST_BORDER_ASSIGN,
                                _e_module_tiling_cb_hook, NULL);
    /* Callback for hiding windows */
    _G.handler_hide = ecore_event_handler_add(E_EVENT_BORDER_HIDE,
                                             _e_module_tiling_hide_hook, NULL);
    /* Callback when virtual desktop changes */
    _G.handler_desk_show = ecore_event_handler_add(E_EVENT_DESK_SHOW,
                                             _e_module_tiling_desk_show, NULL);
    /* Callback before virtual desktop changes */
    _G.handler_desk_before_show =
        ecore_event_handler_add(E_EVENT_DESK_BEFORE_SHOW,
                                _e_module_tiling_desk_before_show, NULL);
    /* Callback when a border is set to another desk */
    _G.handler_desk_set = ecore_event_handler_add(E_EVENT_BORDER_DESK_SET,
                                              _e_module_tiling_desk_set, NULL);

#define ACTION_ADD(_act, _cb, _title, _value)                                \
    {                                                                        \
        E_Action *_action = _act;                                            \
        const char *_name = _value;                                          \
        if ((_action = e_action_add(_name))) {                               \
            _action->func.go = _cb;                                          \
            e_action_predef_name_set(D_("E-Tiling"), D_(_title), _name,      \
                                     NULL, NULL, 0);                         \
        }                                                                    \
    }

    /* Module's actions */
    ACTION_ADD(_G.act_togglefloat, _e_mod_action_toggle_floating_cb,
               "Toggle floating", "toggle_floating");
    ACTION_ADD(_G.act_addcolumn, _e_mod_action_add_column_cb,
               "Add a Column", "add_column");
    ACTION_ADD(_G.act_removecolumn, _e_mod_action_remove_column_cb,
               "Remove a Column", "remove_column");
    ACTION_ADD(_G.act_swap, _e_mod_action_swap_cb,
               "Swap a window with an other", "swap");
    ACTION_ADD(_G.act_move, _e_mod_action_move_cb,
               "Move window", "move");
    ACTION_ADD(_G.act_adjusttransitions, _e_mod_action_adjust_transitions,
               "Adjust transitions", "adjust_transitions");
    ACTION_ADD(_G.act_go, _e_mod_action_go_cb,
               "Focus a particular window", "go");
#undef ACTION_ADD

    /* Configuration entries */
    snprintf(_G.edj_path, sizeof(_G.edj_path), "%s/e-module-e-tiling.edj",
             e_module_dir_get(m));
    e_configure_registry_category_add("windows", 50, D_("Windows"), NULL,
                                      "preferences-system-windows");
    e_configure_registry_item_add("windows/e-tiling", 150, D_("E-Tiling"),
                                  NULL, _G.edj_path,
                                  e_int_config_tiling_module);

    /* Configuration itself */
    _G.config_edd = E_CONFIG_DD_NEW("Tiling_Config", Config);
    _G.vdesk_edd = E_CONFIG_DD_NEW("Tiling_Config_VDesk",
                                   struct _Config_vdesk);
    E_CONFIG_VAL(_G.config_edd, Config, tile_dialogs, INT);
    E_CONFIG_VAL(_G.config_edd, Config, show_titles, INT);

    E_CONFIG_LIST(_G.config_edd, Config, vdesks, _G.vdesk_edd);
    E_CONFIG_VAL(_G.vdesk_edd, struct _Config_vdesk, x, INT);
    E_CONFIG_VAL(_G.vdesk_edd, struct _Config_vdesk, y, INT);
    E_CONFIG_VAL(_G.vdesk_edd, struct _Config_vdesk, zone_num, INT);
    E_CONFIG_VAL(_G.vdesk_edd, struct _Config_vdesk, nb_cols, INT);

    tiling_g.config = e_config_domain_load("module.e-tiling", _G.config_edd);
    if (!tiling_g.config) {
        tiling_g.config = E_NEW(Config, 1);
        tiling_g.config->tile_dialogs = 1;
        tiling_g.config->show_titles = 1;
    }

    E_CONFIG_LIMIT(tiling_g.config->tile_dialogs, 0, 1);
    E_CONFIG_LIMIT(tiling_g.config->show_titles, 0, 1);

    desk = get_current_desk();
    _G.tinfo = _initialize_tinfo(desk);

    _G.input_mode = INPUT_MODE_NONE;
    _G.currently_switching_desktop = 0;
    _G.action_cb = NULL;

    return m;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{

    if (tiling_g.log_domain >= 0) {
        eina_log_domain_unregister(tiling_g.log_domain);
        tiling_g.log_domain = -1;
    }

    if (_G.hook) {
        e_border_hook_del(_G.hook);
        _G.hook = NULL;
    }

#define FREE_HANDLER(x)              \
    if (x) {                         \
        ecore_event_handler_del(x);  \
        x = NULL;                    \
    }
    FREE_HANDLER(_G.handler_hide);
    FREE_HANDLER(_G.handler_desk_show);
    FREE_HANDLER(_G.handler_desk_before_show);
    FREE_HANDLER(_G.handler_mouse_move);
    FREE_HANDLER(_G.handler_desk_set);
#undef FREE_HANDLER


#define ACTION_DEL(act, title, value)                        \
    if (act) {                                               \
        e_action_predef_name_del(D_("E-Tiling"), D_(title)); \
        e_action_del(value);                                 \
        act = NULL;                                          \
    }
    ACTION_DEL(_G.act_togglefloat, "Toggle floating", "toggle_floating");
    ACTION_DEL(_G.act_addcolumn, "Add a Column", "add_column");
    ACTION_DEL(_G.act_removecolumn, "Remove a Column", "remove_column");
    ACTION_DEL(_G.act_swap, "Swap a window with an other", "swap");
    ACTION_DEL(_G.act_move, "Move window", "move");
    ACTION_DEL(_G.act_adjusttransitions, "Adjust transitions",
               "adjust_transitions");
    ACTION_DEL(_G.act_go, "Focus a particular window", "go");
#undef ACTION_DEL

    e_configure_registry_item_del("windows/e-tiling");
    e_configure_registry_category_del("windows");

    end_special_input();

    E_FREE(tiling_g.config);
    E_CONFIG_DD_FREE(_G.config_edd);
    E_CONFIG_DD_FREE(_G.vdesk_edd);

    tiling_g.module = NULL;

    eina_hash_free(_G.info_hash);
    _G.info_hash = NULL;

    eina_hash_free(_G.border_extras);
    _G.border_extras = NULL;

    _G.tinfo = NULL;

    return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
    e_config_domain_save("module.e-tiling", _G.config_edd, tiling_g.config);

    return EINA_TRUE;
}
/* }}} */
