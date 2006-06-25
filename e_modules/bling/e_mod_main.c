#include <e.h>
#include "e_mod_main.h"
#include "e_mod_config.h"
#include "config.h"

/* module private routines */
static Bling * _bling_init(E_Module *m);
static void _bling_shutdown(Bling *b);
#if 0
static void _bling_config_menu_new(Bling *b);
#endif
static void _bling_menu_cb_configure(void *data, E_Menu *m, E_Menu_Item *mi);

/* public module routines. all modules must have these */
EAPI E_Module_Api e_modapi = {
   E_MODULE_API_VERSION,
   "Composite"
};

EAPI void *
e_modapi_init(E_Module *m)
{
   Bling *b = NULL;

   /* Set up module's message catalogue */
   bindtextdomain(PACKAGE, LOCALEDIR);
   bind_textdomain_codeset(PACKAGE, "UTF-8");

   b = _bling_init(m);
   if (!b) return NULL;

   return b;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
   Bling *b;

   b = m->data;
   if (b)
     {
        if (b->config_dialog)
          {
             e_object_del(E_OBJECT(b->config_dialog));
             b->config_dialog = NULL;
          }
        _bling_shutdown(b);
     }
   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   Bling *b;

   b = m->data;
   if (b)
      e_config_domain_save("module.bling", b->conf_edd, b->config);
   return 1;
}

EAPI int
e_modapi_about(E_Module *m)
{
   e_module_dialog_show(D_("Enlightenment Bling Module"), D_("A module that enables X composite features such as translucency and drop shadows"));
   return 1;
}

EAPI int
e_modapi_config(E_Module *m)
{
   Bling *b;
   E_Container *con;

   b = m->data;
   if (!b)
      return 0;
   con = e_container_current_get(e_manager_current_get());
   _config_bling_module(con, b);
   return 1;
}

static void
_bling_shutdown(Bling *b)
{
   composite_shutdown();
   free(b->config);
   E_CONFIG_DD_FREE(b->conf_edd);
   free(b);
}

#if 0
static void
_bling_config_menu_new(Bling *b)
{
   E_Menu *menu;
   E_Menu_Item *mi;

   menu = e_menu_new();
   mi = e_menu_item_new(menu);
   e_menu_item_label_set(mi, _("Configuration"));
   e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");
   e_menu_item_callback_set(mi, _bling_menu_cb_configure, b);
   b->config_menu = menu;
}
#endif

static void
_bling_menu_cb_configure(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Bling *b;
   E_Container *con;

   b = (Bling *)data;
   if (!b) return;

   con = e_container_current_get(e_manager_current_get());
   _config_bling_module(con, b);
}

static int
_bling_composite_restart(void *data)
{
   Bling *b = data;

   composite_init(b);
   return 0;
}

