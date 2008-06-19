/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include "e.h"
#include "e_mod_main.h"
#include "e_mod_config.h"
#include "config.h"
#include "trivials.h"

/* Prototypes */
static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);

/* Extern symbols */
extern E_Module *tiling_module;
extern Config *tiling_config;

/* Some defines to make coding with the e_widget_* easier for configuration panel */
#define CHECK(title, value) e_widget_check_add(evas, D_(title), &(value))
#define RADIO(title, value, radiogroup) e_widget_radio_add(evas, D_(title), value, radiogroup)
#define FRAME_ADD(framelist, object) e_widget_framelist_object_append(framelist, object)
#define LIST_ADD(list, object) e_widget_list_object_append(list, object, 1, 1, 0.5)
#define ILIST(width, height) { \
                                ob = e_widget_ilist_add(evas, 0, 0, NULL); \
                                e_widget_ilist_multi_select_set(ob, 0); \
                                e_widget_min_size_set(ob, width, height); \
                             }
#define ILIST_GO(selected) { \
                                if (selected != -1) e_widget_ilist_selected_set(ob, selected); \
                                e_widget_ilist_go(ob); \
                                e_widget_ilist_thaw(ob); \
                           }
#define CB_ENTRY E_Config_Dialog_Data *cfdata = data; if (!cfdata) return;

static struct _E_Config_vdesk*
get_vdesk(E_Config_Dialog_Data *cfdata, int x, int y, int zone_num)
{
   Evas_List *l;

   for (l = cfdata->vdesks; l; l = l->next)
     {
	struct _E_Config_vdesk *vd = l->data;
	if (!vd) continue;
	if (vd->x == x && vd->y == y && vd->zone_num == zone_num)
	  return vd;
     }

   return NULL;
}

EAPI E_Config_Dialog *
e_int_config_tiling_module(E_Container *con, const char *params __UNUSED__)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;
   char buf[4096];

   if (e_config_dialog_find("E", "_e_mod_tiling_config_dialog")) return NULL;
   v = E_NEW(E_Config_Dialog_View, 1);
   
   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.apply_cfdata = _basic_apply_data;
   v->basic.create_widgets = _basic_create_widgets;

   snprintf(buf, sizeof(buf), "%s/e-module-tiling.edj", e_module_dir_get(tiling_module));
   cfd = e_config_dialog_new(con,
			     D_("Tiling Configuration"),
			     "E", "_e_mod_tiling_config_dialog",
			     buf, 0, v, NULL);
   return cfd;
}

/*
 * Fills the E_Config_Dialog-struct with the data currently in use
 *
 */
static void *
_create_data(E_Config_Dialog *cfd) 
{
   E_Config_Dialog_Data *cfdata = E_NEW(E_Config_Dialog_Data, 1);
   /* Because we save a lot of lines here by using memcpy, the structs have to be ordered the same */
   memcpy(cfdata, tiling_config, sizeof(Config));


   /* Handle things which can't be easily memcpy'd */
   cfdata->tiling_border = (tiling_config->tiling_border ? strdup(tiling_config->tiling_border) : NULL);
   cfdata->floating_border = (tiling_config->floating_border ? strdup(tiling_config->floating_border) : NULL);
   cfdata->vdesks = NULL;
   Evas_List *l;
   for (l = tiling_config->vdesks; l; l = l->next)
     {
	struct _E_Config_vdesk *vd = l->data;
	if (!vd) continue;
	struct _E_Config_vdesk *newvd = malloc(sizeof(struct _E_Config_vdesk));
	newvd->x = vd->x;
	newvd->y = vd->y;
	newvd->zone_num = vd->zone_num;
	newvd->layout = vd->layout;
	cfdata->vdesks = evas_list_append(cfdata->vdesks, newvd);
     }

   return cfdata;
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata) 
{
   free(cfdata);
}

