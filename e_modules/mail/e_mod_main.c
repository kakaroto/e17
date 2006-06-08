/*
 * Copyright (C) 2006 Christopher Michael
 *
 * Portions of this code Copyright (C) 2004 Embrace project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */

#include <sys/types.h>
#include <dirent.h>
#include <e.h>
#include <Ecore.h>
#include <Ecore_File.h>
#include "e_mod_main.h"

typedef enum 
{
   MAIL_TYPE_POP,
     MAIL_TYPE_IMAP,
     MAIL_TYPE_MDIR,
     MAIL_TYPE_MBOX
} MailType;

typedef enum 
{
   STATE_DISCONNECTED,
     STATE_CONNECTED,
     STATE_SERVER_READY,
     STATE_LOGGED_IN,     
     STATE_USER_OK,
     STATE_PASS_OK,
     STATE_STATUS_OK
} State;

typedef struct _Instance Instance;
typedef struct _Mail Mail;

struct _Instance 
{
   E_Gadcon_Client *gcc;
   Evas_Object *mail_obj;
   Mail *mail;
   Ecore_Timer *check_timer;
   Ecore_Con_Server *server;
   Ecore_Event_Handler *add_handler;
   Ecore_Event_Handler *del_handler;
   Ecore_Event_Handler *data_handler;
   Ecore_File_Monitor *monitor;
   Ecore_Exe *exe;
   Ecore_Event_Handler *exit_handler;
   int state;
   int cmd;
};

struct _Mail
{
   Instance *inst;
   Evas_Object *mail_obj;
};

/* Func Protos for Gadcon */
static E_Gadcon_Client *_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style);
static void _gc_shutdown(E_Gadcon_Client *gcc);
static void _gc_orient(E_Gadcon_Client *gcc);
static char *_gc_label(void);
static Evas_Object *_gc_icon(Evas *evas);

/* Func Protos for Module */
static void _mail_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _mail_cb_mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _mail_cb_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _mail_menu_cb_configure(void *data, E_Menu *m, E_Menu_Item *mi);
static void _mail_menu_cb_post(void *data, E_Menu *m);
static Config_Item *_mail_config_item_get(const char *id);
static Mail *_mail_new(Evas *evas);
static void _mail_free(Mail *mail);
static int _mail_cb_check(void *data);
static int _mail_server_add(void *data, int type, void *event);
static int _mail_server_del(void *data, int type, void *event);
static int _mail_server_data(void *data, int type, void *event);
static int _mail_parse_pop(void *data, void *data2);
static int _mail_parse_imap(void *data, void *data2);
static void _mail_mbox_check(void *data, Ecore_File_Monitor *monitor, Ecore_File_Event event, const char *path);
static void _mail_mdir_check(void *data, Ecore_File_Monitor *monitor, Ecore_File_Event event, const char *path);
static int _mail_mdir_get_files(const char *path);
static void _mail_set_text(void *data, int n, int t);
static int _mail_cb_exe_exit(void *data, int type, void *event);

static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_item_edd = NULL;

Config *mail_config = NULL;

static const E_Gadcon_Client_Class _gc_class = 
{
   GADCON_CLIENT_CLASS_VERSION,
     "mail", {_gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon}
};

