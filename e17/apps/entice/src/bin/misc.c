#include "entice.h"
#include <Evas_Engine_Software_X11.h>
#include <limits.h>

double
get_time(void)
{
   struct timeval      timev;

   gettimeofday(&timev, NULL);
   return (double)timev.tv_sec + (((double)timev.tv_usec) / 1000000);
}

static Evas_Object *
e_newim(Evas * e, char *file)
{
   int                 iw, ih;
   Evas_Object        *o;

   o = evas_object_image_add(evas);
   evas_object_image_file_set(o, file, NULL);
   evas_object_image_size_get(o, &iw, &ih);
   evas_object_image_fill_set(o, 0, 0, iw, ih);
   evas_object_resize(o, iw, ih);
   return o;
}

void
setup(void)
{
   int                 i, j;
   Ecore_X_Atom        a_entice;
   char                string[] = "entice";

   /* handler for when the event queue goes idle */
   ecore_idle_enterer_add(e_idle, NULL);
   ecore_event_handler_add(ECORE_EVENT_EXE_EXIT, e_child, NULL);
   /* create a 400x300 toplevel window */
   ecore_evas = ecore_evas_software_x11_new(NULL, 0, 0, 0, win_w, win_h);
   ecore_evas_callback_resize_set(ecore_evas, e_window_resize);
   // main_win = ecore_evas_software_x11_window_get(ecore_evas);
   ecore_evas_name_class_set(ecore_evas, "Entice", "Main");
   ecore_evas_size_min_set(ecore_evas, 288, 128);
   ecore_evas_size_max_set(ecore_evas, 8000, 8000);
   a_entice = ecore_x_atom_get("_ENTICE_APP_WINDOW");
   ecore_evas_title_set(ecore_evas, string);
   evas = ecore_evas_get(ecore_evas);
   evas_output_method_set(evas, render_method);
   evas_output_size_set(evas, win_w, win_h);
   evas_output_viewport_set(evas, 0, 0, win_w, win_h);
   evas_font_path_append(evas, FONT_DIRECTORY);
   evas_font_cache_set(evas, MAX_FONT_CACHE);
   evas_image_cache_set(evas, MAX_IMAGE_CACHE);
   ecore_evas_show(ecore_evas);
   /*
   {
      Evas_Engine_Info_Software_X11 *einfo;

      einfo = (Evas_Engine_Info_Software_X11 *) evas_engine_info_get(evas);

      {
	 Display            *disp;

	 * the following is specific to the engine *
	 disp = ecore_x_display_get();
	 einfo->info.display = disp;
	 einfo->info.visual = DefaultVisual(disp, DefaultScreen(disp));
	 einfo->info.colormap = DefaultColormap(disp, DefaultScreen(disp));
	 einfo->info.drawable = win;
	 einfo->info.depth = DefaultDepth(disp, DefaultScreen(disp));
	 einfo->info.rotation = 0;
	 einfo->info.debug = 0;
      }
      evas_engine_info_set(evas, (Evas_Engine_Info *) einfo);
   }
   */
   /* now... create objects in the evas */

   o_bg = e_newim(evas, IM "bg.png");
   o_logo = e_newim(evas, IM "logo.png");
   o_panel = e_newim(evas, IM "panel.png");
   o_showpanel = evas_object_rectangle_add(evas);
   o_hidepanel = evas_object_rectangle_add(evas);
   o_showbuttons = evas_object_rectangle_add(evas);
   o_arrow_l = e_newim(evas, IM "arrow_l.png");
   o_arrow_r = e_newim(evas, IM "arrow_r.png");
   o_arrow_t = e_newim(evas, IM "arrow_t.png");
   o_arrow_b = e_newim(evas, IM "arrow_b.png");
   o_mini_select = e_newim(evas, IM "mini_sel.png");
   o_list_select = e_newim(evas, IM "list_sel.png");
   o_panel_arrow_u = e_newim(evas, IM "list_arrow_u_1.png");
   o_panel_arrow_d = e_newim(evas, IM "list_arrow_d_1.png");
   o_bt_close = e_newim(evas, IM "bt_close_1.png");
   o_bt_delete = e_newim(evas, IM "bt_delete_1.png");
   o_bt_expand = e_newim(evas, IM "bt_expand_1.png");
   o_bt_full = e_newim(evas, IM "bt_full_1.png");
   o_bt_next = e_newim(evas, IM "bt_next_1.png");
   o_bt_prev = e_newim(evas, IM "bt_prev_1.png");
   o_bt_zoom_in = e_newim(evas, IM "bt_zoom_in_1.png");
   o_bt_zoom_normal = e_newim(evas, IM "bt_zoom_normal_1.png");
   o_bt_zoom_out = e_newim(evas, IM "bt_zoom_out_1.png");

   o_trash = e_newim(evas, IM "trash.png");
   evas_object_color_set(o_trash, 255, 255, 255, 0);
   o_trash_can = e_newim(evas, IM "trash_can.png");
   evas_object_color_set(o_trash_can, 255, 255, 255, 0);

   evas_object_color_set(o_logo, 255, 255, 255, 0);
   evas_object_color_set(o_showpanel, 0, 0, 255, 0);
   evas_object_color_set(o_hidepanel, 0, 255, 0, 0);
   evas_object_color_set(o_showbuttons, 255, 0, 0, 0);
   // evas_object_pass_events_set(o_panel, 1);
   for (j = 0; j < 2; j++)
     {
	for (i = 0; i < 5; i++)
	  {
	     o_txt_info[(j * 5) + i] = evas_object_text_add(evas);
	     evas_object_text_font_set(o_txt_info[(j * 5) + i], "nationff", 16);
	     evas_object_pass_events_set(o_txt_info[(j * 5) + i], 1);
	  }
     }
   for (i = 0; i < 5; i++)
     {
	o_txt_tip[i] = evas_object_text_add(evas);
	evas_object_text_font_set(o_txt_tip[i], "nationff", 16);
	evas_object_pass_events_set(o_txt_info[i], 1);
     }

   e_display_current_image();

   evas_object_show(o_bg);
   evas_object_show(o_logo);
   evas_object_show(o_panel);
   evas_object_show(o_showpanel);
   evas_object_show(o_hidepanel);
   evas_object_resize(o_showbuttons, 288, 32);
   evas_object_show(o_showbuttons);

   evas_object_show(o_panel_arrow_u);
   evas_object_show(o_panel_arrow_d);

   evas_object_show(o_bt_close);
   evas_object_show(o_bt_delete);
   evas_object_show(o_bt_expand);
   evas_object_show(o_bt_full);
   evas_object_show(o_bt_next);
   evas_object_show(o_bt_prev);
   evas_object_show(o_bt_zoom_in);
   evas_object_show(o_bt_zoom_normal);
   evas_object_show(o_bt_zoom_out);

   evas_object_show(o_trash);

   {
      enum active_state command = active_out;
      panel_active = active_in;
      buttons_active = active_in;
      e_slide_panel(&command);
      e_slide_buttons(&command);
   }

   evas_object_event_callback_add(o_showpanel, EVAS_CALLBACK_MOUSE_IN,
				  show_panel, NULL);
   evas_object_event_callback_add(o_showpanel, EVAS_CALLBACK_MOUSE_OUT,
				  hide_panel, NULL);
   evas_object_event_callback_add(o_showbuttons, EVAS_CALLBACK_MOUSE_IN,
				  show_buttons, NULL);
   evas_object_event_callback_add(o_showbuttons, EVAS_CALLBACK_MOUSE_OUT,
				  hide_buttons, NULL);
   evas_object_event_callback_add(o_logo, EVAS_CALLBACK_MOUSE_DOWN, next_image,
				  NULL);
   evas_object_event_callback_add(o_logo, EVAS_CALLBACK_MOUSE_UP, next_image_up,
				  NULL);
   evas_object_event_callback_add(o_bg, EVAS_CALLBACK_MOUSE_DOWN, next_image,
				  NULL);
   evas_object_event_callback_add(o_bg, EVAS_CALLBACK_MOUSE_UP, next_image_up,
				  NULL);
   evas_object_event_callback_add(o_bg, EVAS_CALLBACK_KEY_DOWN, e_key_down, NULL);
   evas_object_event_callback_add(o_bg, EVAS_CALLBACK_KEY_UP, e_key_up, NULL);
   evas_object_focus_set(o_bg, 1);
   evas_object_event_callback_add(o_hidepanel, EVAS_CALLBACK_MOUSE_DOWN,
				  next_image, NULL);
   evas_object_event_callback_add(o_hidepanel, EVAS_CALLBACK_MOUSE_UP,
				  next_image_up, NULL);
   evas_object_event_callback_add(o_hidepanel, EVAS_CALLBACK_MOUSE_MOVE,
				  next_image_move, NULL);
   evas_object_event_callback_add(o_showpanel, EVAS_CALLBACK_MOUSE_DOWN,
				  next_image, NULL);
   evas_object_event_callback_add(o_showpanel, EVAS_CALLBACK_MOUSE_UP,
				  next_image_up, NULL);
   evas_object_event_callback_add(o_showpanel, EVAS_CALLBACK_MOUSE_MOVE,
				  next_image_move, NULL);
   evas_object_event_callback_add(o_panel_arrow_u, EVAS_CALLBACK_MOUSE_DOWN,
				  list_scroll_up, NULL);
   evas_object_event_callback_add(o_panel_arrow_u, EVAS_CALLBACK_MOUSE_UP,
				  list_scroll_up_up, NULL);
   evas_object_event_callback_add(o_panel_arrow_d, EVAS_CALLBACK_MOUSE_DOWN,
				  list_scroll_down, NULL);
   evas_object_event_callback_add(o_panel_arrow_d, EVAS_CALLBACK_MOUSE_UP,
				  list_scroll_down_up, NULL);
   evas_object_event_callback_add(o_bt_close, EVAS_CALLBACK_MOUSE_DOWN,
				  bt_close_down, NULL);
   evas_object_event_callback_add(o_bt_close, EVAS_CALLBACK_MOUSE_UP,
				  bt_close_up, NULL);
   evas_object_event_callback_add(o_bt_close, EVAS_CALLBACK_MOUSE_IN,
				  bt_tip_start, "Exit [ Key: Q ]");
   evas_object_event_callback_add(o_bt_close, EVAS_CALLBACK_MOUSE_OUT,
				  bt_tip_stop, NULL);
   evas_object_event_callback_add(o_bt_delete, EVAS_CALLBACK_MOUSE_DOWN,
				  bt_delete_down, NULL);
   evas_object_event_callback_add(o_bt_delete, EVAS_CALLBACK_MOUSE_UP,
				  bt_delete_up, NULL);
   evas_object_event_callback_add(o_bt_delete, EVAS_CALLBACK_MOUSE_IN,
				  bt_tip_start, "Delete [ Key: D ]");
   evas_object_event_callback_add(o_bt_delete, EVAS_CALLBACK_MOUSE_OUT,
				  bt_tip_stop, NULL);
   evas_object_event_callback_add(o_bt_expand, EVAS_CALLBACK_MOUSE_DOWN,
				  bt_expand_down, NULL);
   evas_object_event_callback_add(o_bt_expand, EVAS_CALLBACK_MOUSE_UP,
				  bt_expand_up, NULL);
   evas_object_event_callback_add(o_bt_expand, EVAS_CALLBACK_MOUSE_IN,
				  bt_tip_start,
				  "Scale image to fit entirely in view [ Key: W ]");
   evas_object_event_callback_add(o_bt_expand, EVAS_CALLBACK_MOUSE_OUT,
				  bt_tip_stop, NULL);
   evas_object_event_callback_add(o_bt_full, EVAS_CALLBACK_MOUSE_DOWN,
				  bt_full_down, NULL);
   evas_object_event_callback_add(o_bt_full, EVAS_CALLBACK_MOUSE_UP, bt_full_up,
				  NULL);
   evas_object_event_callback_add(o_bt_full, EVAS_CALLBACK_MOUSE_IN,
				  bt_tip_start,
				  "Toggle fullscreen display [ Key: F ]");
   evas_object_event_callback_add(o_bt_full, EVAS_CALLBACK_MOUSE_OUT,
				  bt_tip_stop, NULL);
   evas_object_event_callback_add(o_bt_next, EVAS_CALLBACK_MOUSE_DOWN,
				  bt_next_down, NULL);
   evas_object_event_callback_add(o_bt_next, EVAS_CALLBACK_MOUSE_UP, bt_next_up,
				  NULL);
   evas_object_event_callback_add(o_bt_next, EVAS_CALLBACK_MOUSE_IN,
				  bt_tip_start,
				  "View next image [ Key: Space ]");
   evas_object_event_callback_add(o_bt_next, EVAS_CALLBACK_MOUSE_OUT,
				  bt_tip_stop, NULL);
   evas_object_event_callback_add(o_bt_prev, EVAS_CALLBACK_MOUSE_DOWN,
				  bt_prev_down, NULL);
   evas_object_event_callback_add(o_bt_prev, EVAS_CALLBACK_MOUSE_UP, bt_prev_up,
				  NULL);
   evas_object_event_callback_add(o_bt_prev, EVAS_CALLBACK_MOUSE_IN,
				  bt_tip_start,
				  "View previous image [ Key: Backspace ]");
   evas_object_event_callback_add(o_bt_prev, EVAS_CALLBACK_MOUSE_OUT,
				  bt_tip_stop, NULL);
   evas_object_event_callback_add(o_bt_zoom_in, EVAS_CALLBACK_MOUSE_DOWN,
				  bt_zoom_in_down, NULL);
   evas_object_event_callback_add(o_bt_zoom_in, EVAS_CALLBACK_MOUSE_UP,
				  bt_zoom_in_up, NULL);
   evas_object_event_callback_add(o_bt_zoom_in, EVAS_CALLBACK_MOUSE_IN,
				  bt_tip_start, "Zoom in one step [ Key: = ]");
   evas_object_event_callback_add(o_bt_zoom_in, EVAS_CALLBACK_MOUSE_OUT,
				  bt_tip_stop, NULL);
   evas_object_event_callback_add(o_bt_zoom_normal, EVAS_CALLBACK_MOUSE_DOWN,
				  bt_zoom_normal_down, NULL);
   evas_object_event_callback_add(o_bt_zoom_normal, EVAS_CALLBACK_MOUSE_UP,
				  bt_zoom_normal_up, NULL);
   evas_object_event_callback_add(o_bt_zoom_normal, EVAS_CALLBACK_MOUSE_IN,
				  bt_tip_start,
				  "Reset zoom to normal 1:1  [ Key: N ]");
   evas_object_event_callback_add(o_bt_zoom_normal, EVAS_CALLBACK_MOUSE_OUT,
				  bt_tip_stop, NULL);
   evas_object_event_callback_add(o_bt_zoom_out, EVAS_CALLBACK_MOUSE_DOWN,
				  bt_zoom_out_down, NULL);
   evas_object_event_callback_add(o_bt_zoom_out, EVAS_CALLBACK_MOUSE_UP,
				  bt_zoom_out_up, NULL);
   evas_object_event_callback_add(o_bt_zoom_out, EVAS_CALLBACK_MOUSE_IN,
				  bt_tip_start, "Zoom out one step [ Key: - ]");
   evas_object_event_callback_add(o_bt_zoom_out, EVAS_CALLBACK_MOUSE_OUT,
				  bt_tip_stop, NULL);
}

