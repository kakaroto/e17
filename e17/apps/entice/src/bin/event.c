#include "entice.h"

void
e_idle(void *data)
{
   if (need_thumbs)
     {
	if (!generating_image)
	  {
	     Evas_List          *l;
	     int                 doing;
	     int                 i;

	     doing = 0;
	     i = 1;
	     for (l = images; l; l = l->next, i++)
	       {
		  Image              *im;

		  im = l->data;

		  /* CS */
		  /* printf("%s()\n",__FUNCTION__);
		   * printf("%s\n",im->file); */

		  if ((im->subst) && (im->thumb))
		    {
		       int                 w, h;

		       evas_object_del(im->o_thumb);
		       im->o_thumb = evas_object_image_add(evas);
		       if (e_file_is_dir(im->file))
			  evas_object_image_file_set(im->o_thumb,
						     IM "directory.png", NULL);
		       else
			  evas_object_image_file_set(im->o_thumb, im->thumb,
						     NULL);

		       evas_object_event_callback_add(im->o_thumb,
						      EVAS_CALLBACK_MOUSE_MOVE,
						      e_list_item_drag, l);
		       evas_object_event_callback_add(im->o_thumb,
						      EVAS_CALLBACK_MOUSE_DOWN,
						      e_list_item_click, l);
		       evas_object_event_callback_add(im->o_thumb,
						      EVAS_CALLBACK_MOUSE_UP,
						      e_list_item_select, l);
		       evas_object_event_callback_add(im->o_thumb,
						      EVAS_CALLBACK_MOUSE_IN,
						      e_list_item_in, l);
		       evas_object_event_callback_add(im->o_thumb,
						      EVAS_CALLBACK_MOUSE_OUT,
						      e_list_item_out, l);
		       evas_object_image_size_get(im->o_thumb, &w, &h);

		       im->subst = 0;
		       evas_object_move(im->o_thumb,
					2 + ((48 - w) / 2) + icon_x,
					2 + ((48 + 2) * (i - 1)) +
					((48 - h) / 2) + icon_y);
		       evas_object_layer_set(im->o_thumb, 210);
		       evas_object_show(im->o_thumb);
		       im->subst = 0;
		    }
		  if (!im->thumb)
		    {
		       e_generate_thumb(im);
		       doing = 1;
		    }
	       }
	     if (!doing)
		need_thumbs = 0;
	  }
	e_fix_icons();
     }
   evas_render(evas);
}

void
e_window_expose(Ecore_Event * ev)
{
   Ecore_Event_Window_Expose *e;

   e = (Ecore_Event_Window_Expose *) ev->event;
   if (e->win != ewin)
      return;
   evas_damage_rectangle_add(evas, e->x, e->y, e->w, e->h);
}

void
e_mouse_move(Ecore_Event * ev)
{
   Ecore_Event_Mouse_Move *e;

   e = (Ecore_Event_Mouse_Move *) ev->event;
   if (e->win != ewin)
      return;
   evas_event_feed_mouse_move(evas, e->x, e->y);
}

void
e_mouse_down(Ecore_Event * ev)
{
   Ecore_Event_Mouse_Down *e;

   e = (Ecore_Event_Mouse_Down *) ev->event;
   if (e->win != ewin)
      return;
   evas_event_feed_mouse_move(evas, e->x, e->y);
   evas_event_feed_mouse_down(evas, e->button);
}

void
e_mouse_up(Ecore_Event * ev)
{
   Ecore_Event_Mouse_Up *e;

   e = (Ecore_Event_Mouse_Up *) ev->event;
   if (e->win != ewin)
      return;
   evas_event_feed_mouse_move(evas, e->x, e->y);
   evas_event_feed_mouse_up(evas, e->button);
}

void
e_window_configure(Ecore_Event * ev)
{
   Ecore_Event_Window_Configure *e;

   e = (Ecore_Event_Window_Configure *) ev->event;
   if (e->win == main_win)
     {
	win_w = e->w;
	win_h = e->h;
	if (e->wm_generated)
	  {
	     win_x = e->x;
	     win_y = e->y;
	  }
	ecore_window_resize(ewin, win_w, win_h);
	evas_output_viewport_set(evas, 0, 0, win_w, win_h);
	evas_output_size_set(evas, win_w, win_h);
	e_handle_resize();
	if (panel_active == active_in)
	   e_slide_panel_out(0, NULL);
	if (buttons_active == active_in)
	   e_slide_buttons_out(0, NULL);
     }
}