static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style) 
{
   Evas_Object *o;
   E_Gadcon_Client *gcc;
   Instance *inst;
   Mail *mail;
   Config_Item *ci;
   char buf[4096];
   
   inst = E_NEW(Instance, 1);
   ci = _mail_config_item_get(id);
   if (!ci->id) ci->id = evas_stringshare_add(id);
   
   mail = _mail_new(gc->evas);
   mail->inst = inst;
   inst->mail = mail;
   
   o = mail->mail_obj;
   gcc = e_gadcon_client_new(gc, name, id, style, o);
   gcc->data = inst;
   inst->gcc = gcc;
   inst->mail_obj = o;
   
   if (ci->type == MAIL_TYPE_MDIR) 
     inst->monitor = ecore_file_monitor_add(ci->path, _mail_mdir_check, inst);
   else if (ci->type == MAIL_TYPE_MBOX) 
     inst->monitor = ecore_file_monitor_add(ci->path, _mail_mbox_check, inst);
   else 
     {
	if (!inst->add_handler)
	  inst->add_handler = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_ADD, _mail_server_add, inst);
	if (!inst->del_handler)
	  inst->del_handler = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DEL, _mail_server_del, inst);
	if (!inst->data_handler)
	  inst->data_handler = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DATA, _mail_server_data, inst);

	_mail_cb_check(inst);
	if (!inst->check_timer)
	  inst->check_timer = ecore_timer_add((ci->check_time * 60.0), _mail_cb_check, inst);
     }
   
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN, _mail_cb_mouse_down, inst);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_IN, _mail_cb_mouse_in, inst);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_OUT, _mail_cb_mouse_out, inst);

   if (ci->show_label)
     edje_object_signal_emit(inst->mail_obj, "label_active", "");
   else
     edje_object_signal_emit(inst->mail_obj, "label_passive", "");

   edje_object_part_text_set(inst->mail_obj, "name", ci->user);
   
   mail_config->instances = evas_list_append(mail_config->instances, inst);
   return gcc;
}

static void
_gc_shutdown(E_Gadcon_Client *gcc) 
{
   Instance *inst;
   
   inst = gcc->data;
   if (inst->monitor) ecore_file_monitor_del(inst->monitor);
   if (inst->check_timer) ecore_timer_del(inst->check_timer);
   if (inst->add_handler) ecore_event_handler_del(inst->add_handler);
   if (inst->data_handler) ecore_event_handler_del(inst->data_handler);
   if (inst->del_handler) ecore_event_handler_del(inst->del_handler);
   if (inst->server) ecore_con_server_del(inst->server);
   inst->server = NULL;

   evas_object_event_callback_del(inst->mail->mail_obj, EVAS_CALLBACK_MOUSE_DOWN, _mail_cb_mouse_down);
   evas_object_event_callback_del(inst->mail->mail_obj, EVAS_CALLBACK_MOUSE_IN, _mail_cb_mouse_in);
   evas_object_event_callback_del(inst->mail->mail_obj, EVAS_CALLBACK_MOUSE_OUT, _mail_cb_mouse_out);   

   mail_config->instances = evas_list_remove(mail_config->instances, inst);
   _mail_free(inst->mail);
   free(inst);
}

static void
_gc_orient(E_Gadcon_Client *gcc) 
{
   e_gadcon_client_aspect_set(gcc, 16, 16);
   e_gadcon_client_min_size_set(gcc, 16, 16);
}

static char *
_gc_label(void) 
{
   return D_("Mail");
}

static Evas_Object *
_gc_icon(Evas *evas) 
{
   Evas_Object *o;
   char buf[4096];
   
   o = edje_object_add(evas);
   snprintf(buf, sizeof(buf), "%s/module.eap", e_module_dir_get(mail_config->module));
   edje_object_file_set(o, buf, "icon");
   return o;
}

static void
_mail_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info) 
{
   Instance *inst;
   Evas_Event_Mouse_Down *ev;
   Config_Item *ci;
   
   inst = data;
   if (!inst) return;
   
   ci = _mail_config_item_get(inst->gcc->id);
   
   ev = event_info;
   if ((ev->button == 3) && (!mail_config->menu)) 
     {
	E_Menu *mn;
	E_Menu_Item *mi;
	int x, y, w, h;
	
	mn = e_menu_new();
	e_menu_post_deactivate_callback_set(mn, _mail_menu_cb_post, inst);
	mail_config->menu = mn;

	mi = e_menu_item_new(mn);
	e_menu_item_label_set(mi, _("Configuration"));
	e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");
	e_menu_item_callback_set(mi, _mail_menu_cb_configure, inst);
	
	mi = e_menu_item_new(mn);
	e_menu_item_separator_set(mi, 1);
	
	e_gadcon_client_util_menu_items_append(inst->gcc, mn, 0);
	e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon, &x, &y, &w, &h);
	e_menu_activate_mouse(mn,
			      e_util_zone_current_get(e_manager_current_get()),
			      x + ev->output.x, y + ev->output.y, 1, 1, E_MENU_POP_DIRECTION_DOWN,
			      ev->timestamp);
	evas_event_feed_mouse_up(inst->gcc->gadcon->evas, ev->button, EVAS_BUTTON_NONE, ev->timestamp, NULL);
     }
   else if ((ev->button == 1) && (inst) && 
	    ((ci->type == 0) || (ci->type == 1)))
     _mail_cb_check(inst);
}

