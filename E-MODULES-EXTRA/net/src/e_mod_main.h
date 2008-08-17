#define D_(str) dgettext(PACKAGE, str)

#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

typedef struct _Instance Instance;
typedef unsigned long bytes_t;

struct _Instance 
{
   E_Gadcon_Client *gcc;
   Evas_Object *o_net, *pop_bg;
   Ecore_Timer *timer;
   E_Gadcon_Popup *popup;

   short popup_locked;
   bytes_t in, out;

   Config_Item *ci;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init(E_Module *m);
EAPI int e_modapi_shutdown(E_Module *m);
EAPI int e_modapi_save(E_Module *m);

#endif
