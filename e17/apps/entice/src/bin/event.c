#include "entice.h"

int
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

		  if (!im->o_thumb) {
		     im->o_thumb = evas_object_image_add(evas);
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
		  }
		  if ((im->subst) && (im->thumb))
		    {
		       int                 w, h;
		       // printf("%s %s\n", im->file, im->thumb);
		       
			evas_object_image_file_set(im->o_thumb, im->thumb,
						     NULL);

		       evas_object_image_size_get(im->o_thumb, &w, &h);

		       im->subst = 0;
		       evas_object_move(im->o_thumb,
					2 + ((48 - w) / 2) + icon_x,
					2 + ((48 + 2) * (i - 1)) +
					((48 - h) / 2) + icon_y);
		       evas_object_layer_set(im->o_thumb, 249);
		       evas_object_show(im->o_thumb);
		       im->subst = 0;
		    }
		if (!im->thumb) {
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
	    return 1;
}

void
e_window_resize(Ecore_Evas *ee)
{
   e_handle_resize();
}

void
e_key_down(void* data, Evas * unused, Evas_Object *obj, void *event_info)
{
   Evas_Event_Key_Down *e;

   e = event_info;

   if (!strcmp(e->keyname, "n"))
     {
	scale = 1.0;
	e_handle_resize();
     }
   else if ((!strcmp(e->keyname, "minus")) ||
	    (!strcmp(e->keyname, "o")))
     {
	scale *= 1.414;
	e_handle_resize();
     }
   else if ((!strcmp(e->keyname, "plus")) ||
	    (!strcmp(e->keyname, "equal")) ||
	    (!strcmp(e->keyname, "i")))
     {
	scale /= 1.414;
	if (scale < 0.03125)
	  scale = 0.03125;
	e_handle_resize();
     }
   else if (!strcmp(e->keyname, "w"))
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
   else if (!strcmp(e->keyname, "f"))
     {
	e_toggle_fullscreen();
     }
   else if (!strcmp(e->keyname, "i"))
     {
	e_fade_info_in(NULL);
     }
   else if (!strcmp(e->keyname, "?"))
     {
	/* FIXME: display help */
     }
   else if (!strcmp(e->keyname, "q"))
     {
	exit(0);
     }
   else if (!strcmp(e->keyname, "r"))
     {
	e_rotate_r_current_image();
     }
   else if (!strcmp(e->keyname, "l"))
     {
	e_rotate_l_current_image();
     }
   else if (!strcmp(e->keyname, "h"))
     {
	e_flip_h_current_image();
     }
   else if (!strcmp(e->keyname, "v"))
     {
	e_flip_v_current_image();
     }
   else if (!strcmp(e->keyname, "p"))
     {
	if (panel_active == active_in || panel_active == active_force_in) {
	   enum active_state command = active_out;
           panel_active = active_in;
	   e_slide_panel(&command);
	}
	else {
	   enum active_state command = active_in;
	   e_slide_panel(&command);
	}
     }
   else if (!strcmp(e->keyname, "b"))
     {
	if (buttons_active == active_in || buttons_active == active_force_in) {
	   enum active_state command = active_out;
	   buttons_active = active_in;
	   e_slide_buttons(&command);
	}
	else {
	   enum active_state command = active_in;
	   e_slide_buttons(&command);
	}
     }
   else if (!strcmp(e->keyname, "d"))
     {
	e_delete_current_image();
     }
   else if (!strcmp(e->keyname, "s"))
     {
	e_save_current_image();
     }
   else if (!strcmp(e->keyname, "t"))
     {
        e_turntable_l_current_image();
     }
   else if (!strcmp(e->keyname, "e"))
     {
        e_size_match();
     }
   else if (!strcmp(e->keyname, "Escape"))
     {
	exit(0);
     }
   else if (!strcmp(e->keyname, "Return"))
     {
	e_display_current_image();
     }
   else if (!strcmp(e->keyname, "space"))
     {
	e_load_next_image();
     }
   else if (!strcmp(e->keyname, "BackSpace"))
     {
	e_load_prev_image();
     }
   else if (!strcmp(e->keyname, "Up"))
     {
	icon_y += 8;
	e_fix_icons();
     }
   else if (!strcmp(e->keyname, "Down"))
     {
	icon_y -= 8;
	e_fix_icons();
     }
   return;
}

void
e_key_up(void* data, Evas *unused, Evas_Object *obj, void* event_info)
{
   Evas_Event_Key_Up *e;

   e = event_info;

   if (!strcmp(e->keyname, "t"))
     {
        e_turntable_r_current_image();
     }
   return;
}
/*
int
e_property(void* data, int ev_type, Ecore_Event * ev)
{
   Ecore_X_Event_Window_Property *e;
   Ecore_X_Atom        a_entice_newfiles = 0, xa_string;
   char               *files;
   int                 size;

   e = (Ecore_X_Event_Window_Property *) ev;
   if (e->win != main_win)
      return 1;
   a_entice_newfiles = ecore_atom_get("_ENTICE_NEWFILES");
   if (e->atom != a_entice_newfiles)
      return 1;
   xa_string = ecore_x_atom_get("XA_STRING");
   files = ecore_x_window_prop_property_get(e->win, e->atom, xa_string, &size);
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
		     evas_object_image_file_set(im->o_thumb, IM "thumb.png", NULL);
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
   return 1;
}
*/
