#include "stickies.h"

E_Stickies *ss;
static Evas_Object *dialog;
static Eina_Bool _e_sticky_is_moving = EINA_FALSE;
static int _e_sticky_mouse_x = 0;
static int _e_sticky_mouse_y = 0;
const char *home;

/* INTERNAL FUNCTIONS, USUALLY CALLBACKS PROTOTYPES.
 */
static void _e_stickies_exit_cb(void *data, Evas_Object *obj, void *event_info);

static void _e_sticky_new_show_append_cb(void *data, Evas_Object *obj, void *event_info);
static void _e_config_save_cb(void *data, Evas_Object *obj, void *event_info);
static void _e_sticky_export_to_cb(void *data, Evas_Object *obj, void *event_info);
static void _e_theme_chooser_show_cb(void *data, Evas_Object *obj, void *event_info);
static void _e_about_show_cb(void *data, Evas_Object *obj, void *event_info);
static void _e_sticky_lock_toggle_cb(void *data, Evas_Object *obj, void *event_info);
static void _e_sticky_export_fd_cb(void *data, Evas_Object *obj, void *event_info);
static void _e_sticky_delete_dialog_cb(void *data, Evas_Object *obj, void *event_info);
static void _e_sticky_delete_list_dialog_cb(void *data, Evas_Object *obj, void *event_info);
static void _e_sticky_check_cb(void *data, Evas_Object *obj, void *event_info);

static int _e_sticky_sticky_cb(void *data, int type, void *event);
static void _e_sticky_key_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _e_sticky_mouse_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _e_sticky_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _e_sticky_mouse_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _e_sticky_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _e_sticky_resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _e_sticky_delete_event_cb(void *data, Evas_Object *obj, void *event_info);

static void _e_sticky_delete_confirm_cb(void *data, Evas_Object *obj, void *event_info);
static void _e_sticky_switch_pages(void *data, Evas_Object *obj, void *event_info);

/* FUNCTIONS WHICH HAVE PROTOTYPES DEFINED IN STICKIES.H.
 */