void
e_mkdirs(char *path)
{
#ifdef PATH_MAX
   char                ss[PATH_MAX];
#else
   char		       ss[4096];
#endif
   int                 i, ii;

   i = 0;
   ii = 0;
   while (path[i])
     {
	ss[ii++] = path[i];
	ss[ii] = 0;
	if (path[i] == '/')
	  {
	     if (!e_file_exists(ss))
		e_file_mkdir(ss);
	     else if (!e_file_is_dir(ss))
		return;
	  }
	i++;
     }
}

int
e_child(void * data, int ev_type, Ecore_Event * ev)
{
   Ecore_Event_Exe_Exit  *e;

   e = (Ecore_Event_Exe_Exit *) ev;
   if (generating_image)
     {
	if (generating_image->generator == e->pid)
	  {
	    generating_image->generator = 0;
	    if (e->exit_code)
	      image_delete(generating_image);
	    generating_image = NULL;
	  }
     }
   return 1;
}

static int          full = 0;

void
e_toggle_fullscreen(void)
{
   static Ecore_X_Window       win = 0;
   static int          pw = W, ph = H, px = 0, py = 0;
   enum active_state command;

   if (!full)
     {
	ecore_evas_geometry_get(ecore_evas, &px, &py, &pw, &ph);
	full = 1;
	ecore_evas_fullscreen_set(ecore_evas, full);
	ecore_x_window_size_get(0, &win_w, &win_h);
	ecore_evas_resize(ecore_evas, win_w, win_h);
     }
   else
     {
        full = 0;
	ecore_evas_fullscreen_set(ecore_evas, full);
	ecore_evas_move_resize(ecore_evas, px, py, pw, ph);
	win_w = pw;
	win_h = ph;
     }
   e_handle_resize();
   /*
   if (panel_active == active_in || panel_active == active_force_in) {
      command = active_out;
   }
   else {
      command = active_in;
   }
   e_slide_panel(&command);
   */
}

