#include <Ecore_Str.h>
#include "e.h"
#include "eco_config.h"

static void *_create_data(E_Config_Dialog *cfd);
static void  _fill_data(E_Config_Dialog_Data *cfdata);
static void  _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static int   _basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);

E_Config_Dialog_Data *dialog_data;

static void (*_eco_apply_func)(E_Config_Dialog_Data *cfdata); //Pointer to the apply function to call
static void (*_eco_cleanup_func)(void);
static const char *current_search;
Ecore_Timer *timer;

//Match dialog objects
Evas_Object *o_type_normal;
Evas_Object *o_type_dialog;
Evas_Object *o_type_modal_dialog;
Evas_Object *o_type_fullscreen;
Evas_Object *o_type_utility;
Evas_Object *o_type_unknown;
Evas_Object *o_type_menu;
Evas_Object *o_type_popup_menu;
Evas_Object *o_type_dropdown_menu;
Evas_Object *o_type_tooltip;
Evas_Object *o_type_notification;
Evas_Object *o_type_toolbar;
Evas_Object *o_name_entry;
int _eco_type_normal;
int _eco_type_dialog;
int _eco_type_modal_dialog;
int _eco_type_fullscreen;
int _eco_type_utility;
int _eco_type_menu;
int _eco_type_popup_menu;
int _eco_type_dropdown_menu;
int _eco_type_tooltip;
int _eco_type_notification;
int _eco_type_toolbar;
int _eco_type_unknown;


Eco_Group *cfg_screen;
Eco_Group *cfg_display;
//static char *eco_config_filename = NULL;
static Eet_File *eco_config_file = NULL;
static char file_path[2048] = "";
EAPI int
eco_config_file_open()
{
  printf("load config\n");
  
  if (!eco_config_file)
    {
      snprintf(file_path, 2048, "%s/%s", e_user_homedir_get(), ".ecomp/ecomp.eet");
      eco_config_file = eet_open(file_path, EET_FILE_MODE_READ_WRITE);
    }

  printf("loaded %s %d\n", file_path, eco_config_file ? 1 : 0);
  
  return eco_config_file ? 1 : 0;  
}

EAPI void
eco_config_file_close()
{
  if (eco_config_file)
    {
      printf("close eet file\n");
      
      eet_close(eco_config_file);
    }
  eco_config_file = NULL;
  
}

EAPI void
eco_config_group_open(const char *group)
{ 
  char group_display[1024];
  char group_screen[1024];
  snprintf(group_screen, 1024, "%s-screen0", group); 
  snprintf(group_display, 1024, "%s-allscreens", group); 

  if (_eco_cleanup_func) _eco_cleanup_func();
  eco_config_group_close();
  
  if (eco_config_file)
    {
      cfg_display = eet_data_read(eco_config_file, eco_edd_group, group_display);
      cfg_screen  = eet_data_read(eco_config_file, eco_edd_group, group_screen);
    }
  if (cfg_display)
    {
      printf("loaded %s:%d\n", group_display, cfg_display ? 1 : 0);
    }
  else
    {
      printf("create %s\n", group_display);
      cfg_display = calloc(1, sizeof(Eco_Group));
      cfg_display->data = eina_hash_string_superfast_new(NULL);
    }
  if (cfg_screen)
    {
      printf("loaded %s:%d\n", group_screen, cfg_screen ? 1 : 0);      
    }
  else
    {
      printf("create %s\n", group_screen);
      cfg_screen = calloc (1, sizeof(Eco_Group));
      cfg_screen->data = eina_hash_string_superfast_new(NULL);
    }
}

EAPI void
eco_config_group_apply(const char *group)
{
  char group_display[1024];
  char group_screen[1024];
  snprintf(group_screen, 1024, "%s-screen0", group); 
  snprintf(group_display, 1024, "%s-allscreens", group); 
  printf("write %s - %s\n", group_screen, group_display);
  
  if (!eet_data_write(eco_config_file, eco_edd_group, group_display, cfg_display, 1))
    fprintf(stderr, "Error writing data! - Display\n");
  if (!eet_data_write(eco_config_file, eco_edd_group, group_screen, cfg_screen, 1))
    fprintf(stderr, "Error writing data! - Screen\n");

  int err = eet_close(eco_config_file);
  printf("ERROR: %d\n", err);
  
  eco_config_file = eet_open(file_path, EET_FILE_MODE_READ_WRITE);
}


