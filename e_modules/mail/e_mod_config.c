#include <e.h>
#include "e_mod_main.h"

struct _E_Config_Dialog_Data 
{
   int show_label;
   int use_exec;
   int use_ssl;
   int type;
   double check_time;
   
   char *exec;   
   char *host;
   char *user;
   char *pass;
   char *path;
   char *path_current;
   char *port;
   
   Evas_Object *new_path_label;
   Evas_Object *new_path_entry;
   Evas_Object *cur_path_label;
   Evas_Object *cur_path_entry;

   Evas_Object *exec_label;
   Evas_Object *exec_entry;
};

static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);

static void _use_exec_cb_change(void *data, Evas_Object *obj);
static void _mail_type_cb_change(void *data, Evas_Object *obj);

void
_config_mail_module(Config_Item *ci) 
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;
   E_Container *con;
   
   v = E_NEW(E_Config_Dialog_View, 1);
   if (!v) return;
   
   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.apply_cfdata = _basic_apply_data;
   v->basic.create_widgets = _basic_create_widgets;
   
   con = e_container_current_get(e_manager_current_get());
   cfd = e_config_dialog_new(con, _("Mail Configuration"), NULL, 0, v, ci);
   mail_config->config_dialog = cfd;
}

static void
_fill_data(Config_Item *ci, E_Config_Dialog_Data *cfdata) 
{
   char buf[1024];
   
   cfdata->host = NULL;
   cfdata->user = NULL;
   cfdata->pass = NULL;
   cfdata->exec = NULL;
   cfdata->path = NULL;
   cfdata->path_current = NULL;
   cfdata->port = NULL;
   
   if (ci->host)
     cfdata->host = strdup(ci->host);
   if (ci->user)
     cfdata->user = strdup(ci->user);
   if (ci->pass)
     cfdata->pass = strdup(ci->pass);
   if (ci->exec)
     cfdata->exec = strdup(ci->exec);
   if (ci->path)
     cfdata->path = strdup(ci->path);
   if (ci->path_current)
     cfdata->path_current = strdup(ci->path_current);

   cfdata->show_label = ci->show_label;
   cfdata->type = ci->type;
   cfdata->use_exec = ci->use_exec;
   cfdata->check_time = ci->check_time;
   cfdata->use_ssl = ci->use_ssl;
   
   snprintf(buf, sizeof(buf), "%d", ci->port);
   cfdata->port = strdup(buf);
}

