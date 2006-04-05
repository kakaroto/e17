#include "dEvian.h"

static void _devian_menu_cb_general_configure(void *data, E_Menu *m, E_Menu_Item *mi);
static void _devian_menu_cb_general_delete_all_devians(void *data, E_Menu *m, E_Menu_Item *mi);

static void _devian_menu_cb_source_infos(void *data, E_Menu *m, E_Menu_Item *mi);
static void _devian_menu_cb_source_refresh(void *data, E_Menu *m, E_Menu_Item *mi);

#ifdef HAVE_PICTURE
static void _devian_menu_cb_source_previous(void *data, E_Menu *m, E_Menu_Item *mi);
#endif
static void _devian_menu_cb_source_viewer(void *data, E_Menu *m, E_Menu_Item *mi);
static void _devian_menu_cb_devian_delete(void *data, E_Menu *m, E_Menu_Item *mi);
static void _devian_menu_cb_configure(void *data, E_Menu *m, E_Menu_Item *mi);

#ifdef HAVE_ALL
#ifdef HAVE_PICTURE
static void _devian_menu_cb_set_source_picture(void *data, E_Menu *m, E_Menu_Item *mi);
#endif
#ifdef HAVE_RSS
static void _devian_menu_cb_set_source_rss(void *data, E_Menu *m, E_Menu_Item *mi);
#endif
#ifdef HAVE_FILE
static void _devian_menu_cb_set_source_file(void *data, E_Menu *m, E_Menu_Item *mi);
#endif
#endif
static void _devian_menu_cb_edit_mode(void *data, E_Menu *m, E_Menu_Item *mi);

static void
_devian_menu_cb_general_configure(void *data, E_Menu *m, E_Menu_Item *mi)
{
   E_Config_Dialog *cfd;

   if (!(cfd = DEVIANF(config_dialog_main) (DEVIANM->container)))
      return;
   DEVIANM->dialog_conf = cfd;  /* Actualy dont need this */
}

static void
_devian_menu_cb_general_delete_all_devians(void *data, E_Menu *m, E_Menu_Item *mi)
{
   DEVIANF(devian_del_all) ();
}

static void
_devian_menu_cb_source_infos(void *data, E_Menu *m, E_Menu_Item *mi)
{
   DEVIANN *devian;

   devian = data;

   DEVIANF(source_dialog_infos) (devian);
}

static void
_devian_menu_cb_source_refresh(void *data, E_Menu *m, E_Menu_Item *mi)
{
   DEVIANN *devian;

   devian = data;

   devian->source_func.refresh(devian, 0);
}

#ifdef HAVE_PICTURE
static void
_devian_menu_cb_source_previous(void *data, E_Menu *m, E_Menu_Item *mi)
{
   DEVIANN *devian;

   devian = data;

   devian->source_func.refresh(devian, 1);
}
#endif

static void
_devian_menu_cb_source_viewer(void *data, E_Menu *m, E_Menu_Item *mi)
{
   DEVIANN *devian;

   devian = data;

   devian->source_func.viewer(devian);
}

static void
_devian_menu_cb_devian_delete(void *data, E_Menu *m, E_Menu_Item *mi)
{
   DEVIANN *devian;

   devian = data;

   /* CHECK: if the devian was destroyed but the menu stays (key space) */
   if (!devian)
      return;
   if ((!devian->source) || (!devian->conf) || (!devian->menu))
      return;

   DEVIANF(devian_del) (devian, 0);
}

#ifdef HAVE_ALL

#ifdef HAVE_PICTURE
static void
_devian_menu_cb_set_source_picture(void *data, E_Menu *m, E_Menu_Item *mi)
{
   DEVIANN *devian;

   devian = data;

   if (DEVIANF(source_change) (devian, SOURCE_PICTURE))
     {
        e_menu_item_toggle_set(mi, 0);
     }
}
#endif

#ifdef HAVE_RSS
static void
_devian_menu_cb_set_source_rss(void *data, E_Menu *m, E_Menu_Item *mi)
{
   DEVIANN *devian;

   devian = data;

   if (DEVIANF(source_change) (devian, SOURCE_RSS))
     {
        e_menu_item_toggle_set(mi, 0);
     }
}
#endif

#ifdef HAVE_FILE
static void
_devian_menu_cb_set_source_file(void *data, E_Menu *m, E_Menu_Item *mi)
{
   DEVIANN *devian;

   devian = data;

   if (DEVIANF(source_change) (devian, SOURCE_FILE))
     {
        e_menu_item_toggle_set(mi, 0);
     }
}
#endif

#endif

static void
_devian_menu_cb_configure(void *data, E_Menu *m, E_Menu_Item *mi)
{
   DEVIANN *devian;
   E_Config_Dialog *cfd;

   devian = data;
   if (!(cfd = DEVIANF(config_dialog_devian) (DEVIANM->container, devian)))
      return;
   devian->dialog_conf = cfd;
}

static void
_devian_menu_cb_edit_mode(void *data, E_Menu *m, E_Menu_Item *mi)
{
   e_gadman_mode_set(DEVIANM->container->gadman, E_GADMAN_MODE_EDIT);
}

