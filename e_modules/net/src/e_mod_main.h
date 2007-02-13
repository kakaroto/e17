#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

typedef struct _Instance Instance;
typedef struct _Popup Popup;

struct _Instance 
{
   E_Gadcon_Client *gcc;
   Evas_Object *o_net;
   Ecore_Timer *timer;
   Popup *popup;
   short popup_locked;
   unsigned long in, out;
};

struct _Popup 
{
   E_Popup *win;
   Evas_Object *o_bg;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init(E_Module *m);
EAPI int e_modapi_shutdown(E_Module *m);
EAPI int e_modapi_save(E_Module *m);
EAPI int e_modapi_about(E_Module *m);

#endif
