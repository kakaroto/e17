#include "ui.h"

/* Elogin_Entry fx */

void
_elogin_entry_fx_fade_out (int v, void *data)
{
  Elogin_Entry *entry;
  entry = data;

  if (v < 256)
    {
      evas_set_color (evas, entry->fx, 0, 0, 0, 255 - v);
      ecore_add_event_timer ("e_fade_out()", 0.01, _elogin_entry_fx_fade_out,
			     v + 2, entry);
      evas_show (evas, entry->fx);
    }
  else
    {
      //  evas_hide(evas,entry->fx);
      ecore_del_event_timer ("e_fade_out()");
    }
}


/* Elogin_Entry callbacks */
void
_elogin_entry_key_down (Ecore_Event * ev)
{
  Ecore_Event_Key_Down *e;

  e = ev->event;

  if (selected)

    if (e->compose)
      {
	selected->ch = strdup (e->compose);
	if (!strcmp (e->key, "BackSpace"))
	  {
	    selected->ch = strdup (e->key);
	  }
	if (!strcmp (e->key, "Delete"))
	  {
	    selected->ch = strdup (e->key);
	  }
	if (!strcmp (e->key, "Return"))
	  {
	    selected->ch = strdup (e->key);
	  }
	if (!strcmp (e->key, "Tab"))
	  {
	    selected->ch = strdup (e->key);
	  }
	if (!strcmp (e->key, "Right"))
	  {
	    selected->ch = strdup (e->key);
	  }
	if (!strcmp (e->key, "Left"))
	  {
	    selected->ch = strdup (e->key);
	  }
      }
    else
      {
	selected->ch = strdup (e->key);

      }

}

void
_elogin_entry_get_key (int v, void *data)
{
  Elogin_Entry *entry;
  entry = data;


  if (strcmp (entry->ch, ""))
    {
      if (!strcmp (entry->ch, "BackSpace"))
	{
	  int i;
	  int s;
	  s = elogin_entry_get_text_len (entry);
	  if (entry->cp > 0)
	    {
	      entry->t[entry->cp - 1] = 0;
	      for (i = entry->cp - 1; i < s; i++)
		{
		  entry->t[i] = entry->t[i + 1];
		}

	      entry->cp--;
	      elogin_entry_cursor_from_text (entry);
	      elogin_entry_update_text (entry);
	    }



	  entry->ch = "";

	}
      if (!strcmp (entry->ch, "Delete"))
	{
	  int i;
	  int s;
	  s = elogin_entry_get_text_len (entry);
	  if (s > entry->cp)
	    {
	      entry->t[entry->cp] = 0;
	      for (i = entry->cp; i < s; i++)
		{
		  entry->t[i] = entry->t[i + 1];
		}

	      elogin_entry_update_text (entry);
	    }

	  entry->ch = "";

	}
      if (!strcmp (entry->ch, "Right"))
	{
	  if (elogin_entry_get_text_len (entry) > entry->cp)
	    {
	      entry->cp++;
	      elogin_entry_cursor_from_text (entry);
	    }
	  entry->ch = "";

	}

      if (!strcmp (entry->ch, "Left"))
	{
	  if (entry->cp > 0)
	    {
	      entry->cp--;
	      elogin_entry_cursor_from_text (entry);
	    }
	  entry->ch = "";

	}

      if (!strcmp (entry->ch, "Return"))
	{
	  elogin_entry_set_next_selected (entry);
	  entry->ch = "";
	}

      if (!strcmp (entry->ch, "Tab"))
	{
	  elogin_entry_set_next_selected (entry);
	  entry->ch = "";
	}

      if (strlen (entry->ch) == 1)
	{
	  char tmp[1024];
	  int i;
	  int s;

	  s = elogin_entry_get_text_len (entry);

	  for (i = 0; i < 1024; i++)
	    {
	      tmp[i] = 0;
	    }
	  for (i = entry->cp; i < s; i++)
	    {
	      tmp[i - entry->cp] = entry->t[i];
	      entry->t[i] = 0;
	    }

	  sprintf (entry->t, "%s%s%s", entry->t, entry->ch, tmp);

	  /* fx */
	  if (entry->passwd == 0)
	    evas_set_text (evas, entry->fx, entry->ch);
	  else
	    evas_set_text (evas, entry->fx, "*");
	  _elogin_entry_fx_fade_out (0, entry);

	  entry->cp++;
	  elogin_entry_update_text (entry);
	  elogin_entry_cursor_from_text (entry);



	  entry->ch = "";
	}
    }

  ecore_add_event_timer ("e_get_key()", 0.1, _elogin_entry_get_key, 0,
			 selected);
}