ESAPI void
_e_sticky_window_add(E_Sticky *s)
{
   //////////
   _e_sticky_win_add(s);

   evas_object_smart_callback_add(s->win, "delete,request", _e_sticky_delete_event_cb, s);
   s->evhandler = ecore_event_handler_add(ECORE_X_EVENT_WINDOW_PROPERTY, _e_sticky_sticky_cb, s);

   evas_object_show(s->win);
   //////////
   _e_sticky_edje_add(s);

   evas_object_event_callback_add(s->sticky, EVAS_CALLBACK_KEY_DOWN, _e_sticky_key_down_cb, s);
   evas_object_event_callback_add(s->sticky, EVAS_CALLBACK_MOUSE_DOWN, _e_sticky_mouse_down_cb, s);
   evas_object_event_callback_add(s->sticky, EVAS_CALLBACK_MOUSE_UP, _e_sticky_mouse_up_cb, s);
   evas_object_event_callback_add(s->sticky, EVAS_CALLBACK_MOUSE_MOVE, _e_sticky_mouse_move_cb, s);
   evas_object_event_callback_add(s->sticky, EVAS_CALLBACK_MOVE, _e_sticky_move_cb, s);
   evas_object_event_callback_add(s->sticky, EVAS_CALLBACK_RESIZE, _e_sticky_resize_cb, s);

   evas_object_smart_callback_add(s->sticky, "stick_toggle_changed", _e_sticky_check_cb, s);
   evas_object_smart_callback_add(s->sticky, "lock_toggle_changed", _e_sticky_lock_toggle_cb, s);
   evas_object_smart_callback_add(s->sticky, "actions_toggle_changed", _e_sticky_switch_pages, s);
   evas_object_smart_callback_add(s->sticky, "close_button_clicked", _e_sticky_delete_dialog_cb, s);

   elm_win_resize_object_add(s->win, s->sticky);
   evas_object_show(s->sticky);
   //////////
   s->list = elm_list_add(s->win);
   elm_list_multi_select_set(s->list, 0);

   evas_object_size_hint_weight_set(s->list, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(s->list, EVAS_HINT_FILL, EVAS_HINT_FILL);

   elm_list_item_append(s->list, _("New"), NULL, NULL, _e_sticky_new_show_append_cb, NULL);
   elm_list_item_append(s->list, _("Save"), NULL, NULL, _e_config_save_cb, ss);
   elm_list_item_append(s->list, _("Export To File"), NULL, NULL, _e_sticky_export_to_cb, s);
   elm_list_item_append(s->list, _("Delete"), NULL, NULL, _e_sticky_delete_list_dialog_cb, s);
   elm_list_item_append(s->list, _("Options"), NULL, NULL, _e_theme_chooser_show_cb, s);
   elm_list_item_append(s->list, _("About"), NULL, NULL, _e_about_show_cb, NULL);
   elm_list_item_append(s->list, _("Quit"), NULL, NULL, _e_stickies_exit_cb, NULL);

   elm_list_go(s->list);
   evas_object_show(s->list);
   //////////
   _e_sticky_scroller_add(s);
   evas_object_show(s->scroller);
   //////////
   _e_sticky_entry_add(s);
   elm_scroller_content_set(s->scroller, s->textentry);
   evas_object_show(s->textentry);
   //////////
   edje_object_part_swallow(s->sticky, "main_area", s->scroller);
   s->list_shown = EINA_FALSE;

   evas_object_resize(s->win, 208, 206);
}

ESAPI void
_e_sticky_export_to(E_Sticky *s)
{
   Evas_Object *background, *vbox;
   static E_Filedialog *fd = NULL;

   if (!fd)
     {
	fd = calloc(1, sizeof(E_Filedialog));
	if (!fd) return;
	fd->s = NULL;
	fd->dia = NULL;
     }

   fd->s = s;

   // Don't open more then one window
   if (fd->dia)
     {
	evas_object_show(fd->dia);
	evas_object_show(background);
	evas_object_show(vbox);
	evas_object_show(fd->filechooser);
	evas_object_show(fd->export_mode);
	return;
     }
   //////////
   fd->dia = elm_win_add(NULL, "estickies-export", ELM_WIN_BASIC);
   elm_win_title_set(fd->dia, _("Estickies - Export to .."));
   elm_win_autodel_set(fd->dia, 1);

   evas_object_show(fd->dia);
   //////////
   background = elm_bg_add(fd->dia);

   evas_object_size_hint_weight_set(background, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

   elm_win_resize_object_add(fd->dia, background);

   evas_object_show(background);
   //////////
   vbox = elm_box_add(fd->dia);
   elm_box_horizontal_set(vbox, 0);

   evas_object_size_hint_weight_set(vbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(vbox, EVAS_HINT_FILL, EVAS_HINT_FILL);

   elm_win_resize_object_add(fd->dia, vbox);

   evas_object_show(vbox);
   //////////
   fd->filechooser = elm_fileselector_add(fd->dia);
   elm_fileselector_is_save_set(fd->filechooser, 1);
   elm_fileselector_path_set(fd->filechooser, home);

   evas_object_smart_callback_add(fd->filechooser, "done", _e_sticky_export_fd_cb, fd);

   evas_object_size_hint_weight_set(fd->filechooser, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(fd->filechooser, EVAS_HINT_FILL, EVAS_HINT_FILL);

   elm_box_pack_end(vbox, fd->filechooser);

   evas_object_show(fd->filechooser);
   //////////
   fd->export_mode = elm_check_add(fd->dia);
   elm_check_label_set(fd->export_mode, _("All stickies"));
   elm_check_state_set(fd->export_mode, 0);

   elm_box_pack_end(vbox, fd->export_mode);

   evas_object_show(fd->export_mode);

   evas_object_resize(fd->dia, 240, 350);
}

ESAPI void
_e_sticky_delete_confirm(E_Sticky *s)
{
   Evas_Object *background, *frame, *vbox, *label, *hbox, *button_yes, *button_no;

   dialog = elm_win_add(NULL, "estickies-delete-confirm", ELM_WIN_DIALOG_BASIC);
   elm_win_title_set(dialog, _("Confirm Deletion"));
   elm_win_autodel_set(dialog, 1);

   evas_object_show(dialog);
   //////////
   background = elm_bg_add(dialog);

   elm_win_resize_object_add(dialog, background);

   evas_object_show(background);
   //////////
   frame = elm_frame_add(dialog);

   elm_win_resize_object_add(dialog, frame);

   evas_object_show(frame);
   //////////
   vbox = elm_box_add(dialog);
   elm_box_horizontal_set(vbox, 0);

   elm_frame_content_set(frame, vbox);

   evas_object_show(vbox);
   //////////
   label = elm_label_add(dialog);
   elm_label_label_set(label, _("Are you sure you want to delete this sticky?"));

   elm_box_pack_end(vbox, label);

   evas_object_show(label);
   //////////
   hbox = elm_box_add(dialog);
   elm_box_horizontal_set(hbox, 1);
   elm_box_homogenous_set(hbox, 1);

   elm_box_pack_end(vbox, hbox);

   evas_object_show(hbox);
   //////////
   button_no = elm_button_add(dialog);
   elm_button_label_set(button_no, _("No"));

   evas_object_smart_callback_add(button_no, "clicked", _e_sticky_delete_confirm_cb, NULL);

   elm_box_pack_end(hbox, button_no);

   evas_object_show(button_no);
   //////////
   button_yes = elm_button_add(dialog);
   elm_button_label_set(button_yes, _("Yes"));

   evas_object_smart_callback_add(button_yes, "clicked", _e_sticky_delete_confirm_cb, s);

   elm_box_pack_end(hbox, button_yes);

   evas_object_show(button_yes); 
}

ESAPI void
_e_sticky_export_cb(void *data)
{
   E_Filedialog *fd = data;
   Eina_List *l;
   E_Sticky *s = fd->s;
   FILE *fh;
   int c = 1;
   const char *basename;
   char *text;

   if (!elm_fileselector_selected_get(fd->filechooser))
     {
	evas_object_del(fd->dia);
	return;
     }

   basename = elm_fileselector_selected_get(fd->filechooser);
   
   if (!basename || !s) 
     { 
	WARN(_("return!"));
	return;
     }

   evas_object_hide(fd->dia);

   if ((fh = fopen(basename, "w")) == NULL) return;

   if (!elm_check_state_get(fd->export_mode))
     {
	/* save only current sticky */
	fprintf(fh, _("Sticky %d\n=========================\n"), c);
	text = strdup(elm_entry_markup_to_utf8(elm_entry_entry_get(s->textentry)));
	fprintf (fh, "%s\n\n", text);	
     }
   else
     {
	/* save all stickies */
	EINA_LIST_FOREACH(ss->stickies, l, s)
	  {
	     fprintf(fh, _("Sticky %d\n=========================\n"), c);
	     text = strdup(elm_entry_markup_to_utf8(elm_entry_entry_get(s->textentry)));
	     fprintf (fh, "%s\n\n", text);
	     ++c;
	  }	
     }

   E_FREE(text);
   fclose(fh);  
}

ESAPI void
_e_sticky_show(E_Sticky *s)
{
   if (s && s->win)
     {
	evas_object_show(s->win);
	evas_object_show(s->textentry);
	evas_object_show(s->sticky);
	if (s->actions_toggle_state)
	  evas_object_show(s->list);
	else
	  evas_object_show(s->scroller);
     }
}

ESAPI void
_e_sticky_theme_apply(E_Sticky *s, const char *theme)
{
   char theme_file[PATH_MAX];

   if (!theme) return;
   if (!strcmp(s->theme, theme)) return;

   snprintf(theme_file, sizeof(theme_file), "%s/themes/%s", PACKAGE_DATA_DIR, s->theme);

   snprintf(theme_file, sizeof(theme_file), "%s/themes/%s", PACKAGE_DATA_DIR, theme);
   if (!ecore_file_exists(theme_file)) return;

   E_FREE(s->theme);
   s->theme = strdup(theme);

   edje_object_file_set(s->sticky, theme_file, "base");
}

ESAPI void
_e_sticky_theme_apply_all(const char *theme)
{
   Eina_List *l;
   E_Sticky *data;

   EINA_LIST_FOREACH(ss->stickies, l, data)
     _e_sticky_theme_apply(data, theme);
}

/* CALLBACKS THEMSELVES
 */
static void
_e_stickies_exit_cb(void *data, Evas_Object *obj, void *event_info)
{
   elm_exit();
}

static void
_e_sticky_new_show_append_cb(void *data, Evas_Object *obj, void *event_info)
{
   _e_sticky_new_show_append();
   elm_list_item_selected_set(elm_list_selected_item_get(obj), 0);
}

static void
_e_config_save_cb(void *data, Evas_Object *obj, void *event_info)
{
   _e_config_save(data);
   elm_list_item_selected_set(elm_list_selected_item_get(obj), 0);
}

static void
_e_sticky_export_to_cb(void *data, Evas_Object *obj, void *event_info)
{
   _e_sticky_export_to(data);
   elm_list_item_selected_set(elm_list_selected_item_get(obj), 0);
}

static void
_e_theme_chooser_show_cb(void *data, Evas_Object *obj, void *event_info)
{
   _e_theme_chooser_show(data);
   elm_list_item_selected_set(elm_list_selected_item_get(obj), 0);
}

static void
_e_about_show_cb(void *data, Evas_Object *obj, void *event_info)
{
   _e_about_show();
   elm_list_item_selected_set(elm_list_selected_item_get(obj), 0);
}

static void
_e_sticky_lock_toggle_cb(void *data, Evas_Object *obj, void *event_info)
{
   _e_sticky_lock_toggle(data);
}

static void
_e_sticky_delete_dialog_cb(void *data, Evas_Object *obj, void *event_info)
{
   _e_sticky_delete_confirm(data);
}

static void
_e_sticky_delete_list_dialog_cb(void *data, Evas_Object *obj, void *event_info)
{
   _e_sticky_delete_confirm(data);
   elm_list_item_selected_set(elm_list_selected_item_get(obj), 0);
}

static void
_e_sticky_key_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Key_Down *ev = event_info;
   E_Sticky *s = data;

   if (evas_key_modifier_is_set(ev->modifiers, "Control"))
     {
	if (!strcmp(ev->key, "n"))
	  {
	     E_Sticky *sn;
	     sn = _e_sticky_new();
	     ss->stickies = eina_list_append(ss->stickies, sn);
	     _e_sticky_properties_set(sn);
	     _e_sticky_show(sn);
	  }
	else if (!strcmp(ev->key, "d"))
	  _e_sticky_delete_confirm(s);
	else if (!strcmp(ev->key, "q"))
	  elm_exit();
	else if (!strcmp(ev->key, "a"))
	  elm_entry_select_all(s->textentry);
     }
}

static void
_e_sticky_mouse_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Down *ev = event_info;

   _e_sticky_mouse_x = ev->canvas.x;
   _e_sticky_mouse_y = ev->canvas.y;

   if (ev->button == 1) _e_sticky_is_moving = EINA_TRUE;
}

static void
_e_sticky_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Down *ev = event_info;

   if (ev->button == 1) _e_sticky_is_moving = EINA_FALSE;   
}

static void
_e_sticky_mouse_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   E_Sticky *s = data;

   if (_e_sticky_is_moving)
     {
	     int x = 0;
	     int y = 0;
	     ecore_x_pointer_last_xy_get(&x, &y);
	     _e_sticky_move(s,  x-_e_sticky_mouse_x,  y-_e_sticky_mouse_y);
     }
}