static void 
_mail_cb_mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info) 
{
   Instance *inst;
   
   inst = data;
   edje_object_signal_emit(inst->mail_obj, "label_active", "");
}

static void 
_mail_cb_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info) 
{
   Instance *inst;
   Config_Item *ci;
   
   inst = data;
   ci = _mail_config_item_get(inst->gcc->id);
   if (!ci->show_label)
     edje_object_signal_emit(inst->mail_obj, "label_passive", "");
}

static void
_mail_menu_cb_post(void *data, E_Menu *m) 
{
   if (!mail_config->menu) return;
   e_object_del(E_OBJECT(mail_config->menu));
   mail_config->menu = NULL;
}

static void
_mail_menu_cb_configure(void *data, E_Menu *m, E_Menu_Item *mi) 
{
   Instance *inst;
   Config_Item *ci;
   
   inst = data;
   ci = _mail_config_item_get(inst->gcc->id);
   _config_mail_module(ci);
}

static Config_Item *
_mail_config_item_get(const char *id) 
{
   Evas_List *l;
   Config_Item *ci;
   
   for (l = mail_config->items; l; l = l->next) 
     {
	ci = l->data;
	if (!ci->id) continue;
	if (!strcmp(ci->id, id)) return ci;
     }
   
   ci = E_NEW(Config_Item, 1);
   ci->id = evas_stringshare_add(id);
   ci->host = evas_stringshare_add("");
   ci->port = 110;
   ci->user = evas_stringshare_add("");
   ci->pass = evas_stringshare_add("");
   ci->exec = evas_stringshare_add("");
   ci->path = evas_stringshare_add("");
   ci->path_current = evas_stringshare_add("");
   ci->show_label = 1;
   ci->type = MAIL_TYPE_POP;
   ci->use_exec = 0;
   ci->check_time = 15.0;
   ci->use_ssl = 0;
   
   mail_config->items = evas_list_append(mail_config->items, ci);
   return ci;
}

EAPI E_Module_Api e_modapi = 
{
   E_MODULE_API_VERSION,
     "Mail"
};

EAPI void *
e_modapi_init(E_Module *m) 
{   
   conf_item_edd = E_CONFIG_DD_NEW("Mail_Config_Item", Config_Item);
   #undef T
   #undef D
   #define T Config_Item
   #define D conf_item_edd
   E_CONFIG_VAL(D, T, id, STR);
   E_CONFIG_VAL(D, T, host, STR);
   E_CONFIG_VAL(D, T, port, INT);
   E_CONFIG_VAL(D, T, user, STR);
   E_CONFIG_VAL(D, T, pass, STR);
   E_CONFIG_VAL(D, T, exec, STR);
   E_CONFIG_VAL(D, T, path, STR);
   E_CONFIG_VAL(D, T, path_current, STR);
   E_CONFIG_VAL(D, T, show_label, UCHAR);
   E_CONFIG_VAL(D, T, use_exec, UCHAR);
   E_CONFIG_VAL(D, T, use_ssl, UCHAR);
   E_CONFIG_VAL(D, T, type, INT);
   E_CONFIG_VAL(D, T, check_time, DOUBLE);
   
   conf_edd = E_CONFIG_DD_NEW("Mail_Config", Config);
   #undef T
   #undef D
   #define T Config
   #define D conf_edd
   E_CONFIG_LIST(D, T, items, conf_item_edd);
   
   mail_config = e_config_domain_load("module.mail", conf_edd);
   if (!mail_config) 
     {
	Config_Item *ci;
	
	mail_config = E_NEW(Config, 1);
	ci = E_NEW(Config_Item, 1);
	
	ci->id = evas_stringshare_add("0");
	ci->host = evas_stringshare_add("");
	ci->port = 110;
	ci->user = evas_stringshare_add("");
	ci->pass = evas_stringshare_add("");
	ci->exec = evas_stringshare_add("");
	ci->path = evas_stringshare_add("");
	ci->path_current = evas_stringshare_add("");	
	ci->show_label = 1;
	ci->type = MAIL_TYPE_POP;	
	ci->use_exec = 0;
	ci->check_time = 15.0;
	ci->use_ssl = 0;
	
	mail_config->items = evas_list_append(mail_config->items, ci);
     }
   mail_config->module = m;
   e_gadcon_provider_register(&_gc_class);
   return 1;
}

