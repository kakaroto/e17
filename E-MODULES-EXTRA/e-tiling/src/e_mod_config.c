#include "e_mod_tiling.h"

/* HACK: Needed to get subobjs of the widget. Is there a better way? */
typedef struct _E_Widget_Smart_Data E_Widget_Smart_Data;

struct _E_Widget_Smart_Data
{
   Evas_Object *parent_obj;
   Evas_Coord   x, y, w, h;
   Evas_Coord   minw, minh;
   Eina_List   *subobjs;
};

/* Some defines to make coding with the e_widget_* easier for
 * configuration panel */
#define RADIO(title, value, radiogroup) \
  e_widget_radio_add(evas, D_(title), value, radiogroup)
#define LIST_ADD(list, object) \
  e_widget_list_object_append(list, object, 1, 1, 0.5)

struct _Config_vdesk *
get_vdesk(Eina_List *vdesks,
          int x,
          int y,
          unsigned int zone_num)
{
    for (Eina_List *l = vdesks; l; l = l->next) {
        struct _Config_vdesk *vd = l->data;

        if (!vd)
            continue;

        if (0 < vd->nb_cols && vd->nb_cols <= TILING_MAX_COLUMNS
        &&  vd->x == x && vd->y == y && vd->zone_num == zone_num)
            return vd;
    }

    return NULL;
}

/*
 * Fills the E_Config_Dialog-struct with the data currently in use
 *
 */
static void *
_create_data(E_Config_Dialog *cfd)
{
    E_Config_Dialog_Data *cfdata = E_NEW(E_Config_Dialog_Data, 1);

    /* Because we save a lot of lines here by using memcpy,
     * the structs have to be ordered the same */
    memcpy(cfdata, tiling_g.config, sizeof(Config));

    /* Handle things which can't be easily memcpy'd */
    cfdata->config.vdesks = NULL;

    for (Eina_List *l = tiling_g.config->vdesks; l; l = l->next) {
        struct _Config_vdesk *vd = l->data,
                             *newvd;

        if (!vd)
            continue;

        newvd = E_NEW(struct _Config_vdesk, 1);
        newvd->x = vd->x;
        newvd->y = vd->y;
        newvd->zone_num = vd->zone_num;
        newvd->nb_cols = vd->nb_cols;

        cfdata->config.vdesks = eina_list_append(cfdata->config.vdesks,
                                                 newvd);
    }

    return cfdata;
}

static void
_free_data(E_Config_Dialog      *cfd,
           E_Config_Dialog_Data *cfdata)
{
    struct _Config_vdesk *vd;

    eina_list_free(cfdata->config.vdesks);
    free(cfdata);
}

static void
_fill_zone_config(E_Zone               *zone,
                  E_Config_Dialog_Data *cfdata)
{
    Evas *evas = cfdata->evas;
    Evas_Object *o;
    int mw, mh;

    /* Clear old entries first */
    evas_object_del(cfdata->o_desklist);

    cfdata->o_desklist = e_widget_list_add(evas, 1, 0);
    o = e_widget_scrollframe_object_get(cfdata->o_deskscroll);
    e_scrollframe_child_set(o, cfdata->o_desklist);
    e_widget_sub_object_add(cfdata->o_deskscroll, cfdata->o_desklist);

    for (int i = 0; i < zone->desk_y_count * zone->desk_x_count; i++) {
        E_Desk *desk = zone->desks[i];
        struct _Config_vdesk *vd;
        Evas_Object *list, *slider;

        if (!desk)
            continue;

        vd = get_vdesk(cfdata->config.vdesks, desk->x, desk->y, zone->num);
        if (!vd) {
            vd = E_NEW(struct _Config_vdesk, 1);
            vd->x = desk->x;
            vd->y = desk->y;
            vd->zone_num = zone->num;
            vd->nb_cols = 0;

            cfdata->config.vdesks = eina_list_append(cfdata->config.vdesks,
                                                     vd);
        }

        list = e_widget_list_add(evas, 0, 1);

        LIST_ADD(list, e_widget_label_add(evas, desk->name));
        slider = e_widget_slider_add(evas, 1, 0, D_("%1.0f columns"),
                                     0.0, 8.0, 1.0, 0, NULL,
                                     &vd->nb_cols, 150);
        LIST_ADD(list, slider);

        LIST_ADD(cfdata->o_desklist, list);
    }

    /* Get the correct sizes of desklist and scrollframe */
    e_widget_size_min_get(cfdata->o_desklist, &mw, &mh);
    evas_object_resize(cfdata->o_desklist, mw, mh);
    /*FIXME: what are those values?? */
    if (mh > 150)
        mh = 150;
    mw += 32;
    mh += 32;
    e_widget_size_min_set(cfdata->o_deskscroll, mw, mh);
}