static void
_e_sticky_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   /* FIXME:
    * this bit of code forces estickies to only work on X11.
    * we need to acquire this info through elm.
    */

   E_Sticky *s = data;

   Ecore_X_Window root;
   int x, y, w, h;

   evas_object_geometry_get(s->win, &s->x, &s->y, NULL, NULL);

   root = ecore_x_window_root_first_get();
   ecore_x_window_geometry_get(root, &x, &y, &w, &h);

   if (!E_INTERSECTS(x, y, w, h, s->x, s->y, s->w, s->h))
     {
	_e_sticky_move(s, 0, 0);
     }
}

static void
_e_sticky_resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   E_Sticky *s = data;

   if (!_e_sticky_is_moving) evas_object_geometry_get(s->win, NULL, NULL, &s->w, &s->h);
}

static void
_e_sticky_delete_event_cb(void *data, Evas_Object *obj, void *event_info)
{
   E_Sticky *s = data;

   ss->stickies = eina_list_remove(ss->stickies, s);
}

static int
_e_sticky_sticky_cb(void *data, int type, void *event)
{
   E_Sticky *s = data;
   Ecore_X_Event_Window_Property *pp = event;

   if (pp->atom == ECORE_X_ATOM_NET_WM_STATE)
     {
	s->stick = elm_win_sticky_get(s->win);
	s->stick_toggle_state = s->stick;
	if (s->stick_toggle_state)
	  edje_object_signal_emit(s->sticky, "estickies,stick_toggle,on", "estickies");
	else
	  edje_object_signal_emit(s->sticky, "estickies,stick_toggle,off", "estickies");
     }

   return 1;
}