static void
_fill_zone_config(E_Zone *zone, E_Config_Dialog_Data *cfdata)
{
   E_Radio_Group *rg;
   Evas *evas = cfdata->evas;
   int i;

   /* Clear old entries first */
   evas_object_del(cfdata->o_desklist);
   cfdata->o_desklist = e_widget_list_add(evas, 1, 0);
   e_scrollframe_child_set(e_widget_scrollframe_object_get(cfdata->o_deskscroll), cfdata->o_desklist);
   e_widget_sub_object_add(cfdata->o_deskscroll, cfdata->o_desklist);

   for (i = 0; i < zone->desk_y_count * zone->desk_x_count; i++)
     {
	E_Desk *desk = zone->desks[i];
	if (!desk) continue;

	struct _E_Config_vdesk *vd = get_vdesk(cfdata, desk->x, desk->y, zone->num);
	if (!vd)
	  {
	     vd = malloc(sizeof(struct _E_Config_vdesk));
	     vd->x = desk->x;
	     vd->y = desk->y;
	     vd->zone_num = zone->num;
	     vd->layout = TILE_NONE;
	     cfdata->vdesks = evas_list_append(cfdata->vdesks, vd);
	  }

	rg = e_widget_radio_group_new(&(vd->layout));
	Evas_Object *radiolist = e_widget_list_add(evas, 0, 1);

	LIST_ADD(radiolist, e_widget_label_add(evas, desk->name));
	LIST_ADD(radiolist, RADIO("None", TILE_NONE, rg));
	LIST_ADD(radiolist, RADIO("Bigmain", TILE_BIGMAIN, rg));
	LIST_ADD(radiolist, RADIO("Grid", TILE_GRID, rg));
	LIST_ADD(cfdata->o_desklist, radiolist);
     }

   /* Get the correct sizes of desklist and scrollframe */
   int mw, mh;
   e_widget_min_size_get(cfdata->o_desklist, &mw, &mh);
   evas_object_resize(cfdata->o_desklist, mw, mh);
   if (mh > 150)
     mh = 150;
   mw += 32;
   mh += 32;
   e_widget_min_size_set(cfdata->o_deskscroll, mw, mh);
}

static void
_cb_zone_change(void *data, Evas_Object *obj)
{
   CB_ENTRY;

   int n = e_widget_ilist_selected_get(cfdata->o_zonelist);
   E_Zone *zone = e_widget_ilist_nth_data_get(cfdata->o_zonelist, n);
   if (!zone) return;
   _fill_zone_config(zone, cfdata);
}

static void
_cb_tiling_border_change(void *data, Evas_Object *obj)
{
   CB_ENTRY;

   if (cfdata->tiling_border)
     free(cfdata->tiling_border);
   cfdata->tiling_border = strdup(e_widget_ilist_selected_label_get(obj));
}

static void
_cb_floating_border_change(void *data, Evas_Object *obj)
{
   CB_ENTRY;

   if (cfdata->floating_border)
     free(cfdata->floating_border);
   cfdata->floating_border = strdup(e_widget_ilist_selected_label_get(obj));
}