static Eina_Bool
_eco_free_group(const Eina_Hash *hash, const void *key, void *data, void *fdata)
{
  Eco_Option *opt = data;
  Eco_Option *item;
  
  if (opt->stringValue) free (opt->stringValue);
  if (opt->listValue)
    {
      EINA_LIST_FREE(opt->listValue, item)
	{
	  if (item->stringValue) free (item->stringValue);
	  free(item);
	}
    }
  free(opt);
  return 1;
}

EAPI void
eco_config_group_close()
{
  printf("close group\n");
  
  if (cfg_screen)
    {
      eina_hash_foreach(cfg_screen->data,  _eco_free_group, NULL);
      free(cfg_screen);
      cfg_screen  = NULL;
    }
  if (cfg_display)
    {
      eina_hash_foreach(cfg_display->data, _eco_free_group, NULL);
      free(cfg_display);
      cfg_display = NULL;
    }
}

EAPI Eco_Option *
eco_config_option_get(Eco_Group *group, const char *option)
{
  Eco_Option *opt;
  if (!(opt = eina_hash_find(group->data, option)))
    {
      printf("create option %s\n", option);
      opt = calloc (1, sizeof(Eco_Option));      
      eina_hash_add(group->data, option, opt);
    }
  else
    printf("got option %s\n", option);

  return opt;  
}

EAPI Eco_Option *
eco_config_option_list_nth(Eco_Group *group, const char *option, int num)
{
  Eco_Option *opt;
  if (!(opt = eina_hash_find(group->data, option)))
    {
      printf("create option %s\n", option);
      opt = calloc (1, sizeof(Eco_Option));      
      eina_hash_add(group->data, option, opt);
    }
  else
    printf("got option %s\n", option);

  return eina_list_nth(opt->listValue, num);
}

EAPI Eco_Option *
eco_config_option_list_add(Eco_Group *group, const char *option)
{
  Eco_Option *opt = eco_config_option_get(group, option);
  Eco_Option *item = calloc (1, sizeof (Eco_Option));
  opt->listValue = eina_list_append(opt->listValue, item);
  return item;
}

EAPI Eco_Option *
eco_config_option_list_del(Eco_Group *group, const char *option, int num)
{
  Eco_Option *opt = eco_config_option_get(cfg_screen, option);
  Eco_Option *item = eina_list_nth(opt->listValue, num);
  if (item)
    {
      opt->listValue = eina_list_remove(opt->listValue, item);
      if (item->stringValue) free (item->stringValue);
      free(item);
    }
}


EAPI void eco_cleanup_func_set(void cleanup_func(void))
{
  _eco_cleanup_func = cleanup_func;
}




void eco_match_dialog_update(void *data, Evas_Object *obj)
{
   Evas_Object *entry = data;
   char match[4096];
   char types[4096];
   const char *name;

   match[0] = '\0';
   types[0] = '\0';

   if (e_widget_check_checked_get(o_type_normal)) strcat(types, "Normal | ");
   if (e_widget_check_checked_get(o_type_dialog)) strcat(types, "Dialog | ");
   if (e_widget_check_checked_get(o_type_modal_dialog)) strcat(types, "ModalDialog | ");
   if (e_widget_check_checked_get(o_type_utility)) strcat(types, "Utility | ");
   if (e_widget_check_checked_get(o_type_fullscreen)) strcat(types, "Fullscreen | ");
   if (e_widget_check_checked_get(o_type_menu)) strcat(types, "Menu | ");
   if (e_widget_check_checked_get(o_type_popup_menu)) strcat(types, "PopupMenu | ");
   if (e_widget_check_checked_get(o_type_dropdown_menu)) strcat(types, "DropdownMenu | ");
   if (e_widget_check_checked_get(o_type_tooltip)) strcat(types, "Tooltip | ");
   if (e_widget_check_checked_get(o_type_notification)) strcat(types, "Notification | ");
   if (e_widget_check_checked_get(o_type_toolbar)) strcat(types, "Toolbar | ");
   if (e_widget_check_checked_get(o_type_unknown)) strcat(types, "Unknown | ");
   if (strlen(types) > 3) types[strlen(types) - 3] = '\0';

   name = e_widget_entry_text_get(o_name_entry);

   if (name && strlen(name))
      snprintf(match, 4096, "(class=%s)", name);
   else if (strlen(types))
      snprintf(match, 4096, "(type=%s)", types);

   e_widget_entry_text_set(entry, match);
}