void
_elogin_entry_mouse_down (void *_data, Evas _e, Evas_Object _o, int _b,
			  int _x, int _y)
{
  Elogin_Entry *entry;
  Evas_List l;
  double cx, cy, cw, ch;
  int index;
  double x, y, h, w;

  entry = _data;
  for (l = entry->b->entries; l; l = l->next)
    {
      Elogin_Entry *ent;
      ent = l->data;
      elogin_entry_set_normal (ent);
    }

  elogin_entry_set_selected (entry);

  evas_get_geometry (evas, entry->text, &x, &y, &w, &h);
  index =
    evas_text_at_position (evas, entry->text, _x - entry->o->x,
			   _y - entry->o->y, &cx, &cy, &cw, &ch);

  if (index != -1)
    {
      int i;
      int xp = 0;
      int lst;
      int tmp;

      for (i = 0; i <= index; i++)
	{
	  double w;
	  elogin_entry_get_char_width (entry, i, &w);
	  xp += w;
	  lst = w;
	}
      /* cut the char in two part, if right part clicked, cursor pos is before char; else, after */
      entry->cp = index + 1;
      if ((_x - entry->o->x) < (xp - lst / 2))
	{
	  xp -= lst;
	  entry->cp--;
	}


      elogin_entry_set_cursor_pos (entry, xp, 0);
      elogin_entry_update_cursor (entry);
    }


}
void
_elogin_entry_mouse_up (void *_data, Evas _e, Evas_Object _o, int _b, int _x,
			int _y)
{
}
void
_elogin_entry_mouse_in (void *_data, Evas _e, Evas_Object _o, int _b, int _x,
			int _y)
{
  Elogin_Entry *entry;
  entry = _data;

  if (entry->selected == 0)
    {
      elogin_entry_set_hilited (entry);
    }

}
void
_elogin_entry_mouse_out (void *_data, Evas _e, Evas_Object _o, int _b, int _x,
			 int _y)
{
  Elogin_Entry *entry;
  entry = _data;
  if (entry->selected == 0)
    {
      elogin_entry_set_normal (entry);
    }

}