static void 
_e_sticky_check_cb(void *data, Evas_Object *obj, void *event_info)
{
   E_Sticky *s = data;

   s->stick = !elm_win_sticky_get(s->win);
   elm_win_sticky_set(s->win, s->stick);
}

static void
_e_sticky_switch_pages(void *data, Evas_Object *obj, void *event_info)
{
   E_Sticky *s = data;

   if (s->list_shown)
     {
	evas_object_hide(s->list);
	edje_object_part_unswallow(s->sticky, s->list);
	edje_object_part_swallow(s->sticky, "main_area", s->scroller);
	evas_object_show(s->scroller);
     }
   else
     {
	evas_object_hide(s->scroller);
	edje_object_part_unswallow(s->sticky, s->scroller);
	edje_object_part_swallow(s->sticky, "main_area", s->list);
	evas_object_show(s->list);
     }

   s->list_shown = !s->list_shown;
}

static void 
_e_sticky_delete_confirm_cb(void *data, Evas_Object *obj, void *event_info)
{
   if (data) _e_sticky_delete(data);
   if (dialog) evas_object_del(dialog);
}

static void
_e_sticky_export_fd_cb(void *data, Evas_Object *obj, void *event_info)
{
   _e_sticky_export_cb(data);
}

#ifndef ELM_LIB_QUICKLAUNCH
EAPI int
elm_main(int argc, char **argv)
{
   E_Sticky *s;

   ss = E_NEW(1, E_Stickies);
   ss->stickies = NULL;

  // initialize gettext
  setlocale(LC_ALL, "");
  bindtextdomain(PACKAGE, PACKAGE_LOCALE_DIR);
  textdomain(PACKAGE);

   home = getenv("HOME");
   if (!home)
     {
	ERROR(_("Cant find home directory!"));
	return 1;
     }

   if (!eet_init()) return 1;
   if (!_e_config_init()) return 1;
   _e_config_load(ss);

   elm_finger_size_set(1);

   if (ss->stickies)
     {
	Eina_List *l;
	E_Sticky *data;

	EINA_LIST_FOREACH(ss->stickies, l, data)
	  {
	     _e_sticky_window_add(data);
	     _e_sticky_show(data);
	     _e_sticky_move_resize(data);
	     _e_sticky_properties_set(data);
	  }
     }
   else
     {
	s = _e_sticky_new();
	ss->stickies = eina_list_append(ss->stickies, s);
	_e_sticky_show(s);
	_e_sticky_properties_set(s);
     }
   
   elm_run();
   _e_config_save(ss);
   _e_config_shutdown();
   elm_shutdown();
   eet_shutdown();

   return 0; 
}
#endif
ELM_MAIN()