static void *
_create_data(E_Config_Dialog *cfd) 
{
   E_Config_Dialog_Data *cfdata;
   Config_Item *ci;
   
   ci = cfd->data;
   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   _fill_data(ci, cfdata);
   return cfdata;
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata) 
{
   if (!mail_config) return;
   mail_config->config_dialog = NULL;
   free(cfdata);
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata) 
{
   Evas_Object *o, *of, *ob, *ot;
   E_Radio_Group *rg;
   
   o = e_widget_list_add(evas, 0, 0);
   of = e_widget_framelist_add(evas, _("General Settings"), 0);
   ob = e_widget_check_add(evas, _("Always Show Labels"), &(cfdata->show_label));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_label_add(evas, _("Check Interval"));
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_slider_add(evas, 1, 0, _("%2.0f minutes"), 1.0, 60.0, 1.0, 0, &(cfdata->check_time), NULL, 100);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_check_add(evas, _("Start Program When New Mail Arrives"), &(cfdata->use_exec));
   e_widget_framelist_object_append(of, ob);
   e_widget_on_change_hook_set(ob, _use_exec_cb_change, cfdata);

   ot = e_widget_table_add(evas, 1);
   cfdata->exec_label = e_widget_label_add(evas, _("Mail Program:"));
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
   e_widget_on_change_hook_set(ob, _mail_type_cb_change, cfdata);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, _("Imap"), 1, rg);
   e_widget_on_change_hook_set(ob, _mail_type_cb_change, cfdata);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, _("Maildir"), 2, rg);
   e_widget_on_change_hook_set(ob, _mail_type_cb_change, cfdata);
   e_widget_framelist_object_append(of, ob);
   ob = e_widget_radio_add(evas, _("Mbox"), 3, rg);
   e_widget_on_change_hook_set(ob, _mail_type_cb_change, cfdata);
   e_widget_framelist_object_append(of, ob);

   e_widget_list_object_append(o, of, 1, 1, 0.5);   
   
   of = e_widget_frametable_add(evas, _("Mailbox Settings"), 1);
   ob = e_widget_label_add(evas, _("Port:"));
   e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 0, 0, 1, 0);
   ob = e_widget_entry_add(evas, &cfdata->port);
   e_widget_frametable_object_append(of, ob, 1, 0, 1, 1, 0, 0, 1, 0);

   ob = e_widget_check_add(evas, _("Use SSL"), &(cfdata->use_ssl));
   e_widget_frametable_object_append(of, ob, 0, 1, 1, 1, 0, 0, 1, 0);

   ob = e_widget_label_add(evas, _("Mail Host:"));
   e_widget_frametable_object_append(of, ob, 0, 3, 1, 1, 0, 0, 1, 0);
   ob = e_widget_entry_add(evas, &cfdata->host);
   e_widget_frametable_object_append(of, ob, 1, 3, 1, 1, 0, 0, 1, 0);

   ob = e_widget_label_add(evas, _("Username:"));
   e_widget_frametable_object_append(of, ob, 0, 4, 1, 1, 0, 0, 1, 0);
   ob = e_widget_entry_add(evas, &cfdata->user);
   e_widget_frametable_object_append(of, ob, 1, 4, 1, 1, 0, 0, 1, 0);

   ob = e_widget_label_add(evas, _("Password:"));
   e_widget_frametable_object_append(of, ob, 0, 5, 1, 1, 0, 0, 1, 0);
   ob = e_widget_entry_add(evas, &cfdata->pass);
   e_widget_entry_password_set(ob, 1);
   e_widget_frametable_object_append(of, ob, 1, 5, 1, 1, 0, 0, 1, 0);
   
   cfdata->new_path_label = e_widget_label_add(evas, _("New Mail Path:"));
   e_widget_frametable_object_append(of, cfdata->new_path_label, 0, 6, 1, 1, 0, 0, 1, 0);
   cfdata->new_path_entry = e_widget_entry_add(evas, &cfdata->path);
   e_widget_frametable_object_append(of, cfdata->new_path_entry, 1, 6, 1, 1, 0, 0, 1, 0);
   if (cfdata->type == 0) 
     {
	e_widget_disabled_set(cfdata->new_path_label, 1);
	e_widget_disabled_set(cfdata->new_path_entry, 1);
     }

   cfdata->cur_path_label = e_widget_label_add(evas, _("Current Mail Path:"));
   e_widget_frametable_object_append(of, cfdata->cur_path_label, 0, 7, 1, 1, 0, 0, 1, 0);
   cfdata->cur_path_entry = e_widget_entry_add(evas, &cfdata->path_current);
   e_widget_frametable_object_append(of, cfdata->cur_path_entry, 1, 7, 1, 1, 0, 0, 1, 0);
   if ((cfdata->type == 0) || (cfdata->type == 2)) 
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
   Config_Item *ci;
   
   ci = cfd->data;

   if (ci->host) evas_stringshare_del(ci->host);
   if (cfdata->host != NULL)
     ci->host = evas_stringshare_add(cfdata->host);
   else
     ci->host = evas_stringshare_add("");

   if (ci->user) evas_stringshare_del(ci->user);
   if (cfdata->user != NULL)
     ci->user = evas_stringshare_add(cfdata->user);
   else
     ci->user = evas_stringshare_add("");

   if (ci->pass) evas_stringshare_del(ci->pass);
   if (cfdata->pass != NULL)
     ci->pass = evas_stringshare_add(cfdata->pass);
   else
     ci->pass = evas_stringshare_add("");

   if (ci->path) evas_stringshare_del(ci->path);
   if (cfdata->path != NULL)
     ci->path = evas_stringshare_add(cfdata->path);
   else
     ci->path = evas_stringshare_add("");

   if (ci->path_current) evas_stringshare_del(ci->path_current);
   if (cfdata->path_current != NULL)
     ci->path_current = evas_stringshare_add(cfdata->path_current);
   else
     ci->path_current = evas_stringshare_add("");

   if (ci->exec) evas_stringshare_del(ci->exec);   
   if (cfdata->exec != NULL)
     ci->exec = evas_stringshare_add(cfdata->exec);
   else
     ci->exec = evas_stringshare_add("");

   ci->show_label = cfdata->show_label;
   ci->type = cfdata->type;
   ci->use_exec = cfdata->use_exec;
   ci->check_time = cfdata->check_time;

   if (cfdata->port != NULL)
     ci->port = atoi(cfdata->port);
   else
     ci->port = 110;

   ci->use_ssl = cfdata->use_ssl;
   
   e_config_save_queue();
   _mail_config_updated(ci->id);
   return 1;
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

static void 
_mail_type_cb_change(void *data, Evas_Object *obj) 
{
   E_Config_Dialog_Data *cfdata;
   
   cfdata = data;
   if ((cfdata->type == 0) || (cfdata->type == 1))
     {
	e_widget_disabled_set(cfdata->new_path_label, 1);
	e_widget_disabled_set(cfdata->new_path_entry, 1);
	e_widget_disabled_set(cfdata->cur_path_label, 1);
	e_widget_disabled_set(cfdata->cur_path_entry, 1);
	e_widget_entry_text_set(cfdata->new_path_entry, "");
	e_widget_entry_text_set(cfdata->cur_path_entry, "");
     }
   else if (cfdata->type == 3)
     {
	e_widget_disabled_set(cfdata->new_path_label, 0);
	e_widget_disabled_set(cfdata->new_path_entry, 0);
	e_widget_disabled_set(cfdata->cur_path_label, 1);
	e_widget_disabled_set(cfdata->cur_path_entry, 1);
	e_widget_entry_text_set(cfdata->cur_path_entry, "");	
     }
   else if (cfdata->type == 2) 
     {
	e_widget_disabled_set(cfdata->new_path_label, 0);
	e_widget_disabled_set(cfdata->new_path_entry, 0);
	e_widget_disabled_set(cfdata->cur_path_label, 0);
	e_widget_disabled_set(cfdata->cur_path_entry, 0);	
     }
}