static void
_cb_leave_space_change(void *data, Evas_Object *obj)
{
   E_Config_Dialog_Data *cfdata = data;
   if (!cfdata) return;

   recursively_set_disabled(cfdata->o_space_between, !cfdata->space_between);
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata) 
{
   Evas_Object *o, *ob, *of, *osf, *ossf, *ot;
   E_Radio_Group *rg;
   E_Container *con = e_container_current_get(e_manager_current_get());
   E_Zone *zone;
   Evas_List *l;
   
   o = e_widget_list_add(evas, 0, 0);
   ot = e_widget_table_add(evas, 0);

     { /* General settings */
	of = e_widget_framelist_add(evas, D_("General"), 0);
	FRAME_ADD(of, CHECK("Enable tiling", cfdata->tiling_enabled));
	FRAME_ADD(of, CHECK("Don't change window borders", cfdata->dont_touch_borders));
	FRAME_ADD(of, CHECK("Tile dialog windows aswell", cfdata->tile_dialogs));
	FRAME_ADD(of, CHECK("Set too big windows floating automatically", cfdata->float_too_big_windows));
	osf = e_widget_list_add(evas, 0, 0);
	ob = CHECK("Leave space between windows:", cfdata->space_between);
	e_widget_on_change_hook_set(ob, _cb_leave_space_change, cfdata);
	FRAME_ADD(of, ob);
	ossf = e_widget_list_add(evas, 0, 1);
	LIST_ADD(ossf, e_widget_label_add(evas, D_("Horizontal:")));
	LIST_ADD(ossf, e_widget_slider_add(evas, 1, 0, D_("%1.0f px"), 0.0, 50.0, 1.0, 0, NULL, &(cfdata->between_x), 200));
	LIST_ADD(osf, ossf);
	ossf = e_widget_list_add(evas, 0, 1);
	LIST_ADD(ossf, e_widget_label_add(evas, D_("Vertical:")));
	LIST_ADD(ossf, e_widget_slider_add(evas, 1, 0, D_("%1.0f px"), 0.0, 50.0, 1.0, 0, NULL, &(cfdata->between_y), 200));
	LIST_ADD(osf, ossf);
	cfdata->o_space_between = osf;
	recursively_set_disabled(osf, !cfdata->space_between);
	FRAME_ADD(of, osf);
	e_widget_table_object_append(ot, of, 0, 0, 1, 2, 1, 1, 1, 1);
     }

     { /* Virtual desktop settings */
	of = e_widget_framelist_add(evas, D_("Virtual Desktops"), 0);
	rg = e_widget_radio_group_new(&(cfdata->tiling_mode));
	FRAME_ADD(of, RADIO("Don't tile by default", TILE_NONE, rg));
	FRAME_ADD(of, RADIO("Bigmain: Main window left, small windows right", TILE_BIGMAIN, rg));
	FRAME_ADD(of, RADIO("Grid: Distribute windows equally", TILE_GRID, rg));
	FRAME_ADD(of, RADIO("Individual modes:", TILE_INDIVIDUAL, rg));

	osf = e_widget_list_add(evas, 0, 1);

	  { /* Zone list */
	     ILIST(100, 100);
	     cfdata->o_zonelist = ob;
	     e_widget_on_change_hook_set(ob, _cb_zone_change, cfdata);
	     for (l = con->zones; l; l = l->next)
	       {
		  if (!(zone = l->data)) continue;
		  e_widget_ilist_append(ob, NULL, zone->name, NULL, zone, NULL);
	       }
	     ILIST_GO(0);
	     LIST_ADD(osf, ob);
	  }

	  { /* List of individual tiling modes */
	     /* Order is important here: Firstly create the list, then add it to the
	      * scrollframe before any objects get added to the list */
	     cfdata->o_desklist = e_widget_list_add(evas, 1, 0);
	     cfdata->o_deskscroll = e_widget_scrollframe_simple_add(evas, cfdata->o_desklist);
	     cfdata->evas = evas;

	     _fill_zone_config(con->zones->data, cfdata);
   
	     LIST_ADD(osf, cfdata->o_deskscroll);
	  }
	FRAME_ADD(of, osf);
	e_widget_table_object_append(ot, of, 0, 2, 1, 1, 1, 1, 1, 1);
     }

     { /* Grid mode settings */
	of = e_widget_framelist_add(evas, D_("Grid mode settings"), 0);
	rg = e_widget_radio_group_new(&(cfdata->grid_distribute_equally));
	FRAME_ADD(of, RADIO("Distribute space equally", 1, rg));
	FRAME_ADD(of, RADIO("Use this number of rows:", 0, rg));
	FRAME_ADD(of, e_widget_slider_add(evas, 1, 0, D_("%1.0f"), 1.0, 12.0, 1.0, 0, NULL, &(cfdata->grid_rows), 100));
	e_widget_table_object_append(ot, of, 1, 0, 1, 1, 1, 1, 1, 1);
     }

     { /* Bigmain settings */
	of = e_widget_framelist_add(evas, D_("Bigmain settings"), 0);
	FRAME_ADD(of, e_widget_label_add(evas, D_("Big win takes percent of screen:")));
	FRAME_ADD(of, e_widget_slider_add(evas, 1, 0, D_("%.2f"), 0.1, 1.0, 0.01, 0, &(cfdata->big_perc), NULL, 100));
	e_widget_table_object_append(ot, of, 1, 1, 1, 1, 1, 1, 1, 1);
     }

     { /* Tiling mode border style */
	osf = e_widget_list_add(evas, 0, 0);
	of = e_widget_framelist_add(evas, D_("Tiling border"), 0);
	ILIST(100, 75);
	e_widget_on_change_hook_set(ob, _cb_tiling_border_change, cfdata);
	int sel = -1, c = 0;
	for (l = e_theme_border_list(); l; l = l->next, c++)
	  {
	     e_widget_ilist_append(ob, NULL, l->data, NULL, NULL, NULL);
	     if (cfdata->tiling_border && !strcmp(l->data, cfdata->tiling_border))
	       sel = c;
	  }
	ILIST_GO(sel);
	FRAME_ADD(of, ob);
	LIST_ADD(osf, of);

	of = e_widget_framelist_add(evas, D_("Floating border"), 0);
	ILIST(100, 75);
	e_widget_on_change_hook_set(ob, _cb_floating_border_change, cfdata);
	for (sel = -1, c = 0, l = e_theme_border_list(); l; l = l->next, c++)
	  {
	     e_widget_ilist_append(ob, NULL, l->data, NULL, NULL, NULL);
	     if (cfdata->floating_border && !strcmp(l->data, cfdata->floating_border))
	       sel = c;
	  }
	ILIST_GO(sel);
	FRAME_ADD(of, ob);
	LIST_ADD(osf, of);
     }
   e_widget_table_object_append(ot, osf, 1, 2, 1, 1, 1, 1, 1, 1);
   LIST_ADD(o, ot);

   return o;
}

