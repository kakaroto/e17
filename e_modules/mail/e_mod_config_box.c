#include <e.h>
#include "e_mod_main.h"

struct _E_Config_Dialog_Data 
{
   char *name;
   int type;
   int use_exec;
   char *port;
   int ssl;
   char *host;
   char *user;
   char *pass;
   char *new_path;
   char *cur_path;
   char *exec;
   
   /* Evas_Object References */
   Evas_Object *exec_label;
   Evas_Object *exec_entry;
   Evas_Object *port_entry;
   Evas_Object *new_path_label;
   Evas_Object *new_path_entry;
   Evas_Object *cur_path_label;
   Evas_Object *cur_path_entry;
};

static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static void _type_cb_change(void *data, Evas_Object *obj);
static void _use_exec_cb_change(void *data, Evas_Object *obj);

static E_Config_Dialog *prev_dlg;
static Config_Item *mail_ci;

void 
_config_box(Config_Item *ci, Config_Box *cb, E_Config_Dialog *mcfd) 
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;
   E_Container *con;
   
   v = E_NEW(E_Config_Dialog_View, 1);
   if (!v) return;

   prev_dlg = mcfd;
   mail_ci = ci;
   
   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.apply_cfdata = _basic_apply_data;
   v->basic.create_widgets = _basic_create_widgets;
   
   con = e_container_current_get(e_manager_current_get());
   cfd = e_config_dialog_new(con, _("Mailbox Configuration"), NULL, 0, v, cb);
}

static void 
_fill_data(Config_Box *cb, E_Config_Dialog_Data *cfdata) 
{
   char buf[1024];
   char *ptr;
   
   if (!cb) 
     {
	cfdata->type = 0;
	cfdata->ssl = 0;
	cfdata->use_exec = 0;
	snprintf(buf, sizeof(buf), "110");
	cfdata->port = strdup(buf);
	return;
     }
   
   if (cb->name)
     cfdata->name = strdup(cb->name);

   cfdata->type = cb->type;
   cfdata->ssl = cb->ssl;
   cfdata->use_exec = cb->use_exec;
   
   snprintf(buf, sizeof(buf), "%d", cb->port);
   cfdata->port = strdup(buf);

   if (cb->exec)
     cfdata->exec = strdup(cb->exec);   
   if (cb->host)
     cfdata->host = strdup(cb->host);
   if (cb->user)
     cfdata->user = strdup(cb->user);
   if (cb->pass)
     cfdata->pass = strdup(cb->pass);
 
   if (cb->new_path)
     cfdata->new_path = strdup(cb->new_path);
   if (cb->cur_path)
     cfdata->cur_path = strdup(cb->cur_path);   
}