EAPI void
eco_match_dialog(const char *val, void *_ok_func)
{
   E_Dialog *dia;
   Evas_Object *ob, *li, *ta, *entry;
   char *name;

   dia = e_dialog_new(e_container_current_get(e_manager_current_get()), "E", "_match_dialog");
   e_dialog_title_set(dia, "Match dialog");

   li = e_widget_list_add(dia->win->evas, 0, 0);

   //Manual edit create
   entry = e_widget_entry_add(dia->win->evas, NULL, NULL, NULL, NULL);
   e_widget_entry_text_set(entry, val);

   //Match by type
   char buf[1024];
   char buf2[1024];
   
   sscanf(val, "(type=%[^)])", buf);
   snprintf(buf2, 1024, " %s ", buf);

   _eco_type_normal = strstr(buf2, " Normal ") ?  1 :  0;
   _eco_type_dialog = strstr(buf2, " Dialog ") ?  1 :  0;
   _eco_type_modal_dialog = strstr(buf2, " ModalDialog ") ?  1 :  0;
   _eco_type_fullscreen = strstr(buf2, " Fullscreen ") ?  1 :  0;
   _eco_type_utility = strstr(buf2, " Utility ") ?  1 :  0;
   _eco_type_menu = strstr(buf2, " Menu ") ?  1 :  0;
   _eco_type_popup_menu = strstr(buf2, " PopupMenu ") ?  1 :  0;
   _eco_type_dropdown_menu = strstr(buf2, " DropdownMenu ") ?  1 :  0;
   _eco_type_tooltip = strstr(buf2, " Tooltip ") ?  1 :  0;
   _eco_type_notification = strstr(buf2, " Notification ") ?  1 :  0;
   _eco_type_toolbar = strstr(buf2, " Toolbar ") ?  1 :  0;
   _eco_type_unknown = strstr(buf2, " Unknown ") ?  1 :  0;

   ta = e_widget_frametable_add(dia->win->evas, _("Match by type"), 0);

   o_type_normal = e_widget_check_add(dia->win->evas, _("Normal"), &_eco_type_normal);
   e_widget_on_change_hook_set(o_type_normal, eco_match_dialog_update, entry);
   e_widget_frametable_object_append(ta, o_type_normal, 0, 0, 1, 1, 1, 0, 0, 0);
   o_type_dialog = e_widget_check_add(dia->win->evas, _("Dialog"), &_eco_type_dialog);
   e_widget_on_change_hook_set(o_type_dialog, eco_match_dialog_update, entry);
   e_widget_frametable_object_append(ta, o_type_dialog, 1, 0, 1, 1, 1, 0, 0, 0);
   o_type_modal_dialog = e_widget_check_add(dia->win->evas, _("Modal Dialog"), &_eco_type_modal_dialog);
   e_widget_on_change_hook_set(o_type_modal_dialog, eco_match_dialog_update, entry);
   e_widget_frametable_object_append(ta, o_type_modal_dialog, 2, 0, 1, 1, 1, 0, 0, 0);
   o_type_utility = e_widget_check_add(dia->win->evas, _("Utility"), &_eco_type_utility);
   e_widget_on_change_hook_set(o_type_utility, eco_match_dialog_update, entry);
   e_widget_frametable_object_append(ta, o_type_utility, 0, 2, 1, 1, 1, 0, 0, 0);
   o_type_menu = e_widget_check_add(dia->win->evas, _("Menu"), &_eco_type_menu);
   e_widget_on_change_hook_set(o_type_menu, eco_match_dialog_update, entry);
   e_widget_frametable_object_append(ta, o_type_menu, 0, 1, 1, 1, 1, 0, 0, 0);
   o_type_popup_menu = e_widget_check_add(dia->win->evas, _("Popup Menu"), &_eco_type_popup_menu);
   e_widget_on_change_hook_set(o_type_popup_menu, eco_match_dialog_update, entry);
   e_widget_frametable_object_append(ta, o_type_popup_menu, 1, 1, 1, 1, 1, 0, 0, 0);
   o_type_dropdown_menu = e_widget_check_add(dia->win->evas, _("Dropdown Menu"), &_eco_type_dropdown_menu);
   e_widget_on_change_hook_set(o_type_dropdown_menu, eco_match_dialog_update, entry);
   e_widget_frametable_object_append(ta, o_type_dropdown_menu, 2, 1, 1, 1, 1, 0, 0, 0);
   o_type_tooltip = e_widget_check_add(dia->win->evas, _("Tooltip"), &_eco_type_tooltip);
   e_widget_on_change_hook_set(o_type_tooltip, eco_match_dialog_update, entry);
   e_widget_frametable_object_append(ta, o_type_tooltip, 1, 3, 1, 1, 1, 0, 0, 0);
   o_type_notification = e_widget_check_add(dia->win->evas, _("Notification"), &_eco_type_notification);
   e_widget_on_change_hook_set(o_type_notification, eco_match_dialog_update, entry);
   e_widget_frametable_object_append(ta, o_type_notification, 0, 3, 1, 1, 1, 0, 0, 0);
   o_type_toolbar = e_widget_check_add(dia->win->evas, _("Toolbar"), &_eco_type_toolbar);
   e_widget_on_change_hook_set(o_type_toolbar, eco_match_dialog_update, entry);
   e_widget_frametable_object_append(ta, o_type_toolbar, 1, 2, 1, 1, 1, 0, 0, 0);
   o_type_fullscreen = e_widget_check_add(dia->win->evas, _("Fullscreen"), &_eco_type_fullscreen);
   e_widget_on_change_hook_set(o_type_fullscreen, eco_match_dialog_update, entry);
   e_widget_frametable_object_append(ta, o_type_fullscreen, 2, 2, 1, 1, 1, 0, 0, 0);
   o_type_unknown = e_widget_check_add(dia->win->evas, _("Unknown"), &_eco_type_unknown);
   e_widget_on_change_hook_set(o_type_unknown, eco_match_dialog_update, entry);
   e_widget_frametable_object_append(ta, o_type_unknown, 2, 3, 1, 1, 1, 0, 0, 0);

   e_widget_list_object_append(li, ta, 1, 1, 0.0);

   //Match by class
   ta = e_widget_frametable_add(dia->win->evas, _("Match by class name"), 0);
   o_name_entry = e_widget_entry_add(dia->win->evas, NULL, NULL, NULL, NULL);
   e_widget_on_change_hook_set(o_name_entry, eco_match_dialog_update, entry);
   name = strstr(val, "class=");
   if (name)
   {
      if (name[strlen(name)-1] == ')') name[strlen(name)-1] = '\0';
      name += 6;
   }
   e_widget_entry_text_set(o_name_entry, name);
   e_widget_frametable_object_append(ta, o_name_entry, 0, 0, 1, 1, 1, 0, 1, 0);
   e_widget_list_object_append(li, ta, 1, 0, 0.0);

   //Manual edit attach
   ta = e_widget_frametable_add(dia->win->evas, _("Manual edit"), 0);
   e_widget_frametable_object_append(ta, entry, 0, 0, 1, 1, 1, 0, 1, 0);
   e_widget_list_object_append(li, ta, 1, 0, 0.0);

   e_dialog_content_set(dia, li, 360, 240);
   e_dialog_button_add(dia, _("Ok"), NULL, _ok_func, entry);
   e_dialog_button_add(dia, _("Cancel"), NULL, NULL, NULL);
   e_dialog_show(dia);
}

