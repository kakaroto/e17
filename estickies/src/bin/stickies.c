#include "stickies.h"
#include <fcntl.h>
#include <unistd.h>

E_Stickies *ss;
static Evas_Object *dialog;
static Eina_Bool _e_sticky_is_moving = EINA_FALSE;
static int _e_sticky_mouse_x = 0;
static int _e_sticky_mouse_y = 0;
const char *home;

static void _e_stickies_exit_cb(void *data, Evas_Object *obj, void *event_info);

static void
_e_stickies_exit_cb(void *data, Evas_Object *obj, void *event_info)
{
   elm_exit();
}

/* START HELPER CALLBACKS */

static void _e_sticky_new_show_append_cb(void *data, Evas_Object *obj, void *event_info);
static void _e_config_save_cb(void *data, Evas_Object *obj, void *event_info);
static void _e_sticky_export_to_cb(void *data, Evas_Object *obj, void *event_info);
static void _e_theme_chooser_show_cb(void *data, Evas_Object *obj, void *event_info);
static void _e_about_show_cb(void *data, Evas_Object *obj, void *event_info);
static void _e_sticky_lock_toggle_cb(void *data, Evas_Object *obj, void *event_info);
static void _e_sticky_export_fd_cb(void *data, Evas_Object *obj, void *event_info);
static void _e_sticky_delete_dialog_cb(void *data, Evas_Object *obj, void *event_info);
static void _e_sticky_check_cb(void *data, Evas_Object *obj, void *event_info);

static void
_e_sticky_new_show_append_cb(void *data, Evas_Object *obj, void *event_info)
{
   _e_sticky_new_show_append();
}

static void
_e_config_save_cb(void *data, Evas_Object *obj, void *event_info)
{
   _e_config_save(data);
}

static void
_e_sticky_export_to_cb(void *data, Evas_Object *obj, void *event_info)
{
   _e_sticky_export_to(data);
}

static void
_e_theme_chooser_show_cb(void *data, Evas_Object *obj, void *event_info)
{
   _e_theme_chooser_show(data);
}