E_Menu *DEVIANF(menu_init) (DEVIANN *devian)
{
   E_Menu *mn, *mn2;
   E_Menu_Item *mi;

   DMAIN(("menu begin"));

   mn = e_menu_new();

   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Source Infos"));
   e_menu_item_callback_set(mi, _devian_menu_cb_source_infos, devian);
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Source Refresh"));
   e_menu_item_callback_set(mi, _devian_menu_cb_source_refresh, devian);
#ifdef HAVE_PICTURE
   if (devian->conf->source_type == SOURCE_PICTURE)
     {
        mi = e_menu_item_new(mn);
        e_menu_item_label_set(mi, _("Source Previous"));
        e_menu_item_callback_set(mi, _devian_menu_cb_source_previous, devian);
     }
#endif
   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Source Viewer"));
   e_menu_item_callback_set(mi, _devian_menu_cb_source_viewer, devian);

   /*
    * mi = e_menu_item_new(mn);
    * e_menu_item_separator_set(mi, 1);
    * 
    * mi = e_menu_item_new(mn);
    * e_menu_item_label_set(mi, _("Container Box"));
    * e_menu_item_radio_set(mi, 1);
    * e_menu_item_radio_group_set(mi, 1);
    * if ( devian->conf->container_type == CONTAINER_BOX )
    * e_menu_item_toggle_set(mi, 1);
    * e_menu_item_callback_set(mi, _devian_menu_cb_set_container_box, devian);
    * 
    * mi = e_menu_item_new(mn);
    * e_menu_item_label_set(mi, _("Bar"));
    * e_menu_item_radio_set(mi, 1);
    * e_menu_item_radio_group_set(mi, 1);
    * if ( devian->conf->container_type == CONTAINER_BAR )
    * e_menu_item_toggle_set(mi, 1);
    * e_menu_item_callback_set(mi, _devian_menu_cb_set_container_bar, devian);
    */

   mi = e_menu_item_new(mn);
   e_menu_item_separator_set(mi, 1);

   mn2 = e_menu_new();

   mi = e_menu_item_new(mn2);
   e_menu_item_label_set(mi, _("Configuration"));
   e_menu_item_callback_set(mi, _devian_menu_cb_configure, devian);

   mi = e_menu_item_new(mn2);
   e_menu_item_label_set(mi, _("Delete"));
   e_menu_item_callback_set(mi, _devian_menu_cb_devian_delete, devian);

#ifdef HAVE_ALL

   mi = e_menu_item_new(mn2);
   e_menu_item_separator_set(mi, 1);

#ifdef HAVE_PICTURE
   mi = e_menu_item_new(mn2);
   e_menu_item_label_set(mi, _("Source Picture"));
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 2);
   if (devian->conf->source_type == SOURCE_PICTURE)
      e_menu_item_toggle_set(mi, 1);
   e_menu_item_callback_set(mi, _devian_menu_cb_set_source_picture, devian);
#endif

#ifdef HAVE_RSS
   mi = e_menu_item_new(mn2);
   e_menu_item_label_set(mi, _("Source Rss"));
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 2);
   if (devian->conf->source_type == SOURCE_RSS)
      e_menu_item_toggle_set(mi, 1);
   e_menu_item_callback_set(mi, _devian_menu_cb_set_source_rss, devian);
#endif

#ifdef HAVE_FILE
   mi = e_menu_item_new(mn2);
   e_menu_item_label_set(mi, _("Source Log"));
   e_menu_item_radio_set(mi, 1);
   e_menu_item_radio_group_set(mi, 2);
   if (devian->conf->source_type == SOURCE_FILE)
      e_menu_item_toggle_set(mi, 1);
   e_menu_item_callback_set(mi, _devian_menu_cb_set_source_file, devian);
#endif
#endif

   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("This " MODULE_NAME));
   e_menu_item_submenu_set(mi, mn2);

   mn2 = e_menu_new();

   mi = e_menu_item_new(mn2);
   e_menu_item_label_set(mi, _("Configuration"));
   e_menu_item_callback_set(mi, _devian_menu_cb_general_configure, NULL);

   mi = e_menu_item_new(mn2);
   e_menu_item_label_set(mi, _("Delete all " MODULE_NAME "s"));
   e_menu_item_callback_set(mi, _devian_menu_cb_general_delete_all_devians, NULL);

   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("General"));
   e_menu_item_submenu_set(mi, mn2);

   mi = e_menu_item_new(mn);
   e_menu_item_label_set(mi, _("Edit Mode"));
   e_menu_item_callback_set(mi, _devian_menu_cb_edit_mode, devian);

   DMAIN(("menu fin"));

   return mn;
}

void DEVIANF(menu_shutdown) (DEVIANN *devian)
{
   if (devian->menu)
     {
        e_menu_deactivate(devian->menu);
        e_object_del(E_OBJECT(devian->menu));
        devian->menu = NULL;
     }
}

void DEVIANF(menu_refresh) (DEVIANN *devian)
{
   DEVIANF(menu_shutdown) (devian);
   devian->menu = DEVIANF(menu_init) (devian);
}