EAPI void
eco_attach_widget(Evas_Object *sub, void apply_func(E_Config_Dialog_Data *cfdata))
{
   //clear the current content
   if (dialog_data->o_content)
   {
      e_widget_sub_object_del(dialog_data->o_container, dialog_data->o_content);
      evas_object_del(dialog_data->o_content);
   }
   if (sub)
   {
      //and attach to the custom container widget
      dialog_data->o_content = sub;
      e_widget_sub_object_add(dialog_data->o_container, sub);
      e_widget_resize_object_set(dialog_data->o_container, sub);
      evas_object_show(sub);
   }
   _eco_apply_func = apply_func;
}



////////////////////////////////////////////////////////////////////////////////
static int
_eco_check_ecomorph(void *data)
{
   E_Config_Dialog_Data *cfdata = data;

   if (system("pidof ecomorph"))
   {
      e_widget_disabled_set(cfdata->o_stop, 1);
      e_widget_disabled_set(cfdata->o_start, 0);
   }
   else
   {
      e_widget_disabled_set(cfdata->o_stop, 0);
      e_widget_disabled_set(cfdata->o_start, 1);
   }

   return 1; //repeat the timer
}

static void
_eco_start_ecomorph(void *data, void *data2)
{
   ecore_exe_run("ecomorph.sh", NULL);
}