static void *
_create_data(E_Config_Dialog *cfd) 
{
   E_Config_Dialog_Data *cfdata;
   Config_Box *cb;
   
   cb = cfd->data;
   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   _fill_data(cb, cfdata);
   return cfdata;
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata) 
{
   E_FREE(cfdata->name);
   E_FREE(cfdata->port);
   E_FREE(cfdata->host);
   E_FREE(cfdata->user);
   E_FREE(cfdata->pass);
   E_FREE(cfdata->new_path);
   E_FREE(cfdata->cur_path);
   E_FREE(cfdata->exec);

   free(cfdata);
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata) 
{
   Evas_Object *o, *of, *ob, *ot;
   E_Radio_Group *rg;
   
   o = e_widget_list_add(evas, 0, 0);
   of = e_widget_framelist_add(evas, _("General Settings"), 0);
   ob = e_widget_check_add(evas, _("Start Program When New Mail Arrives"), &(cfdata->use_exec));
   e_widget_framelist_object_append(of, ob);
   e_widget_on_change_hook_set(ob, _use_exec_cb_change, cfdata);
   ot = e_widget_table_add(evas, 1);
   cfdata->exec_label = e_widget_label_add(evas, _("Program:"));
   e_widget_table_object_append(ot, cfdata->exec_label, 0, 0, 1, 1, 0, 0, 1, 0);
   cfdata->exec_entry = e_widget_entry_add(evas, &cfdata->exec);
   e_widget_table_object_append(ot, cfdata->exec_entry, 1, 0, 1, 1, 0, 0, 1, 0);
   e_widget_framelist_object_append(of, ot);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   e_widget_disabled_set(cfdata->exec_label, !cfdata->use_exec);
   e_widget_disabled_set(cfdata->exec_entry, !cfdata->use_exec);   

   of = e_widget_framelist_add(evas, _("Mailbox Type"), 0);
   rg = e_widget_radio_group_new(&(cfdata->type));
   ob = e_widget_radio_add(evas, _("Pop3"), 0, rg);
   e_widget_on_change_hook_set(ob, _type_cb_change, cfdata);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, _("Imap"), 1, rg);
   e_widget_on_change_hook_set(ob, _type_cb_change, cfdata);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, _("Maildir"), 2, rg);
   e_widget_on_change_hook_set(ob, _type_cb_change, cfdata);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, _("Mbox"), 3, rg);
   e_widget_on_change_hook_set(ob, _type_cb_change, cfdata);
   e_widget_framelist_object_append(of, ob);
   e_widget_list_object_append(o, of, 1, 1, 0.5);

   of = e_widget_frametable_add(evas, _("Port Settings"), 1);
   ob = e_widget_label_add(evas, _("Port:"));
   e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 0, 0, 1, 0);
   ob = e_widget_entry_add(evas, &cfdata->port);
   cfdata->port_entry = ob;
   e_widget_frametable_object_append(of, ob, 1, 0, 1, 1, 0, 0, 1, 0);

   ob = e_widget_label_add(evas, _("Use SSL:"));
   e_widget_frametable_object_append(of, ob, 0, 1, 1, 1, 0, 0, 1, 0);   
   ob = e_widget_check_add(evas, "", &(cfdata->ssl));
   e_widget_frametable_object_append(of, ob, 1, 1, 1, 1, 0, 0, 1, 0);
   e_widget_list_object_append(o, of, 1, 1, 0.5);
   
   of = e_widget_frametable_add(evas, _("Mailbox Settings"), 1);
   ob = e_widget_label_add(evas, _("Name:"));
   e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 0, 0, 1, 0);
   ob = e_widget_entry_add(evas, &cfdata->name);
   e_widget_frametable_object_append(of, ob, 1, 0, 1, 1, 0, 0, 1, 0);

   ob = e_widget_label_add(evas, _("Mail Host:"));
   e_widget_frametable_object_append(of, ob, 0, 1, 1, 1, 0, 0, 1, 0);
   ob = e_widget_entry_add(evas, &cfdata->host);
   e_widget_frametable_object_append(of, ob, 1, 1, 1, 1, 0, 0, 1, 0);

   ob = e_widget_label_add(evas, _("Username:"));
   e_widget_frametable_object_append(of, ob, 0, 2, 1, 1, 0, 0, 1, 0);
   ob = e_widget_entry_add(evas, &cfdata->user);
   e_widget_frametable_object_append(of, ob, 1, 2, 1, 1, 0, 0, 1, 0);

   ob = e_widget_label_add(evas, _("Password:"));
   e_widget_frametable_object_append(of, ob, 0, 3, 1, 1, 0, 0, 1, 0);
   ob = e_widget_entry_add(evas, &cfdata->pass);
   e_widget_frametable_object_append(of, ob, 1, 3, 1, 1, 0, 0, 1, 0);

   cfdata->new_path_label = e_widget_label_add(evas, _("New Mail Path:"));
   e_widget_frametable_object_append(of, cfdata->new_path_label, 0, 4, 1, 1, 0, 0, 1, 0);
   cfdata->new_path_entry = e_widget_entry_add(evas, &cfdata->new_path);
   e_widget_frametable_object_append(of, cfdata->new_path_entry, 1, 4, 1, 1, 0, 0, 1, 0);
   if (cfdata->type == 0) 
     {
	e_widget_disabled_set(cfdata->new_path_label, 1);
	e_widget_disabled_set(cfdata->new_path_entry, 1);
     }

   cfdata->cur_path_label = e_widget_label_add(evas, _("Current Mail Path:"));
   e_widget_frametable_object_append(of, cfdata->cur_path_label, 0, 5, 1, 1, 0, 0, 1, 0);
   cfdata->cur_path_entry = e_widget_entry_add(evas, &cfdata->cur_path);
   e_widget_frametable_object_append(of, cfdata->cur_path_entry, 1, 5, 1, 1, 0, 0, 1, 0);
   if ((cfdata->type == 1) || (cfdata->type == 3)) 
     {
	e_widget_disabled_set(cfdata->cur_path_label, 1);
	e_widget_disabled_set(cfdata->cur_path_entry, 1);
     }
   
   e_widget_list_object_append(o, of, 1, 1, 0.5);
   return o;
}