static void
_cb_zone_change(void        *data,
                Evas_Object *obj)
{
    int n;
    E_Config_Dialog_Data *cfdata = data;
    E_Zone *zone;

    if (!cfdata)
        return;

    n = e_widget_ilist_selected_get(cfdata->o_zonelist);
    zone = e_widget_ilist_nth_data_get(cfdata->o_zonelist, n);
    if (!zone)
        return;
    _fill_zone_config(zone, cfdata);
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog      *cfd,
                      Evas                 *evas,
                      E_Config_Dialog_Data *cfdata)
{
    Evas_Object *o, *ob, *of, *osf;
    E_Container *con = e_container_current_get(e_manager_current_get());
    E_Zone *zone;

    o = e_widget_list_add(evas, 0, 0);

    /* General settings */
    of = e_widget_framelist_add(evas, D_("General"), 0);
    e_widget_framelist_object_append(of,
      e_widget_check_add(evas, D_("Tile dialog windows aswell"),
                         &cfdata->config.tile_dialogs));
    e_widget_framelist_object_append(of,
      e_widget_check_add(evas, D_("Show window titles"),
                         &cfdata->config.show_titles));
    LIST_ADD(o, of);

    /* Virtual desktop settings */
    of = e_widget_framelist_add(evas, D_("Virtual Desktops"), 0);
    e_widget_label_add(evas,
                       D_("Number of columns used to tile per desk"
                          " (0 → tiling disabled):"));
    osf = e_widget_list_add(evas, 0, 1);

    /* Zone list */
    ob = e_widget_ilist_add(evas, 0, 0, NULL);
    e_widget_ilist_multi_select_set(ob, 0);
    e_widget_size_min_set(ob, 100, 100);
    cfdata->o_zonelist = ob;
    e_widget_on_change_hook_set(ob, _cb_zone_change, cfdata);
    for (Eina_List *l = con->zones; l; l = l->next) {
        if (!(zone = l->data))
            continue;
        e_widget_ilist_append(ob, NULL, zone->name, NULL, zone, NULL);
    }
    e_widget_ilist_selected_set(ob, 0);
    e_widget_ilist_go(ob);
    e_widget_ilist_thaw(ob);
    LIST_ADD(osf, ob);


    /* List of individual tiling modes */
    /* Order is important here: Firstly create the list, then add it to the
     * scrollframe before any objects get added to the list */
    cfdata->o_desklist = e_widget_list_add(evas, 1, 0);
    cfdata->o_deskscroll = e_widget_scrollframe_simple_add(evas,
                                                           cfdata->o_desklist);
    cfdata->evas = evas;

    _fill_zone_config(con->zones->data, cfdata);

    LIST_ADD(osf, cfdata->o_deskscroll);

    e_widget_framelist_object_append(of, osf);

    LIST_ADD(o, of);

    return o;
}

static int
_basic_apply_data(E_Config_Dialog      *cfd,
                  E_Config_Dialog_Data *cfdata)
{
    struct _Config_vdesk *vd;

    tiling_g.config->tile_dialogs = cfdata->config.tile_dialogs;
    tiling_g.config->show_titles = cfdata->config.show_titles;

    /* Check if the layout for one of the vdesks has changed */
    for (Eina_List *l = tiling_g.config->vdesks; l; l = l->next) {
        struct _Config_vdesk *newvd;

        vd = l->data;

        if (!vd)
            continue;
        if (!(newvd = get_vdesk(cfdata->config.vdesks,
                                vd->x, vd->y, vd->zone_num))) {
            change_column_number(vd);
            continue;
        }

        if (newvd->nb_cols != vd->nb_cols) {
            DBG("number of columns for (%d, %d, %d) changed from %d to %d",
                vd->x, vd->y, vd->zone_num, vd->nb_cols, newvd->nb_cols);
            change_column_number(newvd);
            free(vd);
            l->data = NULL;
        }
    }

    for (Eina_List *l = cfdata->config.vdesks; l; l = l->next) {
        vd = l->data;

        if (!vd)
            continue;
        if (!get_vdesk(tiling_g.config->vdesks,
                       vd->x, vd->y, vd->zone_num)) {
            change_column_number(vd);
            continue;
        }
    }

    EINA_LIST_FREE(tiling_g.config->vdesks, vd) {
        free(vd);
    }
    memcpy(tiling_g.config, cfdata, sizeof(Config));
    cfdata->config.vdesks = NULL; /* we don't want this list to be freed */

    e_tiling_update_conf();

    e_config_save_queue();

    return EINA_TRUE;
}

E_Config_Dialog *
e_int_config_tiling_module(E_Container *con,
                           const char  *params)
{
    E_Config_Dialog *cfd;
    E_Config_Dialog_View *v;
    char buf[PATH_MAX];

    if (e_config_dialog_find("E", "windows/e-tiling"))
        return NULL;

    v = E_NEW(E_Config_Dialog_View, 1);

    v->create_cfdata = _create_data;
    v->free_cfdata = _free_data;
    v->basic.apply_cfdata = _basic_apply_data;
    v->basic.create_widgets = _basic_create_widgets;

    snprintf(buf, sizeof(buf), "%s/e-module-e-tiling.edj",
             e_module_dir_get(tiling_g.module));
    cfd = e_config_dialog_new(con,
                              D_("Tiling Configuration"),
                              "E", "windows/e-tiling",
                              buf, 0, v, NULL);
    return cfd;
}