static void
_eco_stop_ecomorph(void *data, void *data2)
{
   ecore_exe_run("killall ecomorph", NULL);
}

static void
eco_list_populate(Evas_Object *list)
{
   Evas_Object *ico;

   //General
   e_widget_ilist_header_append(list, NULL, _("Ecomorph"));
   
   ico = e_icon_add(dialog_data->evas);
   e_icon_file_edje_set(ico, edje_file, "icon");
   e_widget_ilist_append(list, ico, _("General"), eco_config_general, dialog_data, NULL);

   ico = e_icon_add(dialog_data->evas);
   e_icon_file_edje_set(ico, edje_file, "icon");
   e_widget_ilist_append(list, ico, _("Window opacity"), eco_config_opacity, dialog_data, NULL);

   ico = e_icon_add(dialog_data->evas);
   e_icon_file_edje_set(ico, edje_file, "icon");
   e_widget_ilist_append(list, ico, _("Window Move/Resize"), eco_config_move, dialog_data, NULL);

   //Animations
   e_widget_ilist_header_append(list, NULL, _("Animations"));

   ico = e_icon_add(dialog_data->evas);
   e_icon_file_edje_set(ico, edje_file, "icon_animation");
   e_widget_ilist_append(list, ico, _("Open animation"), eco_config_animation_open, dialog_data, NULL);
   
   ico = e_icon_add(dialog_data->evas);
   e_icon_file_edje_set(ico, edje_file, "icon_animation");
   e_widget_ilist_append(list, ico, _("Close animation"), eco_config_animation_close, dialog_data, NULL);
   
   ico = e_icon_add(dialog_data->evas);
   e_icon_file_edje_set(ico, edje_file, "icon_animation");
   e_widget_ilist_append(list, ico, _("Minimize animation"), eco_config_animation_minimize, dialog_data, NULL);

   ico = e_icon_add(dialog_data->evas);
   e_icon_file_edje_set(ico, edje_file, "icon_animation");
   e_widget_ilist_append(list, ico, _("Effect settings (1)"), eco_config_animation, dialog_data, NULL);

   ico = e_icon_add(dialog_data->evas);
   e_icon_file_edje_set(ico, edje_file, "icon_animation");
   e_widget_ilist_append(list, ico, _("Effect settings (2)"), eco_config_animation3, dialog_data, NULL);

   ico = e_icon_add(dialog_data->evas);
   e_icon_file_edje_set(ico, edje_file, "icon_animation");
   e_widget_ilist_append(list, ico, _("Effect settings (3)"), eco_config_animation4, dialog_data, NULL);

   ico = e_icon_add(dialog_data->evas);
   e_icon_file_edje_set(ico, edje_file, "icon_animation");
   e_widget_ilist_append(list, ico, _("Effect Settings (4)"), eco_config_animation5, dialog_data, NULL);

   //Switchers
   e_widget_ilist_header_append(list, NULL, _("Switchers"));

   ico = e_icon_add(dialog_data->evas);
   e_icon_file_edje_set(ico, edje_file, "icon_scale");
   e_widget_ilist_append(list, ico, _("Scale 'Exposè' effect"), eco_config_scale, dialog_data, NULL);

   ico = e_icon_add(dialog_data->evas);
   e_icon_file_edje_set(ico, edje_file, "icon_scale");
   e_widget_ilist_append(list, ico, _("Scale Addons"), eco_config_scaleaddon, dialog_data, NULL);

   ico = e_icon_add(dialog_data->evas);
   e_icon_file_edje_set(ico, edje_file, "icon_switcher");
   e_widget_ilist_append(list, ico, _("Application switcher"), eco_config_switcher, dialog_data, NULL);

   ico = e_icon_add(dialog_data->evas);
   e_icon_file_edje_set(ico, edje_file, "icon_shift");
   e_widget_ilist_append(list, ico, _("Application Shift switcher"), eco_config_shift, dialog_data, NULL);
   
   ico = e_icon_add(dialog_data->evas);
   e_icon_file_edje_set(ico, edje_file, "icon_ring");
   e_widget_ilist_append(list, ico, _("Application Ring switcher"), eco_config_ring, dialog_data, NULL);

   //Desktop
   e_widget_ilist_header_append(list, NULL, _("Desktop"));

   ico = e_icon_add(dialog_data->evas);
   e_icon_file_edje_set(ico, edje_file, "icon_expo");
   e_widget_ilist_append(list, ico, _("Expo wall"), eco_config_expo, dialog_data, NULL);

   ico = e_icon_add(dialog_data->evas);
   e_icon_file_edje_set(ico, edje_file, "icon_wall");
   e_widget_ilist_append(list, ico, _("Wall of desktop"), eco_config_wall, dialog_data, NULL);

   ico = e_icon_add(dialog_data->evas);
   e_icon_file_edje_set(ico, edje_file, "icon_cube");
   e_widget_ilist_append(list, ico, _("Cube"), eco_config_cube, dialog_data, NULL);

   ico = e_icon_add(dialog_data->evas);
   e_icon_file_edje_set(ico, edje_file, "icon_cube");
   e_widget_ilist_append(list, ico, _("Cube Rotate"), eco_config_rotate, dialog_data, NULL);

   //Effects
   e_widget_ilist_header_append(list, NULL, _("Effects"));

   ico = e_icon_add(dialog_data->evas);
   e_icon_file_edje_set(ico, edje_file, "icon_wobbly");
   e_widget_ilist_append(list, ico, _("Wobbly windows"), eco_config_wobbly, dialog_data, NULL);

   ico = e_icon_add(dialog_data->evas);
   e_icon_file_edje_set(ico, edje_file, "icon_water");
   e_widget_ilist_append(list, ico, _("Water effect"), eco_config_water, dialog_data, NULL);

   ico = e_icon_add(dialog_data->evas);
   e_icon_file_edje_set(ico, edje_file, "icon_blur");
   e_widget_ilist_append(list, ico, _("Blur effect"), eco_config_blur, dialog_data, NULL);

   ico = e_icon_add(dialog_data->evas);
   e_icon_file_edje_set(ico, edje_file, "icon_thumbnail");
   e_widget_ilist_append(list, ico, _("Thumbnail"), eco_config_thumbnail, dialog_data, NULL);

}
////////////////////////////////////////////////////////////////////////////////
EAPI E_Config_Dialog *
e_int_config_eco(E_Container *con, const char *params __UNUSED__)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;

   if (e_config_dialog_find("E", "_config_eco_dialog")) return NULL;
   v = E_NEW(E_Config_Dialog_View, 1);
   if (!v) return NULL;
   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.apply_cfdata = _basic_apply_data;
   v->basic.create_widgets = _basic_create_widgets;
   printf("eco config dialog \n");
   
   eco_config_file_open();
   cfg_screen = NULL;
   cfg_display = NULL;
   
   cfd = e_config_dialog_new(con, _("Ecomorph Configuration"),
                             "E", "_config_eco_dialog",
                             edje_file, 0, v, NULL);
   return cfd;
}

