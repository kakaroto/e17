#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

typedef struct _Config Config;
typedef struct _Bling Bling;

struct _Config
{
   /* Translucency options */
   unsigned char trans_border_only;     /* Make only the border */
   unsigned char trans_active_enable;   /* Enable for focused windows */
   int trans_active_value;
   unsigned char trans_inactive_enable; /* Enable for unfocused windows */
   int trans_inactive_value;
   unsigned char trans_moving_enable;   /* Enable for moving windows */
   int trans_moving_value;
   unsigned char trans_on_top_is_active;/* Apply active setting to on-top windows */
   
   /* Drop shadow options */
   unsigned char shadow_enable;         /* Enable drop shadows */
   unsigned char shadow_dock_enable;    /* For 'dock' windows */
   int shadow_active_size;    /* For focused windows */
   int shadow_inactive_size;  /* For unfocused windows */
   double shadow_opacity;
   int shadow_vert_offset;
   int shadow_horz_offset;
   unsigned char shadow_hide_on_move;
   unsigned char shadow_hide_on_resize;
   const char *shadow_color;

   /* FX */
   unsigned char fx_fade_enable;     /* Fade windows */
   unsigned char fx_fade_opacity_enable;/* Fade between opacity changes */
   double fx_fade_delta;
   double fx_fade_in_step;
   double fx_fade_out_step;
};

struct _Bling
{
   E_Menu *config_menu;
   E_Module *module;
   E_Config_DD *conf_edd;
   Config *config;
   E_Config_Dialog *config_dialog;
};

extern E_Module *bling_mod;

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init(E_Module *m);
EAPI int e_modapi_shutdown(E_Module *m);
EAPI int e_modapi_save(E_Module *m);

/* Composite functions */
int composite_init(Bling *b);
void composite_shutdown(void);
int bling_composite_restart(void *data);
#endif /* E_MOD_MAIN_H */
