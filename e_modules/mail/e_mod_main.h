#define D_(str) dgettext(PACKAGE, str)

#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

typedef enum 
{
   MAIL_TYPE_POP,
     MAIL_TYPE_IMAP,
     MAIL_TYPE_MDIR,
     MAIL_TYPE_MBOX
} MailType;

typedef struct _Config Config;
typedef struct _Config_Item Config_Item;
typedef struct _Config_Box Config_Box;

struct _Config 
{
   E_Module *module;
   E_Config_Dialog *config_dialog;
   E_Menu *menu;
   Evas_List *instances;
   Evas_List *items;
};

struct _Config_Item 
{
   const char *id;

   unsigned char show_label;
   double check_time;

   Evas_List *boxes;
};

struct _Config_Box 
{
   const char *name;

   int type;   
   int port;
   unsigned char ssl;
   const char *host;
   const char *user;
   const char *pass;
   const char *new_path;
   const char *cur_path;   

   int num_new, num_total;

   unsigned char use_exec;   
   const char *exec;
   Ecore_Exe *exe;
};

EAPI extern E_Module_Api e_modapi;

EAPI void *e_modapi_init(E_Module *m);
EAPI int   e_modapi_shutdown(E_Module *m);
EAPI int   e_modapi_save(E_Module *m);
EAPI int   e_modapi_about(E_Module *m);

void _config_mail_module(Config_Item *ci);
void _mail_config_updated(const char *id);
void _mail_box_config_updated(E_Config_Dialog *cfd);
void _mail_box_deleted(const char *ci_name, const char *box_name);
void _mail_box_added(const char *ci_name, const char *box_name);
void _mail_set_text(void *data, int count);

extern Config *mail_config;

#endif