EAPI int
e_modapi_shutdown(E_Module *m) 
{
   mail_config->module = NULL;
   e_gadcon_provider_unregister(&_gc_class);
   
   if (mail_config->config_dialog)
     e_object_del(E_OBJECT(mail_config->config_dialog));
   if (mail_config->menu) 
     {
	e_menu_post_deactivate_callback_set(mail_config->menu, NULL, NULL);
	e_object_del(E_OBJECT(mail_config->menu));
	mail_config->menu = NULL;
     }
   while (mail_config->items) 
     {
	Config_Item *ci;
	
	ci = mail_config->items->data;
	if (ci->id) evas_stringshare_del(ci->id);
	if (ci->host) evas_stringshare_del(ci->host);
	if (ci->user) evas_stringshare_del(ci->user);
	if (ci->pass) evas_stringshare_del(ci->pass);
	if (ci->exec) evas_stringshare_del(ci->exec);
	if (ci->path) evas_stringshare_del(ci->path);
	if (ci->path_current) evas_stringshare_del(ci->path_current);	
	mail_config->items = evas_list_remove_list(mail_config->items, mail_config->items);
	free(ci);
     }
   free(mail_config);
   mail_config = NULL;
   E_CONFIG_DD_FREE(conf_item_edd);
   E_CONFIG_DD_FREE(conf_edd);
   return 1;
}

EAPI int
e_modapi_save(E_Module *m) 
{
   Evas_List *l;
   
   for (l = mail_config->instances; l; l = l->next) 
     {
	Instance *inst;
	Config_Item *ci;
	
	inst = l->data;
	ci = _mail_config_item_get(inst->gcc->id);
	if (ci->id) evas_stringshare_del(ci->id);
	ci->id = evas_stringshare_add(inst->gcc->id);
     }
   e_config_domain_save("module.mail", conf_edd, mail_config);
   return 1;
}

EAPI int
e_modapi_about(E_Module *m) 
{
   e_module_dialog_show(D_("Enlightenment Mail Module"), 
			D_("This is a module to notify when you have new mail."));
   return 1;
}

static Mail *
_mail_new(Evas *evas)
{
   Mail *mail;
   char buf[4096];

   mail = E_NEW(Mail, 1);
   mail->mail_obj = edje_object_add(evas);

   snprintf(buf, sizeof(buf), "%s/mail.edj", e_module_dir_get(mail_config->module));
   if (!e_theme_edje_object_set(mail->mail_obj, "base/theme/modules/mail", "modules/mail/main"))
     edje_object_file_set(mail->mail_obj, buf, "modules/mail/main");
   evas_object_show(mail->mail_obj);
   
   return mail;
}

static void
_mail_free(Mail *mail) 
{
   evas_object_del(mail->mail_obj);
   free(mail);
}