static void
_e_about_show_cb(void *data, Evas_Object *obj, void *event_info)
{
   _e_about_show();
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

/* END HELPER CALLBACKS */

/* START WINDOW, KEYBOARD AND MOUSE RELATED FUNCTIONS AND CALLBACKS */

static void _e_sticky_sticky_cb(void *data, int type, void *event);
static void _e_sticky_key_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _e_sticky_mouse_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _e_sticky_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _e_sticky_mouse_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _e_sticky_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _e_sticky_resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _e_sticky_delete_event_cb(void *data, Evas_Object *obj, void *event_info);

// callbacks
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

static void 
_e_sticky_sticky_cb(void *data, int type, void *event)
{
   E_Sticky *s = data;
   Ecore_X_Event_Window_Property *pp = event;
   if (pp->atom == ECORE_X_ATOM_NET_WM_STATE)
     {
	s->stick = elm_win_sticky_get(s->win);
	elm_check_state_set(s->stick_toggle, s->stick);
     }
}

static void 
_e_sticky_check_cb(void *data, Evas_Object *obj, void *event_info)
{
   E_Sticky *s = data;

   s->stick = !elm_win_sticky_get(s->win);
   elm_win_sticky_set(s->win, s->stick);
}

// functions
void
_e_sticky_move(E_Sticky *s, int x, int y)
{
   printf("%i %i %i %i\n", s->w, s->h, s->x, s->y);
   evas_object_move(s->win, x, y);
   s->x = x;
   s->y = y;
}

void
_e_sticky_resize(E_Sticky *s, int w, int h)
{
   printf("%i %i %i %i\n", s->w, s->h, s->x, s->y);
   evas_object_resize(s->win, s->w, s->h);
   s->w = w;
   s->h = h;
}

void
_e_sticky_move_resize(E_Sticky *s)
{
   if (!s || !s->win) return;
   printf("%i %i %i %i\n", s->w, s->h, s->x, s->y);
   evas_object_resize(s->win, s->w, s->h);
   evas_object_move(s->win, s->x, s->y);
}

void
_e_sticky_lock_toggle(E_Sticky *s)
{   
   s->locked = elm_check_state_get(s->lock_toggle);
   elm_entry_editable_set(s->textentry, !s->locked);
   if (s->locked) 
     elm_entry_editable_set(s->textentry, 0);
   else
     elm_entry_editable_set(s->textentry, 1);
}

void
_e_sticky_properties_set(E_Sticky *s)
{   
   elm_win_sticky_set(s->win, s->stick);
   elm_check_state_set(s->lock_toggle, s->locked);
   _e_sticky_lock_toggle(s);
}

/* END WINDOW, KEYBOARD AND MOUSE RELATED FUNCTIONS AND CALLBACKS */

/* START STICKY RELATED FUNCTIONS */

E_Sticky *
_e_sticky_new(void)
{  
   E_Sticky *s = E_NEW(1, E_Sticky);

   s->theme = NULL;
   s->stick = EINA_FALSE;
   s->locked = EINA_FALSE;
   _e_sticky_window_add(s);

   return s;
}

E_Sticky *
_e_sticky_new_show_append(void)
{
   E_Sticky *s = _e_sticky_new();

   ss->stickies = eina_list_append(ss->stickies, s);
   _e_sticky_show(s);

   return s;
}

void
_e_sticky_destroy(E_Sticky *s)
{
   evas_object_del(s->win);
   E_FREE(s);
}

void
_e_sticky_delete(E_Sticky *s)
{
   if(eina_list_count(ss->stickies) == 1)
     {
	ss->stickies = eina_list_remove(ss->stickies, s);
	evas_object_del(s->win);
	s = _e_sticky_new();
	ss->stickies = eina_list_append(ss->stickies, s);
	_e_sticky_show(s);
	return;
     }
   ss->stickies = eina_list_remove(ss->stickies, s);
   evas_object_del(s->win);

   if(!ss->stickies || eina_list_count(ss->stickies) == 0)
     elm_exit();
}

/* END STICKY RELATED FUNCTIONS */

/* START GUI RELATED FUNCTIONS */

static void _e_sticky_delete_confirm_cb(void *data, Evas_Object *obj, void *event_info);
static void _e_sticky_switch_pages(void *data, Evas_Object *obj, void *event_info);

// callbacks
static void
_e_sticky_switch_pages(void *data, Evas_Object *obj, void *event_info)
{
   E_Sticky *s = data;

   elm_box_unpack_all(s->layoutbox);

   if (s->list_shown)
     {
	evas_object_hide(s->list);
	elm_box_pack_end(s->layoutbox, s->scroller);
	evas_object_show(s->scroller);
     }
   else
     {
	evas_object_hide(s->scroller);
	elm_box_pack_end(s->layoutbox, s->list);
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

// functions
void
_e_sticky_window_add(E_Sticky *s)
{
   char theme[PATH_MAX];

   if (!s->theme) s->theme = strdup(ss->theme);

   snprintf(theme, sizeof(theme), "%s/themes/%s", PACKAGE_DATA_DIR, s->theme);
   elm_theme_extension_add(theme);
   //////////
   s->win = elm_win_add(NULL, "estickies", ELM_WIN_BASIC);
   elm_win_title_set(s->win, "estickies");
   elm_win_borderless_set(s->win, 1);

   evas_object_smart_callback_add(s->win, "delete-request", _e_sticky_delete_event_cb, s);
   ecore_event_handler_add(ECORE_X_EVENT_WINDOW_PROPERTY, _e_sticky_sticky_cb, s);

   evas_object_show(s->win);
   //////////
   s->layout = elm_layout_add(s->win);
   elm_layout_file_set(s->layout, theme, "estickies/base");

   Evas_Modifier_Mask mask;
   mask = evas_key_modifier_mask_get(evas_object_evas_get(s->win), "Control");
   evas_object_key_grab(s->layout, "n", mask, 0, 0);
   evas_object_key_grab(s->layout, "d", mask, 0, 0);
   evas_object_key_grab(s->layout, "q", mask, 0, 0);
   evas_object_key_grab(s->layout, "a", mask, 0, 0);
   evas_object_size_hint_weight_set(s->layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_event_callback_add(s->layout, EVAS_CALLBACK_KEY_DOWN, _e_sticky_key_down_cb, s);
   evas_object_event_callback_add(s->layout, EVAS_CALLBACK_MOUSE_DOWN, _e_sticky_mouse_down_cb, s);
   evas_object_event_callback_add(s->layout, EVAS_CALLBACK_MOUSE_UP, _e_sticky_mouse_up_cb, s);
   evas_object_event_callback_add(s->layout, EVAS_CALLBACK_MOUSE_MOVE, _e_sticky_mouse_move_cb, s);
   evas_object_event_callback_add(s->layout, EVAS_CALLBACK_MOVE, _e_sticky_move_cb, s);
   evas_object_event_callback_add(s->layout, EVAS_CALLBACK_RESIZE, _e_sticky_resize_cb, s);

   elm_win_resize_object_add(s->win, s->layout);

   evas_object_show(s->layout);
   //////////
   s->stick_toggle = elm_check_add(s->win);
   elm_check_label_set(s->stick_toggle, NULL);
   elm_object_style_set(s->stick_toggle, "stick_toggle");

   evas_object_smart_callback_add(s->stick_toggle, "changed", _e_sticky_check_cb, s);

   elm_layout_content_set(s->layout, "bt_sticky", s->stick_toggle);

   evas_object_show(s->stick_toggle);
   //////////
   s->lock_toggle = elm_check_add(s->win);
   elm_check_label_set(s->lock_toggle, NULL);
   elm_object_style_set(s->lock_toggle, "lock_toggle");

   evas_object_smart_callback_add(s->lock_toggle, "changed", _e_sticky_lock_toggle_cb, s);

   elm_layout_content_set(s->layout, "bt_locked", s->lock_toggle);

   evas_object_show(s->lock_toggle);
   //////////
   s->close_button = elm_button_add(s->win);
   elm_button_label_set(s->close_button, NULL);
   elm_object_style_set(s->close_button, "close_button");

   evas_object_smart_callback_add(s->close_button, "clicked", _e_sticky_delete_dialog_cb, s);

   elm_layout_content_set(s->layout, "bt_close", s->close_button);

   evas_object_show(s->close_button);
   //////////
   s->actions_toggle = elm_check_add(s->win);
   elm_check_label_set(s->actions_toggle, NULL);
   elm_object_style_set(s->actions_toggle, "actions_toggle");

   evas_object_smart_callback_add(s->actions_toggle, "changed", _e_sticky_switch_pages, s);

   elm_layout_content_set(s->layout, "bt_actions", s->actions_toggle);

   evas_object_show(s->actions_toggle);
   //////////
   s->layoutbox = elm_box_add(s->win);
   elm_layout_content_set(s->layout, "main_area", s->layoutbox);

   evas_object_show(s->layoutbox);
   //////////
   s->list = elm_list_add(s->win);
   elm_list_multi_select_set(s->list, 0);

   evas_object_size_hint_weight_set(s->list, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(s->list, EVAS_HINT_FILL, EVAS_HINT_FILL);

   elm_list_item_append(s->list, "New", NULL, NULL, _e_sticky_new_show_append_cb, NULL);
   elm_list_item_append(s->list, "Save", NULL, NULL, _e_config_save_cb, ss);
   elm_list_item_append(s->list, "Export To File", NULL, NULL, _e_sticky_export_to_cb, s);
   elm_list_item_append(s->list, "Delete", NULL, NULL, _e_sticky_delete_dialog_cb, s);
   elm_list_item_append(s->list, "Options", NULL, NULL, _e_theme_chooser_show_cb, s);
   elm_list_item_append(s->list, "About", NULL, NULL, _e_about_show_cb, NULL);
   elm_list_item_append(s->list, "Quit", NULL, NULL, _e_stickies_exit_cb, NULL);

   elm_list_go(s->list);
   evas_object_show(s->list);
   //////////
   s->scroller = elm_scroller_add(s->win);
   elm_scroller_policy_set(s->scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
   elm_scroller_bounce_set(s->scroller, 0, 0);

   evas_object_size_hint_weight_set(s->scroller, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(s->scroller, EVAS_HINT_FILL, EVAS_HINT_FILL);

   evas_object_show(s->scroller);
   //////////
   s->textentry = elm_entry_add(s->win);
   if (s->text)
     elm_entry_entry_set(s->textentry, s->text);
   else
     elm_entry_entry_set(s->textentry, NULL);
   elm_entry_line_wrap_set(s->textentry, 1);

   evas_object_size_hint_weight_set(s->textentry, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(s->textentry, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_pass_events_set(s->textentry, 1);

   elm_scroller_content_set(s->scroller, s->textentry);

   evas_object_show(s->textentry);
   //////////
   elm_box_pack_end(s->layoutbox, s->scroller);
   s->list_shown = EINA_FALSE;

   evas_object_resize(s->win, 208, 206);
}

void
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
   elm_win_title_set(fd->dia, "Estickies export to ..");
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
   elm_check_label_set(fd->export_mode, "All stickies");
   elm_check_state_set(fd->export_mode, 0);

   elm_box_pack_end(vbox, fd->export_mode);

   evas_object_show(fd->export_mode);

   evas_object_resize(fd->dia, 240, 350);
}

void
_e_sticky_delete_confirm(E_Sticky *s)
{
   Evas_Object *background, *frame, *vbox, *label, *hbox, *button_yes, *button_no;

   dialog = elm_win_add(NULL, "estickies-delete-confirm", ELM_WIN_DIALOG_BASIC);
   elm_win_title_set(dialog, "Confirm Deletion");
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
   elm_label_label_set(label, "Are you sure you want to delete this sticky?");

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
   elm_button_label_set(button_no, "No");

   evas_object_smart_callback_add(button_no, "clicked", _e_sticky_delete_confirm_cb, NULL);

   elm_box_pack_end(hbox, button_no);

   evas_object_show(button_no);
   //////////
   button_yes = elm_button_add(dialog);
   elm_button_label_set(button_yes, "Yes");

   evas_object_smart_callback_add(button_yes, "clicked", _e_sticky_delete_confirm_cb, s);

   elm_box_pack_end(hbox, button_yes);

   evas_object_show(button_yes); 
}

/* END GUI RELATED FUNCTIONS */

/* START OTHER STUFF */

void
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
	WARN("return!");
	return;
     }

   evas_object_hide(fd->dia);

   if ((fh = fopen(basename, "w")) == NULL) return;

   if (!elm_check_state_get(fd->export_mode))
     {
	/* save only current sticky */
	fprintf(fh, "Sticky %d\n=========================\n", c);
	text = strdup(elm_entry_markup_to_utf8(elm_entry_entry_get(s->textentry)));
	fprintf (fh, "%s\n\n", text);	
     }
   else
     {
	/* save all stickies */
	EINA_LIST_FOREACH(ss->stickies, l, s)
	  {	     
	     fprintf(fh, "Sticky %d\n=========================\n", c);
	     text = strdup(elm_entry_markup_to_utf8(elm_entry_entry_get(s->textentry)));
	     fprintf (fh, "%s\n\n", text);
	     ++c;
	  }	
     }

   E_FREE(text);
   fclose(fh);  
}

static void
_e_sticky_export_fd_cb(void *data, Evas_Object *obj, void *event_info)
{
   _e_sticky_export_cb(data);
}

void
_e_sticky_show(E_Sticky *s)
{
   if(s && s->win)
     {
	evas_object_show(s->win);
	evas_object_show(s->background);
	evas_object_show(s->textentry);
	evas_object_show(s->layout);
	if (elm_check_state_get(s->actions_toggle))
	  evas_object_show(s->list);
	else
	  evas_object_show(s->scroller);
	evas_object_show(s->vbox);
	evas_object_show(s->stick_toggle);
	evas_object_show(s->lock_toggle);
	evas_object_show(s->close_button);
	evas_object_show(s->actions_toggle);
	evas_object_show(s->layoutbox);
     }
}

void
_e_sticky_theme_apply(E_Sticky *s, const char *theme)
{
   char theme_file[PATH_MAX];

   if (!theme) return;
   if (!strcmp(s->theme, theme)) return;

   snprintf(theme_file, sizeof(theme_file), "%s/themes/%s", PACKAGE_DATA_DIR, theme);
   if (!ecore_file_exists(theme_file)) return;

   E_FREE(s->theme);
   s->theme = strdup(theme);

   elm_theme_extension_add(theme_file);
   elm_layout_file_set(s->layout, theme_file, "estickies/base");
}

void
_e_sticky_theme_apply_all(const char *theme)
{
   Eina_List *l;
   E_Sticky *data;

   EINA_LIST_FOREACH(ss->stickies, l, data)
     _e_sticky_theme_apply(data, theme);
}

/* END OTHER STUFF */

#ifndef ELM_LIB_QUICKLAUNCH
EAPI int
elm_main(int argc, char **argv)
{
   E_Sticky *s;

   ss = E_NEW(1, E_Stickies);
   ss->stickies = NULL;

   home = getenv("HOME");
   if (!home)
     {
	ERROR("Cant find home directory!");
	return 1;
     }

   if (!eet_init()) return 1;
   if (!_e_config_init()) return 1;
   _e_config_load(ss);

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
