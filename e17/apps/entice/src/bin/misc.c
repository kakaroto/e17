#include "entice.h"

double get_time(void)
{
  struct timeval      timev;
   
  gettimeofday(&timev, NULL);
  return (double)timev.tv_sec + (((double)timev.tv_usec) / 1000000);
}

void setup(void)
{
  Window win, ewin;
  int i, j;
  Atom a_entice;
  char string[] = "entice";
   
  /* setup callbacks for events */
  ecore_event_filter_handler_add(ECORE_EVENT_WINDOW_EXPOSE,            e_window_expose);
  ecore_event_filter_handler_add(ECORE_EVENT_MOUSE_MOVE,               e_mouse_move);
  ecore_event_filter_handler_add(ECORE_EVENT_MOUSE_DOWN,               e_mouse_down);
  ecore_event_filter_handler_add(ECORE_EVENT_MOUSE_UP,                 e_mouse_up);
  ecore_event_filter_handler_add(ECORE_EVENT_WINDOW_CONFIGURE,         e_window_configure);
  ecore_event_filter_handler_add(ECORE_EVENT_KEY_DOWN,                 e_key_down);
  ecore_event_filter_handler_add(ECORE_EVENT_WINDOW_PROPERTY,          e_property);

  ecore_event_filter_handler_add(ECORE_EVENT_DND_DROP,                 e_dnd_drop);
  ecore_event_filter_handler_add(ECORE_EVENT_DND_DROP_REQUEST,         e_dnd_drop_request);
  /* handler for when the event queue goes idle */
  ecore_event_filter_idle_handler_add(e_idle, NULL);
  /* create a 400x300 toplevel window */
  win = ecore_window_new(0, 0, 0, win_w, win_h);
  ecore_window_set_events(win, XEV_CONFIGURE | XEV_PROPERTY);
  ecore_window_set_name_class(win, "Entice", "Main");
  ecore_window_set_min_size(win, 256, 128);
  ecore_window_set_max_size(win, 8000, 8000);
  a_entice = ecore_atom_get("_ENTICE_APP_WINDOW");
  ecore_window_property_set(win, a_entice, XA_STRING, 8, string, 6);
  main_win = win;
   
  /* create a 400x300 evas rendering in software - conveience function that */
  /* also creates the window for us in the right colormap & visual */
  evas = evas_new_all(ecore_display_get(), win, 0, 0, win_w, win_h, render_method,
		      max_colors, MAX_FONT_CACHE, MAX_IMAGE_CACHE,
		      FONT_DIRECTORY);
  /* tell evas if we want our image scaled with "smooth" scaling or not */
  evas_set_scale_smoothness(evas, smoothness);
  /* get the window ID for the evas created for us */
  ewin = evas_get_window(evas);
  
  /* show the evas window */
  ecore_window_show(ewin);
  /* set the events this window accepts */
  ecore_window_set_events(ewin, XEV_EXPOSE | XEV_BUTTON | XEV_MOUSE_MOVE | XEV_KEY);
  /* Advertise for dnd */
  ecore_window_dnd_advertise(ewin);
  /* show the toplevel */
  ecore_window_show(win);
   
  /* now... create objects in the evas */
   
  o_bg = evas_add_image_from_file(evas, IM"bg.png");   
  o_logo = evas_add_image_from_file(evas, IM"logo.png");
  o_panel = evas_add_image_from_file(evas, IM"panel.png");
  o_showpanel = evas_add_rectangle(evas);
  o_hidepanel = evas_add_rectangle(evas);
  o_showbuttons = evas_add_rectangle(evas);
  o_arrow_l = evas_add_image_from_file(evas, IM"arrow_l.png");
  o_arrow_r = evas_add_image_from_file(evas, IM"arrow_r.png");
  o_arrow_t = evas_add_image_from_file(evas, IM"arrow_t.png");
  o_arrow_b = evas_add_image_from_file(evas, IM"arrow_b.png");
  o_mini_select = evas_add_image_from_file(evas, IM"mini_sel.png");
  o_list_select = evas_add_image_from_file(evas, IM"list_sel.png");
  o_panel_arrow_u = evas_add_image_from_file(evas, IM"list_arrow_u_1.png");
  o_panel_arrow_d = evas_add_image_from_file(evas, IM"list_arrow_d_1.png");
  o_bt_close = evas_add_image_from_file(evas, IM"bt_close_1.png");
  o_bt_expand = evas_add_image_from_file(evas, IM"bt_expand_1.png");
  o_bt_full = evas_add_image_from_file(evas, IM"bt_full_1.png");
  o_bt_next = evas_add_image_from_file(evas, IM"bt_next_1.png");
  o_bt_prev = evas_add_image_from_file(evas, IM"bt_prev_1.png");
  o_bt_zoom_in = evas_add_image_from_file(evas, IM"bt_zoom_in_1.png");
  o_bt_zoom_normal = evas_add_image_from_file(evas, IM"bt_zoom_normal_1.png");
  o_bt_zoom_out = evas_add_image_from_file(evas, IM"bt_zoom_out_1.png");
  evas_set_color(evas, o_logo, 255, 255, 255, 0);
  evas_set_color(evas, o_showpanel, 0, 0, 0, 0);
  evas_set_color(evas, o_hidepanel, 0, 0, 0, 0);
  evas_set_color(evas, o_showbuttons, 0, 0, 0, 0);
  evas_set_pass_events(evas, o_panel, 1);
  for (j = 0; j < 2; j++)
    {
      for (i = 0; i < 5; i++)
	{
	  o_txt_info[(j * 5) + i] = evas_add_text(evas, "nationff", 16, "");
	  evas_set_pass_events(evas, o_txt_info[(j * 5) + i], 1);
	}	
    }
  for (i = 0; i < 5; i++)
    {
      o_txt_tip[i] = evas_add_text(evas, "nationff", 16, "");
      evas_set_pass_events(evas, o_txt_info[i], 1);
    }
   
  e_display_current_image();
   
  evas_show(evas, o_bg);
  evas_show(evas, o_logo);
  evas_show(evas, o_panel);
  evas_show(evas, o_showpanel);
  evas_show(evas, o_hidepanel);
  evas_show(evas, o_showbuttons);

  evas_show(evas, o_panel_arrow_u);
  evas_show(evas, o_panel_arrow_d);

  evas_show(evas, o_bt_close);
  evas_show(evas, o_bt_expand);
  evas_show(evas, o_bt_full);
  evas_show(evas, o_bt_next);
  evas_show(evas, o_bt_prev);
  evas_show(evas, o_bt_zoom_in);
  evas_show(evas, o_bt_zoom_normal);
  evas_show(evas, o_bt_zoom_out);

  e_slide_panel_out(0, NULL);
  e_slide_buttons_out(0, NULL);

  evas_callback_add(evas, o_showpanel, CALLBACK_MOUSE_IN, show_panel, NULL);
  evas_callback_add(evas, o_hidepanel, CALLBACK_MOUSE_IN, hide_panel, NULL);
  evas_callback_add(evas, o_showbuttons, CALLBACK_MOUSE_IN, show_buttons, NULL);
  evas_callback_add(evas, o_logo, CALLBACK_MOUSE_DOWN, next_image, NULL);
  evas_callback_add(evas, o_logo, CALLBACK_MOUSE_UP, next_image_up, NULL);
  evas_callback_add(evas, o_bg, CALLBACK_MOUSE_DOWN, next_image, NULL);
  evas_callback_add(evas, o_bg, CALLBACK_MOUSE_UP, next_image_up, NULL);
  evas_callback_add(evas, o_hidepanel, CALLBACK_MOUSE_DOWN, next_image, NULL);
  evas_callback_add(evas, o_hidepanel, CALLBACK_MOUSE_UP, next_image_up, NULL);
  evas_callback_add(evas, o_hidepanel, CALLBACK_MOUSE_MOVE, next_image_move, NULL);
  evas_callback_add(evas, o_showpanel, CALLBACK_MOUSE_DOWN, next_image, NULL);
  evas_callback_add(evas, o_showpanel, CALLBACK_MOUSE_UP, next_image_up, NULL);
  evas_callback_add(evas, o_showpanel, CALLBACK_MOUSE_MOVE, next_image_move, NULL);
  evas_callback_add(evas, o_panel_arrow_u, CALLBACK_MOUSE_DOWN, list_scroll_up, NULL);
  evas_callback_add(evas, o_panel_arrow_u, CALLBACK_MOUSE_UP, list_scroll_up_up, NULL);
  evas_callback_add(evas, o_panel_arrow_d, CALLBACK_MOUSE_DOWN, list_scroll_down, NULL);
  evas_callback_add(evas, o_panel_arrow_d, CALLBACK_MOUSE_UP, list_scroll_down_up, NULL);
  evas_callback_add(evas, o_bt_close, CALLBACK_MOUSE_DOWN, bt_close_down, NULL);
  evas_callback_add(evas, o_bt_close, CALLBACK_MOUSE_UP, bt_close_up, NULL);
  evas_callback_add(evas, o_bt_close, CALLBACK_MOUSE_IN, bt_tip_start, "Exit [ Key: Q ]");
  evas_callback_add(evas, o_bt_close, CALLBACK_MOUSE_OUT, bt_tip_stop, NULL);
  evas_callback_add(evas, o_bt_expand, CALLBACK_MOUSE_DOWN, bt_expand_down, NULL);
  evas_callback_add(evas, o_bt_expand, CALLBACK_MOUSE_UP, bt_expand_up, NULL);
  evas_callback_add(evas, o_bt_expand, CALLBACK_MOUSE_IN, bt_tip_start, "Scale image to fit entirely in view [ Key: W ]");
  evas_callback_add(evas, o_bt_expand, CALLBACK_MOUSE_OUT, bt_tip_stop, NULL);
  evas_callback_add(evas, o_bt_full, CALLBACK_MOUSE_DOWN, bt_full_down, NULL);
  evas_callback_add(evas, o_bt_full, CALLBACK_MOUSE_UP, bt_full_up, NULL);
  evas_callback_add(evas, o_bt_full, CALLBACK_MOUSE_IN, bt_tip_start, "Toggle fullscreen display [ Key: F ]");
  evas_callback_add(evas, o_bt_full, CALLBACK_MOUSE_OUT, bt_tip_stop, NULL);
  evas_callback_add(evas, o_bt_next, CALLBACK_MOUSE_DOWN, bt_next_down, NULL);
  evas_callback_add(evas, o_bt_next, CALLBACK_MOUSE_UP, bt_next_up, NULL);
  evas_callback_add(evas, o_bt_next, CALLBACK_MOUSE_IN, bt_tip_start, "View next image [ Key: Space ]");
  evas_callback_add(evas, o_bt_next, CALLBACK_MOUSE_OUT, bt_tip_stop, NULL);
  evas_callback_add(evas, o_bt_prev, CALLBACK_MOUSE_DOWN, bt_prev_down, NULL);
  evas_callback_add(evas, o_bt_prev, CALLBACK_MOUSE_UP, bt_prev_up, NULL);
  evas_callback_add(evas, o_bt_prev, CALLBACK_MOUSE_IN, bt_tip_start, "View previous image [ Key: Backspace ]");
  evas_callback_add(evas, o_bt_prev, CALLBACK_MOUSE_OUT, bt_tip_stop, NULL);
  evas_callback_add(evas, o_bt_zoom_in, CALLBACK_MOUSE_DOWN, bt_zoom_in_down, NULL);
  evas_callback_add(evas, o_bt_zoom_in, CALLBACK_MOUSE_UP, bt_zoom_in_up, NULL);
  evas_callback_add(evas, o_bt_zoom_in, CALLBACK_MOUSE_IN, bt_tip_start, "Zoom in one step [ Key: = ]");
  evas_callback_add(evas, o_bt_zoom_in, CALLBACK_MOUSE_OUT, bt_tip_stop, NULL);
  evas_callback_add(evas, o_bt_zoom_normal, CALLBACK_MOUSE_DOWN, bt_zoom_normal_down, NULL);
  evas_callback_add(evas, o_bt_zoom_normal, CALLBACK_MOUSE_UP, bt_zoom_normal_up, NULL);
  evas_callback_add(evas, o_bt_zoom_normal, CALLBACK_MOUSE_IN, bt_tip_start, "Reset zoom to normal 1:1  [ Key: N ]");
  evas_callback_add(evas, o_bt_zoom_normal, CALLBACK_MOUSE_OUT, bt_tip_stop, NULL);
  evas_callback_add(evas, o_bt_zoom_out, CALLBACK_MOUSE_DOWN, bt_zoom_out_down, NULL);
  evas_callback_add(evas, o_bt_zoom_out, CALLBACK_MOUSE_UP, bt_zoom_out_up, NULL);
  evas_callback_add(evas, o_bt_zoom_out, CALLBACK_MOUSE_IN, bt_tip_start, "Zoom out one step [ Key: - ]");
  evas_callback_add(evas, o_bt_zoom_out, CALLBACK_MOUSE_OUT, bt_tip_stop, NULL);
}