void
e_key_down(Ecore_Event * ev)
{
   Ecore_Event_Key_Down *e;

   e = ev->event;
   if (e->win != ewin)
      return;

   if (!strcmp(e->key, "n"))
     {
	scale = 1.0;
	e_handle_resize();
     }
   else if (!strcmp(e->symbol, "minus"))
     {
	scale += 0.5;
	e_handle_resize();
     }
   else if (!strcmp(e->symbol, "plus"))
     {
	scale -= 0.5;
	if (scale < 0.5)
	   scale = 0.5;
	e_handle_resize();
     }
   else if (!strcmp(e->symbol, "w"))
     {
	int                 w, h;

	if (o_image)
	  {
	     double              sh, sv;

	     evas_object_image_size_get(o_image, &w, &h);
	     sh = (double)w / (double)win_w;
	     sv = (double)h / (double)win_h;
	     scale = sv;
	     if (sh > sv)
		scale = sh;
	     e_handle_resize();
	  }
     }
   else if (!strcmp(e->symbol, "f"))
     {
	e_toggle_fullscreen();
     }
   else if (!strcmp(e->symbol, "i"))
     {
	e_fade_info_in(0, NULL);
     }
   else if (!strcmp(e->symbol, "?"))
     {
	/* FIXME: display help */
     }
   else if (!strcmp(e->symbol, "q"))
     {
	exit(0);
     }
   else if (!strcmp(e->symbol, "r"))
     {
	e_rotate_r_current_image();
     }
   else if (!strcmp(e->symbol, "l"))
     {
	e_rotate_l_current_image();
     }
   else if (!strcmp(e->symbol, "h"))
     {
	e_flip_h_current_image();
     }
   else if (!strcmp(e->symbol, "v"))
     {
	e_flip_v_current_image();
     }
   else if (!strcmp(e->symbol, "p"))
     {
	int force = 1;
	if (panel_active == active_in || panel_active == active_force_in) {
           panel_active = active_in;
	   e_slide_panel_out(0, NULL);
	}
	else {
	   e_slide_panel_in(0, &force);
	}
     }
   else if (!strcmp(e->symbol, "b"))
     {
	int force = 1;
	if (buttons_active == active_in || buttons_active == active_force_in) {
	   buttons_active = active_in;
	   e_slide_buttons_out(0, NULL);
	}
	else {
	   e_slide_buttons_in(0, &force);
	}
     }
   else if (!strcmp(e->symbol, "d"))
     {
	e_delete_current_image();
     }
   else if (!strcmp(e->symbol, "s"))
     {
	e_save_current_image();
     }
   else if (!strcmp(e->symbol, "t"))
     {
        e_turntable_l_current_image();
     }
   else if (!strcmp(e->symbol, "e"))
     {
        e_size_match();
     }
   else if (!strcmp(e->symbol, "Escape"))
     {
	exit(0);
     }
   else if (!strcmp(e->symbol, "Return"))
     {
	e_display_current_image();
     }
   else if (!strcmp(e->symbol, "space"))
     {
	e_load_next_image();
     }
   else if (!strcmp(e->symbol, "BackSpace"))
     {
	e_load_prev_image();
     }
   else if (!strcmp(e->symbol, "Up"))
     {
	icon_y += 8;
	e_fix_icons();
     }
   else if (!strcmp(e->symbol, "Down"))
     {
	icon_y -= 8;
	e_fix_icons();
     }
}

void
e_key_up(Ecore_Event * ev)
{
   Ecore_Event_Key_Down *e;

   e = ev->event;
   if (e->win != ewin)
      return;
   if (!strcmp(e->symbol, "t"))
     {
        e_turntable_r_current_image();
     }
}

void
e_property(Ecore_Event * ev)
{
   Ecore_Event_Window_Property *e;
   Atom                a_entice_newfiles = 0;
   char               *files;
   int                 size;

   e = (Ecore_Event_Window_Property *) ev->event;
   if (e->win != main_win)
      return;
   if (!a_entice_newfiles)
      a_entice_newfiles = ecore_atom_get("_ENTICE_NEWFILES");
   if (e->atom != a_entice_newfiles)
      return;
   files = ecore_window_property_get(e->win, e->atom, XA_STRING, &size);
   if (files)
     {
	char                file[4096], *p, *pp;

	pp = files;
	while ((p = strchr(pp, '\n')))
	  {
	     Image              *im;

	     *p = 0;
	     strcpy(file, pp);
	     im = e_image_new(file);
	     images = evas_list_append(images, im);
	     pp = p + 1;
	     if (pp >= files + size)
		break;
	  }
	free(files);
	{
	   Evas_List          *l;
	   int                 i;

	   i = 1;
	   for (l = images; l; l = l->next, i++)
	     {
		Image              *im;
		int                 first;

		im = l->data;
		first = 1;
		if (!im->o_thumb)
		  {
		     if (first)
		       {
			  current_image = l;
			  first = 0;
		       }
		     im->modified = 0;
		     im->o_thumb = evas_object_rectangle_add(evas);
		     evas_object_image_file_set(evas, IM "thumb.png", NULL);
		     evas_object_event_callback_add(im->o_thumb,
						    EVAS_CALLBACK_MOUSE_DOWN,
						    e_list_item_click, l);
		     evas_object_event_callback_add(im->o_thumb,
						    EVAS_CALLBACK_MOUSE_UP,
						    e_list_item_select, l);
		     evas_object_event_callback_add(im->o_thumb,
						    EVAS_CALLBACK_MOUSE_IN,
						    e_list_item_in, l);
		     evas_object_event_callback_add(im->o_thumb,
						    EVAS_CALLBACK_MOUSE_OUT,
						    e_list_item_out, l);
		     im->subst = 1;
		     evas_object_image_border_set(im->o_thumb, 4, 4, 4, 4);
		     evas_object_move(im->o_thumb, 2, 2 + ((48 + 2) * (i - 1)));
		     evas_object_resize(im->o_thumb, 48, 48);
		     evas_object_image_fill_set(im->o_thumb, 0, 0, 48, 48);
		     evas_object_layer_set(im->o_thumb, 210);
		     evas_object_show(im->o_thumb);
		  }
	     }
	}
	need_thumbs = 1;
	e_fix_icons();
	e_display_current_image();
     }
}