Elogin_Entry *
elogin_entry_new (int passwd)
{
  Elogin_Object *obj;
  Elogin_Entry *entry;

  obj = elogin_object_new ();

  entry = NEW (Elogin_Entry, 1);
  ZERO (entry, Elogin_Entry, 1);

  entry->o = obj;

  entry->ch = (char *) malloc (sizeof (char) * 32);
  entry->ch = "";
  entry->passwd = passwd;
  entry->cp = 0;

  /* could be an Ebits object */
  entry->bg =
    evas_add_image_from_file (evas,
			      PACKAGE_DATA_DIR "/elogin/data/entry.png");
  evas_set_image_border (evas, entry->bg, 3, 2, 1, 3);

  /* too */
  entry->cursor =
    evas_add_image_from_file (evas,
			      PACKAGE_DATA_DIR "/elogin/data/cursor.png");
  entry->text = evas_add_text (evas, "borzoib", 14, "");
  entry->fx = evas_add_text (evas, "borzoib", 14, "");

  elogin_entry_set_normal (entry);

  evas_callback_add (evas, entry->bg, CALLBACK_MOUSE_IN,
		     _elogin_entry_mouse_in, entry);
  evas_callback_add (evas, entry->bg, CALLBACK_MOUSE_OUT,
		     _elogin_entry_mouse_out, entry);

  evas_callback_add (evas, entry->text, CALLBACK_MOUSE_IN,
		     _elogin_entry_mouse_in, entry);
  evas_callback_add (evas, entry->text, CALLBACK_MOUSE_OUT,
		     _elogin_entry_mouse_out, entry);

  evas_callback_add (evas, entry->cursor, CALLBACK_MOUSE_IN,
		     _elogin_entry_mouse_in, entry);
  evas_callback_add (evas, entry->cursor, CALLBACK_MOUSE_OUT,
		     _elogin_entry_mouse_out, entry);

  evas_callback_add (evas, entry->bg, CALLBACK_MOUSE_DOWN,
		     _elogin_entry_mouse_down, entry);
  evas_callback_add (evas, entry->bg, CALLBACK_MOUSE_UP,
		     _elogin_entry_mouse_up, entry);

  evas_callback_add (evas, entry->text, CALLBACK_MOUSE_DOWN,
		     _elogin_entry_mouse_down, entry);
  evas_callback_add (evas, entry->text, CALLBACK_MOUSE_UP,
		     _elogin_entry_mouse_up, entry);

  evas_callback_add (evas, entry->cursor, CALLBACK_MOUSE_DOWN,
		     _elogin_entry_mouse_down, entry);
  evas_callback_add (evas, entry->cursor, CALLBACK_MOUSE_UP,
		     _elogin_entry_mouse_up, entry);

  ecore_event_filter_handler_add (ECORE_EVENT_KEY_DOWN,
				  _elogin_entry_key_down);



  evas_set_clip (evas, entry->bg, entry->o->clip);
  evas_set_clip (evas, entry->text, entry->o->clip);
  evas_set_clip (evas, entry->cursor, entry->o->clip);
  evas_set_clip (evas, entry->fx, entry->o->clip);

  return entry;
}

void
elogin_entry_set_next_selected (Elogin_Entry * entry)
{
  Evas_List l;
  int b;
  int i;
  int t = 0;
  int nb = 0;
  for (i = 0; i < 3; i++)
    {
      b = 0;
      for (l = selected->b->entries; l; l = l->next)
	{
	  Elogin_Entry *ent;
	  ent = l->data;
	  b++;

	  if (i == 0)
	    {
	      nb++;
	    }
	  if (i == 1)
	    {
	      if (ent->selected == 1)
		{
		  elogin_entry_set_normal (ent);

		  if (b == nb)
		    {
		      t = 1;
		      if (strcmp (ent->ch, "Tab"))
			{
			  for (l = selected->b->buttons; l; l = l->next)
			    {
			      Elogin_Button *but;
			      but = l->data;
			      if (but->login == 1)
				{
				  void (*clicked) (void *data);
				  printf ("Login !\n");
				  clicked = but->clicked;
				  clicked (NULL);
				}
			    }
			}
		    }
		  if (b < nb)
		    {
		      t = b + 1;
		      break;
		    }
		}
	    }

	  if (i == 2)
	    {
	      if (t == b)
		{
		  elogin_entry_set_selected (ent);
		  return;
		}
	    }
	}
    }
}


void
elogin_entry_set_normal (Elogin_Entry * entry)
{
  entry->selected = 0;
  evas_set_color (evas, entry->bg, 255, 255, 255, 128);
  evas_set_color (evas, entry->text, 0, 0, 0, 128);
  evas_set_color (evas, entry->cursor, 255, 255, 255, 128);
}

void
elogin_entry_set_hilited (Elogin_Entry * entry)
{
  evas_set_color (evas, entry->bg, 255, 255, 255, 192);
  evas_set_color (evas, entry->text, 0, 0, 0, 192);
  evas_set_color (evas, entry->cursor, 255, 255, 255, 192);
}

void
elogin_entry_set_selected (Elogin_Entry * entry)
{
  ecore_del_event_timer ("e_get_key()");
  entry->selected = 1;
  selected = entry;
  evas_set_color (evas, entry->bg, 255, 255, 255, 255);
  evas_set_color (evas, entry->text, 0, 0, 0, 224);
  evas_set_color (evas, entry->cursor, 255, 255, 255, 255);
  _elogin_entry_get_key (0, entry);
}

int
elogin_entry_get_text_len (Elogin_Entry * entry)
{
  return strlen (entry->t);
}