void e_mkdirs(char *path)
{
  char ss[1024];
  int  i, ii;
   
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

void e_child(Ecore_Event * ev)
{
  Ecore_Event_Child      *e;
   
  e = (Ecore_Event_Child *)ev->event;
  if (generating_image)
    {
      if (generating_image->generator == e->pid)
	{
	  generating_image->generator = 0;
	  generating_image = NULL;
	}
    }
}

void e_toggle_fullscreen(void)
{
  static int full = 0;
  static Window win = 0;
   
  if (!full)
    {
      Window ewin;
      int rw, rh;
	
      ecore_grab();
      ecore_sync();
      ecore_window_get_geometry(0, NULL, NULL, &rw, &rh);
      win = ecore_window_override_new(0, 0, 0, rw, rh);
      ecore_window_set_events(win, XEV_CONFIGURE);
      ewin = evas_get_window(evas);
      ecore_window_reparent(ewin, win, 0, 0);
      ecore_window_resize(ewin, rw, rh);
      evas_set_output_viewport(evas, 0, 0, rw, rh);
      evas_set_output_size(evas, rw, rh);
      win_w = rw;
      win_h = rh;
      e_handle_resize();
      if (panel_active)
	e_slide_panel_out(0, NULL);
      if (buttons_active)
	e_slide_buttons_out(0, NULL);
      ecore_window_show(win);
      ecore_keyboard_grab(ewin);
      ecore_grab_mouse(ewin, 1, 0);
      full = 1;
      if ((!ecore_grab_window_get()) || (!ecore_keyboard_grab_window_get()))
	 {
	    e_toggle_fullscreen();
	 }
    }
  else
    {
      Window ewin;
	
      ecore_ungrab();
      ecore_sync();
      ecore_pointer_ungrab(CurrentTime);
      ecore_keyboard_ungrab();
      ecore_window_get_geometry(main_win, NULL, NULL, &win_w, &win_h);
      ewin = evas_get_window(evas);
      ecore_window_reparent(ewin, main_win, 0, 0);
      if (win) ecore_window_destroy(win);
      win = 0;
      ecore_window_resize(ewin, win_w, win_h);
      evas_set_output_viewport(evas, 0, 0, win_w, win_h);
      evas_set_output_size(evas, win_w, win_h);
      e_handle_resize();
      if (panel_active)
	e_slide_panel_out(0, NULL);
      if (buttons_active)
	e_slide_buttons_out(0, NULL);
      full = 0;
    }
}

void e_fix_icons(void)
{
  Evas_List l;
  int i, im_no;
  double pos;
   
  i = 1;
  im_no = 0;
  for (l = images; l; l = l->next, i++)
    {
      if (l == current_image) im_no = i;
    }
  i = 1;
  pos = 0.0;
  for (l = images; l; l = l->next, i++)
    {
      Image *im;
      double w, h;
      double sc;
      double space;
      int d;
	
      im = l->data;
      w = 48; h = 48;
      if (im->thumb)
	{
	  int iw, ih;
	     
	  evas_get_image_size(evas, im->o_thumb, &iw, &ih);
	  w = iw; h = ih;
	}
      d = im_no - i;
      if (d < 0) d = -d;
      sc = 1.0 / (1.0 + (((double)d) * 0.2));
      if (sc < 0.333333) sc = 0.333333;
      w *= sc;
      h *= sc;
      space = 48 * sc;
      evas_resize(evas, im->o_thumb, w, h);
      evas_set_image_fill(evas, im->o_thumb, 0, 0, w, h);		    
      evas_move(evas, im->o_thumb, 
		2 + ((48 - w) / 2) + icon_x, 
		2 + pos + ((space - h) / 2) + icon_y);
      if (d == 0) focus_pos = pos + (space / 2) + 1;
      pos += space + 2;
    }
  evas_set_layer(evas, o_list_select, 240);
  evas_show(evas, o_list_select);
  evas_move(evas, o_list_select, icon_x, focus_pos - 2 + icon_y - (48 / 2));
  evas_resize(evas, o_list_select, 52, 52);
  evas_set_image_fill(evas, o_list_select, 0, 0, 52, 52);
  evas_set_image_border(evas, o_list_select, 4, 4, 4, 4);
}


void e_dnd_drop_request(Ecore_Event *ev)
{
    Ecore_Event_Dnd_Drop_Request *dnd;
    DIR *d;
    Window dnd_win;
    int i;

    dnd_win = evas_get_window(evas);
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

void e_dnd_drop(Ecore_Event *ev)
{
    Ecore_Event_Dnd_Drop_Request *dnd;
    Window dnd_win;

    dnd_win = evas_get_window(evas);
    dnd = ev->event;
    if (dnd_win == dnd->win) 
    {
	e_handle_dnd();
	ecore_window_dnd_send_finished(main_win, dnd->source_win);
	e_dnd_drop_request_free();
    }

    return;
}

void e_dnd_drop_request_free(void)
{
    int i;

    for (i = 0; i < dnd_num_files; i++) 
	free(dnd_files[i]);
    free(dnd_files);
    dnd_files = NULL;
    dnd_num_files = 0;
    return;
}
    