void
e_fix_icons(void)
{
   Evas_List          *l;
   int                 i, im_no;
   double              pos;

   i = 1;
   im_no = 0;
   for (l = images; l; l = l->next, i++)
     {
	if (l == current_image)
	   im_no = i;
     }
   i = 1;
   pos = 0.0;
   for (l = images; l; l = l->next, i++)
     {
	Image              *im;
	double              w, h;
	double              sc;
	double              space;
	int                 d;

	im = l->data;
	if (!im->o_thumb)
	   continue;
	w = 48;
	h = 48;
	if (im->thumb)
	  {
	     int                 iw, ih;

	     evas_object_image_size_get(im->o_thumb, &iw, &ih);
	     w = iw / 2;
	     h = ih / 2;
	  }
	d = im_no - i;
	if (d < 0)
	   d = -d;
	sc = 1.0 / (1.0 + (((double)d) * 0.2));
	if (sc < 0.333333)
	   sc = 0.333333;
	w *= sc;
	h *= sc;
	space = 48 * sc;
	evas_object_resize(im->o_thumb, w, h);
	evas_object_image_fill_set(im->o_thumb, 0, 0, w, h);
	evas_object_move(im->o_thumb,
			 2 + ((48 - w) / 2) + icon_x,
			 2 + pos + ((space - h) / 2) + icon_y);
	if (d == 0)
	   focus_pos = pos + (space / 2) + 1;
	pos += space + 2;
     }
   evas_object_layer_set(o_list_select, 240);
   evas_object_show(o_list_select);
   evas_object_move(o_list_select, icon_x, focus_pos - 2 + icon_y - (48 / 2));
   evas_object_resize(o_list_select, 52, 52);
   evas_object_image_fill_set(o_list_select, 0, 0, 52, 52);
   evas_object_image_border_set(o_list_select, 4, 4, 4, 4);
}