static void *
_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata;

   cfdata = E_NEW(E_Config_Dialog_Data, 1);
   _fill_data(cfdata);
   cfdata->cfd = cfd;

   return cfdata;
}

static void
_fill_data(E_Config_Dialog_Data *cfdata)
{
   cfdata->use_composite = e_config->use_composite;
   cfdata->ecomorph = evil;
   cfdata->o_start = NULL;
   cfdata->o_stop = NULL;
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
  if (_eco_cleanup_func) _eco_cleanup_func();
  
  eco_config_group_close();
  eco_config_file_close();

  ecore_timer_del(timer);
  E_FREE(cfdata);
}

static int
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   if (_eco_apply_func) _eco_apply_func(cfdata);

   e_config->use_composite = cfdata->use_composite;
   e_config_save_queue();

   if (evil != cfdata->ecomorph)
   {      
      E_Action *a;
      evil = cfdata->ecomorph;

      if (!evil)
	{
	   Eina_List *l;
	   E_Border *bd;
   
	   for (l = e_border_client_list(); l; l = l->next)
	     {
		bd = l->data;
    
		bd->changed = 1;
		bd->changes.pos = 1;
		bd->fx.x = 0;
		bd->fx.y = 0;
		
		if ((!bd->desk->visible) && (!bd->sticky))
		  e_border_hide(bd, 1);
		/* if (bd->client.netwm.state.hidden && !bd->client.netwm.state.shaded)
		 *   {
		 *      bd->visible = 1;
		 *      bd->iconic = 0;
		 *      e_border_iconify(bd);
		 *   } */
	     }
	}
      else
	{
	  e_util_env_set("E_ECOMORPH", evil ? "1" : "0");
	  a = e_action_find("restart");
	  if ((a) && (a->func.go)) a->func.go(NULL, NULL);
	}
   }

   return 1;
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *ob, *of, *sf;
   Eina_List *l;
   int engine;

   dialog_data = cfdata;
   dialog_data->evas = evas;
   dialog_data->o_content = NULL;
   o = e_widget_list_add(evas, 0, 1);

   /* Plugins Frame */
   of = e_widget_frametable_add(evas, _("Plugins"), 0);

   //list
   ob = e_widget_ilist_add(evas, 32, 32, NULL);
   e_widget_size_min_set(ob, 200, 330);
   e_widget_frametable_object_append(of, ob, 0, 0, 2, 1, 1, 1, 1, 1);
   eco_list_populate(ob);

   //e configs
   ob = e_widget_check_add(evas, _("Enable Composite"),
                           &(cfdata->use_composite));
   e_widget_frametable_object_append(of, ob, 0, 1, 2, 1, 1, 0, 0, 0);
   
   ob = e_widget_check_add(evas, _("Ecomorph Mode"),
                           &(cfdata->ecomorph));
   e_widget_frametable_object_append(of, ob, 0, 2, 2, 1, 1, 0, 0, 0);
   
   ob = e_widget_button_add(evas, _("Start Ecomorph"), NULL,
                            _eco_start_ecomorph, NULL, NULL);
   e_widget_frametable_object_append(of, ob, 0, 3, 1, 1, 1, 0, 0, 0);
   e_widget_disabled_set(ob, 1);
   cfdata->o_start = ob;
   
   ob = e_widget_button_add(evas, _("Stop Ecomorph"), NULL,
                            _eco_stop_ecomorph, NULL, NULL);
   e_widget_frametable_object_append(of, ob, 1, 3, 1, 1, 1, 0, 0, 0);
   e_widget_disabled_set(ob, 1);
   cfdata->o_stop = ob;

   e_widget_list_object_append(o, of, 1, 0, 0.0);

   //widget container
   dialog_data->o_container = e_widget_add(evas);
   e_widget_size_min_set(dialog_data->o_container, 500, 600);
   
   ob = e_widget_label_add(evas, _("Welcome !!"));
   dialog_data->o_content = ob;
   e_widget_sub_object_add(cfdata->o_container, ob);
   e_widget_resize_object_set(cfdata->o_container, ob);
   evas_object_show(ob);

   e_widget_list_object_append(o, cfdata->o_container, 1, 1, 0.0);

   e_dialog_resizable_set(cfd->dia, 1);

   //Check if ecomorph is running and run the timer
   _eco_check_ecomorph(cfdata);
   timer = ecore_timer_add(3, _eco_check_ecomorph, cfdata);
   return o;
}