static int
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata) 
{
   int need_rearrangement = memcmp(cfdata, tiling_config, sizeof(Config) - (sizeof(char*) * 2) - sizeof(Evas_List*)) ||
                            strcmp(cfdata->tiling_border, tiling_config->tiling_border) ||
			    strcmp(cfdata->floating_border, tiling_config->floating_border);

   if (!need_rearrangement)
     {
	/* Check if the layout for one of the vdesks has changed */
	Evas_List *l;
	for (l = tiling_config->vdesks; l; l = l->next)
	  {
	     struct _E_Config_vdesk *vd = l->data,
				    *newvd;
	     if (!vd || !(newvd = get_vdesk(cfdata, vd->x, vd->y, vd->zone_num))) continue;
	     if (newvd->layout != vd->layout)
	       {
		  E_Zone *zone = e_zone_current_get(e_container_current_get(e_manager_current_get()));
		  E_Desk *desk = e_desk_current_get(zone);
		  if (desk->x == vd->x && desk->y == vd->y && zone->num == vd->zone_num)
		    {
		       need_rearrangement = 1;
		       break;
		    }
	       }
	  }
     }

   if (tiling_config->floating_border)
     free(tiling_config->floating_border);
   if (tiling_config->tiling_border)
     free(tiling_config->tiling_border);
   memcpy(tiling_config, cfdata, sizeof(Config));

   cfdata->floating_border = NULL;
   cfdata->tiling_border = NULL;
   cfdata->vdesks = NULL;
   
   e_config_save_queue();

   if (need_rearrangement)
     e_mod_tiling_rearrange();

   return 1;
}