static Bling *
_bling_init(E_Module *m)
{
   Bling *b;
   
   b = E_NEW(Bling, 1);
   if (!b) return NULL;

   b->conf_edd = E_CONFIG_DD_NEW("Bling_Config", Config);
#undef T
#undef D
#define T Config
#define D b->conf_edd
   E_CONFIG_VAL(D, T, trans_border_only, UCHAR);
   E_CONFIG_VAL(D, T, trans_active_enable, UCHAR);
   E_CONFIG_VAL(D, T, trans_active_value, INT);
   E_CONFIG_VAL(D, T, trans_inactive_enable, UCHAR);
   E_CONFIG_VAL(D, T, trans_inactive_value, INT);
   E_CONFIG_VAL(D, T, trans_moving_enable, UCHAR);
   E_CONFIG_VAL(D, T, trans_moving_value, INT);
   E_CONFIG_VAL(D, T, trans_on_top_is_active, UCHAR);

   E_CONFIG_VAL(D, T, shadow_enable, UCHAR);
   E_CONFIG_VAL(D, T, shadow_dock_enable, UCHAR);
   E_CONFIG_VAL(D, T, shadow_active_size, INT);
   E_CONFIG_VAL(D, T, shadow_inactive_size,INT);
   E_CONFIG_VAL(D, T, shadow_opacity, DOUBLE);
   E_CONFIG_VAL(D, T, shadow_vert_offset, INT);
   E_CONFIG_VAL(D, T, shadow_horz_offset, INT);
   E_CONFIG_VAL(D, T, shadow_hide_on_move, UCHAR);
   E_CONFIG_VAL(D, T, shadow_hide_on_resize, UCHAR);

   E_CONFIG_VAL(D, T, fx_fade_enable, UCHAR);
   E_CONFIG_VAL(D, T, fx_fade_opacity_enable, UCHAR);
   E_CONFIG_VAL(D, T, fx_fade_in_step, DOUBLE);
   E_CONFIG_VAL(D, T, fx_fade_out_step, DOUBLE);

   b->config = e_config_domain_load("module.bling", b->conf_edd);
   if (!b->config)
   {
      b->config = E_NEW(Config, 1);

      b->config->trans_border_only = 0;
      b->config->trans_active_enable = 0;
      b->config->trans_active_value = 100;
      b->config->trans_inactive_enable = 1;
      b->config->trans_inactive_value = 75;
      b->config->trans_moving_enable = 1;
      b->config->trans_moving_value = 50;
      b->config->trans_on_top_is_active = 0;

      b->config->shadow_enable = 1;
      b->config->shadow_dock_enable = 1;
      b->config->shadow_active_size = 7;
      b->config->shadow_inactive_size = 4;
      b->config->shadow_opacity = 0.85;
      b->config->shadow_vert_offset = 3;
      b->config->shadow_horz_offset = 3;
      b->config->shadow_hide_on_move = 0;

      b->config->fx_fade_enable = 1;
      b->config->fx_fade_opacity_enable = 1;
      b->config->fx_fade_in_step = 0.075;
      b->config->fx_fade_out_step = 0.100;
   }

#if 0
   E_CONFIG_LIMIT(b->config->trans_border_only, 0, 1);
   E_CONFIG_LIMIT(b->config->trans_active_enable, 0, 1);
   E_CONFIG_LIMIT(b->config->trans_inactive_enable, 0, 1);
   E_CONFIG_LIMIT(b->config->trans_moving_enable, 0, 1);
   E_CONFIG_LIMIT(b->config->trans_on_top_is_active, 0, 1);
#endif
   E_CONFIG_LIMIT(b->config->trans_active_value, 0, 100);
   E_CONFIG_LIMIT(b->config->trans_inactive_value, 0, 100);
   E_CONFIG_LIMIT(b->config->trans_moving_value, 0, 100);

#if 0
   E_CONFIG_LIMIT(b->config->shadow_enable, 0, 1);
   E_CONFIG_LIMIT(b->config->shadow_dock_enable, 0, 1);
   E_CONFIG_LIMIT(b->config->shadow_hide_on_move, 0, 1);
#endif
   E_CONFIG_LIMIT(b->config->shadow_active_size, 0, 50);
   E_CONFIG_LIMIT(b->config->shadow_inactive_size, 0, 50);
   E_CONFIG_LIMIT(b->config->shadow_opacity, 0.0, 1.0);
   E_CONFIG_LIMIT(b->config->shadow_vert_offset, 0, 100);
   E_CONFIG_LIMIT(b->config->shadow_horz_offset, 0, 100);

#if 0
   E_CONFIG_LIMIT(b->config->fx_fade_enable, 0, 1);
   E_CONFIG_LIMIT(b->config->fx_fade_opacity_enable, 0, 1);
#endif
   E_CONFIG_LIMIT(b->config->fx_fade_in_step, 0.0, 1.0);
   E_CONFIG_LIMIT(b->config->fx_fade_out_step, 0.0, 1.0);

#if 0
   _bling_config_menu_new(b);
#endif
   if (getenv("RESTART"))
      ecore_idle_enterer_add(_bling_composite_restart, b);
   else if (!composite_init(b))
   {
      E_FREE(b);
      return NULL;
   }

   return b;
}