/* XXX
void
e_dnd_drop_request(Ecore_Event * ev)
{
   Ecore_Event_Dnd_Drop_Request *dnd;
   Window              dnd_win;
   int                 i;

   dnd_win = main_win;
   dnd = ev->event;
   if (dnd_win == dnd->win)
     {
	dnd_num_files = dnd->num_files;
	dnd_files = NEW_PTR(dnd_num_files);
	for (i = 0; i < dnd_num_files; i++)
	   dnd_files[i] = strdup(dnd->files[i]);
     }
   return;
}

void
e_dnd_drop(Ecore_Event * ev)
{
   Ecore_Event_Dnd_Drop_Request *dnd;
   Window              dnd_win;

   dnd_win = main_win;
   dnd = ev->event;
   if (dnd_win == dnd->win)
     {
	e_handle_dnd();
	ecore_window_dnd_send_finished(main_win, dnd->source_win);
	e_dnd_drop_request_free();
     }

   return;
}

void
e_dnd_drop_request_free(void)
{
   int                 i;

   for (i = 0; i < dnd_num_files; i++)
      free(dnd_files[i]);
   free(dnd_files);
   dnd_files = NULL;
   dnd_num_files = 0;
   return;
}
*/
void
e_size_match(void)
{
   if (!full)
     {
	double w, h;
	
	evas_object_geometry_get(o_image, NULL, NULL, &w, &h);
	ecore_evas_resize(ecore_evas, w, h);
     }
}