static int 
_mail_cb_check(void *data) 
{
   Instance *inst;
   Config_Item *ci;
   Ecore_Con_Type type = ECORE_CON_REMOTE_SYSTEM;
   inst = data;
   
   ci = _mail_config_item_get(inst->gcc->id);
   if ((!ci->host) || (!ci->user) || (!ci->pass)) return 0;

   if (inst->server) ecore_con_server_del(inst->server);
   inst->server = NULL;
   inst->state = STATE_DISCONNECTED;
   inst->cmd = 0;
   
   if ((ci->host) && (ci->port != 0)) 
     {	
	if (ci->use_ssl)
	  type |= ECORE_CON_USE_SSL;
	inst->server = ecore_con_server_connect(type, ci->host, ci->port, inst);
	inst->state = STATE_CONNECTED;
     }
   return 1;
}

static int 
_mail_server_add(void *data, int type, void *event) 
{
   Instance *inst;
   Ecore_Con_Event_Server_Add *ev;

   inst = data;
   if (!inst) return 1;
   
   ev = event;
   if ((!inst->server) || (inst->server != ev->server)) return 1;
   inst->state = STATE_CONNECTED;
   inst->cmd = 0;
   return 0;
}

static int 
_mail_server_del(void *data, int type, void *event) 
{
   Instance *inst;
   Config_Item *ci;
   Ecore_Con_Event_Server_Del *ev;

   inst = data;
   if (!inst) return 1;

   ci = _mail_config_item_get(inst->gcc->id);
   
   ev = event;
   if ((!inst->server) || (inst->server != ev->server)) return 1;
   if (!ci->host) return 1;

   if (inst->state == STATE_DISCONNECTED)
     printf("Cannot Connect to %s\n", ci->host);
   else
     inst->state = STATE_DISCONNECTED;
   
   ecore_con_server_del(inst->server);
   inst->server = NULL;
   inst->cmd = 0;
   return 0;
}

static int 
_mail_server_data(void *data, int type, void *event) 
{
   Instance *inst;
   Config_Item *ci;
   Ecore_Con_Event_Server_Data *ev;

   inst = data;
   if (!inst) return 1;
   
   ev = event;
   if ((!inst->server) || (inst->server != ev->server)) return 1;

   ci = _mail_config_item_get(inst->gcc->id);
   
   if (ci->type == MAIL_TYPE_POP) 
     return _mail_parse_pop(inst, ev);
   else if (ci->type == MAIL_TYPE_IMAP)
     return _mail_parse_imap(inst, ev);
}

static int
_mail_parse_pop(void *data, void *data2) 
{
   Instance *inst;
   Config_Item *ci;
   Ecore_Con_Event_Server_Data *ev;
   char in[2048], out[2048];
   int num = 0, size = 0, len;

   inst = data;
   if (!inst) return;
   
   ev = data2;
   if (!ev) return;

   if ((!inst->server) || (inst->server != ev->server)) return;

   _mail_set_text(inst, 0, 0);
   
   ci = _mail_config_item_get(inst->gcc->id);
   
   len = sizeof(in) -1;
   len = (((len) > (ev->size)) ? ev->size : len);

   memcpy(in, ev->data, len);
   in[len] = 0;
   
   if (!strncmp(in, "-ERR", 4)) 
     {
	printf("ERROR: %s\n", in);
	inst->state = STATE_DISCONNECTED;
	ecore_con_server_del(inst->server);
	inst->server = NULL;
	return 0;
     }
   else if (strncmp(in, "+OK", 3)) 
     {
	printf("Unexpected reply: %s\n", in);
	inst->state = STATE_DISCONNECTED;
	ecore_con_server_del(inst->server);
	inst->server = NULL;
	return 0;
     }

   if (inst->state == STATE_CONNECTED)
     inst->state++;
   
   switch (inst->state) 
     {
      case STATE_SERVER_READY:
	len = snprintf(out, sizeof(out), "USER %s\r\n", ci->user);
	ecore_con_server_send(inst->server, out, len);
	inst->state = STATE_USER_OK;
	break;
      case STATE_USER_OK:
	len = snprintf(out, sizeof(out), "PASS %s\r\n", ci->pass);
	ecore_con_server_send(inst->server, out, len);
	inst->state = STATE_PASS_OK;
	break;
      case STATE_PASS_OK:
	len = snprintf(out, sizeof(out), "STAT\r\n");
	ecore_con_server_send(inst->server, out, len);
	inst->state = STATE_STATUS_OK;
	break;
      case STATE_STATUS_OK:
	if (sscanf(in, "+OK %i %i", &num, &size) == 2) 
	  _mail_set_text(inst, num, num); 
	else
	  _mail_set_text(inst, 0, 0);
	
	inst->state = STATE_DISCONNECTED;
	ecore_con_server_del(inst->server);
	inst->server = NULL;
	if ((ci->use_exec) && (ci->exec != NULL)) 
	  {
	     if (num <= 0) break;
	     if (!inst->exe) 
	       {
		  inst->exit_handler = ecore_event_handler_add(ECORE_EXE_EVENT_DEL, _mail_cb_exe_exit, inst);
		  inst->exe = ecore_exe_run(ci->exec, inst);
	       }
	  }
	break;
      default:
	break;
     }
   
   return 0;
}
   
