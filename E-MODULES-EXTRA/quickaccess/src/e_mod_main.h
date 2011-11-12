#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <e.h>

#ifdef ENABLE_NLS
# include <libintl.h>
# define _(string) dgettext(PACKAGE, string)
#else
# define bindtextdomain(domain,dir)
# define bind_textdomain_codeset(domain,codeset)
# define _(string) (string)
#endif

typedef enum
{
  E_QUICK_ACCESS_ENTRY_RELATIVE = 0,
  E_QUICK_ACCESS_ENTRY_ABSOLUTE,
  E_QUICK_ACCESS_ENTRY_FULLSCREEN /* no animation (maybe fade in future) */
} E_Quick_Access_Entry_Mode;

typedef enum
  {
    E_QUICK_ACCESS_ENTRY_ORIENT_TOP = 0,
    E_QUICK_ACCESS_ENTRY_ORIENT_BOTTOM,
    E_QUICK_ACCESS_ENTRY_ORIENT_LEFT,
    E_QUICK_ACCESS_ENTRY_ORIENT_RIGHT,
    E_QUICK_ACCESS_ENTRY_ORIENT_CORNER_TL,
    E_QUICK_ACCESS_ENTRY_ORIENT_CORNER_TR,
    E_QUICK_ACCESS_ENTRY_ORIENT_CORNER_BL,
    E_QUICK_ACCESS_ENTRY_ORIENT_CORNER_BR,
    E_QUICK_ACCESS_ENTRY_ORIENT_CENTER /* no animation (maybe fade in future) */
  } E_Quick_Access_Entry_Orient;

typedef struct E_Quick_Access_Entry
{
   const char *id; /* entry identifier (config, actions...), stringshared */
   const char *name; /* icccm name, stringshared */
   const char *class; /* icccm class, stringshared */
   const char *cmd; /* stringshared */
   struct {
      double anim_duration; /* used when !mode.fullscreen && !orient.center */
      unsigned char mode; /* E_Quick_Access_Entry_Mode */
      unsigned char orient; /* E_Gadcon_Orient (that makes sense) */
      struct {
	 double w, h;
      } relative;
      struct {
	 int w, h;
      } absolute;
   } conf;
   E_Border *border; /* associated border, if any */
   Ecore_Exe *exe; /* if executed cmd but still no border associated */
   Ecore_Animator *anim; /* during animations to show/hide */
} E_Quick_Access_Entry;


extern int _e_quick_access_log_dom;
#define DBG(...) EINA_LOG_DOM_DBG(_e_quick_access_log_dom, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(_e_quick_access_log_dom, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(_e_quick_access_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_e_quick_access_log_dom, __VA_ARGS__)
#define CRIT(...) EINA_LOG_DOM_CRIT(_e_quick_access_log_dom, __VA_ARGS__)

EAPI extern E_Module_Api e_modapi;
EAPI void *e_modapi_init(E_Module *m);
EAPI int e_modapi_shutdown(E_Module *m);
EAPI int e_modapi_save(E_Module *m);

#endif