static int
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata) 
{
   Config_Box *cb;
   int is_new = 0;

   if (cfdata->name == NULL) 
     {
	e_module_dialog_show(_("Enlightenment Mail Module"),
			     _("You must enter a name for this mailbox."));
	return 0;
     }
   
   cb = cfd->data;
   if (!cb) 
     {
	cb = E_NEW(Config_Box, 1);
	cb->type = 0;
	cb->port = 110;
	cb->ssl = 0;
	is_new = 1;
     }
   
   if (cb->name) evas_stringshare_del(cb->name);
   if (cfdata->name != NULL)
     cb->name = evas_stringshare_add(cfdata->name);
   else
     cb->name = evas_stringshare_add("");
   
   cb->type = cfdata->type;
   cb->port = atoi(cfdata->port);
   cb->ssl = cfdata->ssl;
   cb->use_exec = cfdata->use_exec;
   if (cb->exec) evas_stringshare_del(cb->exec);   
   if (cfdata->exec != NULL)
     cb->exec = evas_stringshare_add(cfdata->exec);
   else
     cb->exec = evas_stringshare_add("");

   if (cb->host) evas_stringshare_del(cb->host);
   if (cfdata->host != NULL)
     cb->host = evas_stringshare_add(cfdata->host);
   else
     cb->host = evas_stringshare_add("");
   
   if (cb->user) evas_stringshare_del(cb->user);
   if (cfdata->user != NULL)
     cb->user = evas_stringshare_add(cfdata->user);
   else
     cb->user = evas_stringshare_add("");

   if (cb->pass) evas_stringshare_del(cb->pass);
   if (cfdata->pass != NULL)
     cb->pass = evas_stringshare_add(cfdata->pass);
   else
     cb->pass = evas_stringshare_add("");
   
   if (cb->new_path) evas_stringshare_del(cb->new_path);
   if (cfdata->new_path != NULL)
     cb->new_path = evas_stringshare_add(cfdata->new_path);
   else
     cb->new_path = evas_stringshare_add("");

   if (cb->cur_path) evas_stringshare_del(cb->cur_path);
   if (cfdata->cur_path != NULL)
     cb->cur_path = evas_stringshare_add(cfdata->cur_path);
   else
     cb->cur_path = evas_stringshare_add("");
   
   e_config_save_queue();

   _mail_box_config_updated(prev_dlg);

   if (is_new) 
     {
	mail_ci->boxes = evas_list_append(mail_ci->boxes, cb);
	e_config_save_queue();
	_mail_box_added(mail_ci->id, cb->name);
     }
   
   _mail_box_config_updated(prev_dlg);
   return 1;
}

static void 
_type_cb_change(void *data, Evas_Object *obj) 
{
   E_Config_Dialog_Data *cfdata;
   
   cfdata = data;
   if (cfdata->type == 0) 
     {
	e_widget_disabled_set(cfdata->new_path_label, 1);
	e_widget_disabled_set(cfdata->new_path_entry, 1);
	e_widget_disabled_set(cfdata->cur_path_label, 1);
	e_widget_disabled_set(cfdata->cur_path_entry, 1);
	e_widget_entry_text_set(cfdata->new_path_entry, "");
	e_widget_entry_text_set(cfdata->cur_path_entry, "");
	e_widget_entry_text_set(cfdata->port_entry, "110");
	cfdata->port = strdup("110");
     }
   else if ((cfdata->type == 1) || (cfdata->type == 3))
     {
	e_widget_disabled_set(cfdata->new_path_label, 0);
	e_widget_disabled_set(cfdata->new_path_entry, 0);
	e_widget_disabled_set(cfdata->cur_path_label, 1);
	e_widget_disabled_set(cfdata->cur_path_entry, 1);
	e_widget_entry_text_set(cfdata->cur_path_entry, "");
	e_widget_entry_text_set(cfdata->new_path_entry, _("Inbox"));
	e_widget_entry_text_set(cfdata->port_entry, "143");
	cfdata->port = strdup("143");
     }
   else if (cfdata->type == 2) 
     {
	e_widget_disabled_set(cfdata->new_path_label, 0);
	e_widget_disabled_set(cfdata->new_path_entry, 0);
	e_widget_disabled_set(cfdata->cur_path_label, 0);
	e_widget_disabled_set(cfdata->cur_path_entry, 0);	
     }
}

static void 
_use_exec_cb_change(void *data, Evas_Object *obj) 
{
   E_Config_Dialog_Data *cfdata;
   
   cfdata = data;
   e_widget_disabled_set(cfdata->exec_label, !cfdata->use_exec);
   e_widget_disabled_set(cfdata->exec_entry, !cfdata->use_exec);
   if (!cfdata->use_exec)
     e_widget_entry_text_set(cfdata->exec_entry, "");
}