static int
_mail_parse_imap(void *data, void *data2) 
{
   Instance *inst;
   Config_Item *ci;
   Ecore_Con_Event_Server_Data *ev;
   char in[2048], out[2048];
   int total = 0, unread = 0, len;
   char *spc;
   size_t slen;
   
   inst = data;
   if (!inst) return;
   
   ev = data2;
   if (!ev) return;

   if ((!inst->server) || (inst->server != ev->server)) return;

   _mail_set_text(inst, 0, 0);
   
   ci = _mail_config_item_get(inst->gcc->id);
   
   len = sizeof(in) -1;
   len = (((len) > (ev->size)) ? ev->size : len);

   memcpy(in, ev->data, len);
   in[len] = 0;

   if (spc = strchr(in, ' '))
     {
	slen = strlen(spc);
	if ((slen > 5) && (!strncmp(spc + 1, "NO ", 3))) 
	  {
	     len = snprintf(out, sizeof(out), "A%03i LOGOUT", ++inst->cmd);
	     ecore_con_server_send(inst->server, out, len);
	     printf("Imap Failure: %s\n", spc + 4);
	     inst->state = STATE_DISCONNECTED;
	     inst->cmd = 0;
	     return 0;
	  }
	else if ((slen > 6) && (!strncmp(spc + 1, "BAD ", 4))) 
	  {
	     len = snprintf(out, sizeof(out), "A%03i LOGOUT", ++inst->cmd);
	     ecore_con_server_send(inst->server, out, len);
	     printf("Imap Bad Command: %s\n", spc + 5);
	     inst->state = STATE_DISCONNECTED;
	     inst->cmd = 0;
	     return 0;
	  }	
     }

   if (inst->state == STATE_CONNECTED)
     inst->state++;
   
   switch (inst->state) 
     {
      case STATE_SERVER_READY:
	len = snprintf(out, sizeof(out), "A%03i LOGIN %s %s\r\n", ++inst->cmd, ci->user, ci->pass);
	ecore_con_server_send(inst->server, out, len);
	inst->state = STATE_LOGGED_IN;
	break;
      case STATE_LOGGED_IN:
	len = snprintf(out, sizeof(out), "A%03i STATUS %s (MESSAGES UNSEEN)\r\n",++inst->cmd, ci->path);
	ecore_con_server_send(inst->server, out, len);
	inst->state = STATE_STATUS_OK;
	break;
      case STATE_STATUS_OK:
	if (sscanf(in, "* STATUS %*s (MESSAGES %i UNSEEN %i)", &total, &unread) == 2) 
	  _mail_set_text(inst, unread, total);	     
	else 
	  _mail_set_text(inst, 0, 0);

	len = snprintf(out, sizeof(out), "A%03i LOGOUT", ++inst->cmd);
	ecore_con_server_send(inst->server, out, len);	     
	inst->state = STATE_DISCONNECTED;
	inst->cmd = 0;
	break;
      default:
	break;
     }
   
   return 0;
}

