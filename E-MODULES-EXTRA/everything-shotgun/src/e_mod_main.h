/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#ifdef ENABLE_NLS
# include <libintl.h>
# define _(string) dgettext(PACKAGE, string)
#else
# define bindtextdomain(domain,dir)
# define bind_textdomain_codeset(domain,codeset)
# define _(string) (string)
#endif
#define N_(str) (str)

#include "evry_api.h"

typedef struct _Contact Contact;
typedef struct _Message Message;

struct _Contact
{
  Evry_Item base;

  const char *id;
  const char *icon;
  Evas_Object *o_icon;
  DBusPendingCall *pnd_info;
};

struct _Message
{
  const char *contact;
  const char *msg;
  double time;
  int self;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init     (E_Module *m);
EAPI int   e_modapi_shutdown (E_Module *m);
EAPI int   e_modapi_save     (E_Module *m);

void      evry_plug_msg_shutdown(void);
Eina_Bool evry_plug_msg_init(void);

extern char *theme_file;
extern Evry_Type SHOTGUN_CONTACT;
extern Evry_Type SHOTGUN_MESSAGE;
extern Eina_List *messages;
extern int SHOTGUN_EVENT_MESSAGE_ADD;

#define MAX_HISTORY 100

#define CONTACT(_it)((Contact *)_it)
#define GET_CONTACT(_c, _it) Contact *_c = (Contact *)_it

#endif