void
elogin_entry_get_char_width (Elogin_Entry * entry, int index, double *w)
{
  double tcx, tcy, tch;
  evas_text_at (evas, entry->text, index, &tcx, &tcy, w, &tch);
}

void
elogin_entry_set_cursor_pos (Elogin_Entry * entry, int x, int y)
{
  entry->cx = x + entry->o->x;
  entry->cy = y + entry->o->y;
}

void
elogin_entry_cursor_from_text (Elogin_Entry * entry)
{
  int i;
  int pos = 0;
  double w;
  int lst;
  for (i = 0; i < entry->cp; i++)
    {
      elogin_entry_get_char_width (entry, i, &w);
      pos += w;
      lst = w;
    }
  elogin_entry_set_cursor_pos (entry, pos, 0);
  elogin_entry_update_cursor (entry);

  if (entry->passwd == 0)
    evas_move (evas, entry->fx, entry->o->x + pos - w + 1, entry->o->y);
  else
    evas_move (evas, entry->fx, entry->o->x + pos - w + 1, entry->o->y + 3);
}


void
elogin_entry_update_cursor (Elogin_Entry * entry)
{
  evas_move (evas, entry->cursor, entry->cx, entry->cy);
}


void
elogin_entry_update_text (Elogin_Entry * entry)
{
  double w;
  if (entry->passwd == 0)
    {
      evas_set_text (evas, entry->text, entry->t);
    }
  else
    {
      int i;
      char tmp[1024];
      tmp[0] = 0;
      for (i = 0; i < strlen (entry->t); i++)
	{

	  sprintf (tmp, "%s*", tmp);
	}
      evas_set_text (evas, entry->text, tmp);
    }


}

void
elogin_entry_set_text (Elogin_Entry * entry, char *text)
{
  sprintf (entry->t, "%s", text);
  elogin_entry_update_text (entry);
  evas_set_color (evas, entry->text, 0, 0, 0, 255);
}

char *
elogin_get_text (Elogin_Entry * entry)
{
  return strdup (entry->t);
}


void
elogin_entry_set_pos (Elogin_Entry * entry, int x, int y)
{
  elogin_entry_set_abs_pos (entry, x + entry->b->o->x, y + entry->b->o->y);
}

void
elogin_entry_set_abs_pos (Elogin_Entry * entry, int x, int y)
{


  elogin_object_set_pos (entry->o, x, y);



  evas_move (evas, entry->bg, entry->o->x, entry->o->y);
  if (entry->passwd == 0)
    {
      evas_move (evas, entry->text, entry->o->x + 2, entry->o->y + 1);
    }
  else
    {
      evas_move (evas, entry->text, entry->o->x + 2, entry->o->y + 5);
    }
  elogin_entry_cursor_from_text (entry);

  evas_hide (evas, entry->fx);

}


void
elogin_entry_set_size (Elogin_Entry * entry, int h, int w)
{
  elogin_object_set_size (entry->o, h, w);
  evas_resize (evas, entry->bg, entry->o->h, entry->o->w);
  evas_set_image_fill (evas, entry->bg, 0, 0, entry->o->h, entry->o->w);
  evas_resize (evas, entry->cursor, 4, entry->o->w - 2);
  evas_set_image_fill (evas, entry->cursor, 0, 0, 4, entry->o->w - 2);
}

void
elogin_entry_add_to_box (Elogin_Box * box, Elogin_Entry * entry)
{
  box->entries = evas_list_append (box->entries, entry);
  evas_set_layer (evas, entry->bg, 11);
  evas_set_layer (evas, entry->text, 12);
  evas_set_layer (evas, entry->cursor, 12);
  evas_set_layer (evas, entry->fx, 13);
  entry->b = box;
  elogin_entry_set_pos (entry, box->o->x, box->o->y);
}

void
elogin_entry_show (Elogin_Entry * entry)
{
  elogin_object_show (entry->o);
  evas_show (evas, entry->bg);
  evas_show (evas, entry->text);
  evas_show (evas, entry->cursor);
}

void
elogin_entry_hide (Elogin_Entry * entry)
{
  elogin_object_hide (entry->o);
  evas_hide (evas, entry->bg);
  evas_hide (evas, entry->text);
  evas_hide (evas, entry->cursor);
}