static void 
_mail_mbox_check(void *data, Ecore_File_Monitor *monitor, Ecore_File_Event event, const char *path) 
{
   Instance *inst;
   Config_Item *ci;
   FILE *f;
   char buf[1024];
   int total = 0, unread = 0;
   int header;
   
   inst = data;
   if (!inst) return;

   _mail_set_text(inst, 0, 0);
   
   ci = _mail_config_item_get(inst->gcc->id);
   if (!(f = fopen(ci->path, "r")))
     return;
   while (fgets(buf, sizeof(buf), f)) 
     {
	if (buf[0] == '\n')
	  header = 0;
	else if (!strncmp(buf, "From ", 5)) 
	  {
	     header = 1;
	     total++;
	     unread++;
	  }
	else if ((header) && (!strncmp(buf, "Status: ", 7)) && (strchr(buf, 'R')))
	  unread--;
     }
   fclose(f);

   _mail_set_text(inst, unread, total);
}

static void 
_mail_mdir_check(void *data, Ecore_File_Monitor *monitor, Ecore_File_Event event, const char *path) 
{
   Instance *inst;
   Config_Item *ci;   
   int total = 0, unread = 0;
   
   inst = data;
   if (!inst) return;

   _mail_set_text(inst, 0, 0);
   
   ci = _mail_config_item_get(inst->gcc->id);

   total = _mail_mdir_get_files(ci->path_current);
   unread = _mail_mdir_get_files(ci->path);
   _mail_set_text(inst, unread, total);
}

static int 
_mail_mdir_get_files(const char *path) 
{
   Ecore_List *l;
   char *item;
   int i = 0;
   
   l = ecore_file_ls(path);
   ecore_list_goto_first(l);
   while ((item = (char *)ecore_list_next(l)) != NULL) 
     {
	if ((!strcmp(item, ".")) || (!strcmp(item, ".."))) continue;
	i++;
     }
      
   ecore_list_destroy(l);
   return i;
}

static void 
_mail_set_text(void *data, int n, int t) 
{
   Instance *inst;
   Config_Item *ci;   
   char buf[1024];
   
   inst = data;
   if (!inst) return;

   ci = _mail_config_item_get(inst->gcc->id);
   edje_object_part_text_set(inst->mail->mail_obj, "name", ci->user);
   
   snprintf(buf, sizeof(buf), "New: %d", n);
   edje_object_part_text_set(inst->mail->mail_obj, "new_label", buf);
   
   snprintf(buf, sizeof(buf), "Total: %d", t);
   edje_object_part_text_set(inst->mail->mail_obj, "total_label", buf);
   
   if (n > 0)
     edje_object_signal_emit(inst->mail->mail_obj, "new_mail", "");
   else
     edje_object_signal_emit(inst->mail->mail_obj, "no_mail", "");   
}

static int 
_mail_cb_exe_exit(void *data, int type, void *event) 
{
   Instance *inst;
   
   inst = data;
   inst->exe = NULL;
   ecore_event_handler_del(inst->exit_handler);
}

void 
_mail_config_updated(const char *id) 
{
   Evas_List *l;
   Config_Item *ci;
   
   if (!mail_config) return;
   ci = _mail_config_item_get(id);
   for (l = mail_config->instances; l; l = l->next) 
     {
	Instance *inst;
	
	inst = l->data;
	if (!inst->gcc->id) continue;
	if (!strcmp(inst->gcc->id, ci->id)) 
	  {
	     if (inst->check_timer) ecore_timer_del(inst->check_timer);
	     if ((ci->type == 0) || (ci->type == 1)) 
	       inst->check_timer = ecore_timer_add((ci->check_time * 60.0), _mail_cb_check, inst);
	     
	     if (ci->show_label)
	       edje_object_signal_emit(inst->mail_obj, "label_active", "");
	     else
	       edje_object_signal_emit(inst->mail_obj, "label_passive", "");
	     break;
	  }
     }
}
