#include "ui.h"

/* Elogin_Box callbacks */

static void
_elogin_box_mouse_down (void *_data, Ebits_Object _o,
			char *_c, int _b, int _x, int _y,
			int _ox, int _oy, int _ow, int _oh)
{
  Elogin_Box *box;
  Evas_List l;

  box = _data;
  box->move = 1;
  box->o->dx = _x - box->o->x;
  box->o->dy = _y - box->o->y;

  for (l = box->labels; l; l = l->next)
    {
      Elogin_Label *lab;
      lab = l->data;
      lab->o->dx = _x - lab->o->x;
      lab->o->dy = _y - lab->o->y;
    }
  for (l = box->entries; l; l = l->next)
    {
      Elogin_Entry *ent;
      ent = l->data;
      ent->o->dx = _x - ent->o->x;
      ent->o->dy = _y - ent->o->y;


    }
  for (l = box->buttons; l; l = l->next)
    {
      Elogin_Button *but;
      but = l->data;
      but->o->dx = _x - but->o->x;
      but->o->dy = _y - but->o->y;


    }

}

static void
_elogin_box_mouse_up (void *_data, Ebits_Object _o,
		      char *_c, int _b, int _x, int _y,
		      int _ox, int _oy, int _ow, int _oh)
{
  Elogin_Box *box;
  box = _data;
  box->move = 0;
}

static void
_elogin_box_mouse_move (void *_data, Ebits_Object _o,
			char *_c, int _b, int _x, int _y,
			int _ox, int _oy, int _ow, int _oh)
{
  Elogin_Box *box;

  box = _data;
  if (box->move == 1)
    {
      Evas_List l;
      for (l = box->labels; l; l = l->next)
	{
	  Elogin_Label *lab;
	  lab = l->data;
	  elogin_label_set_abs_pos (lab, _x - lab->o->dx, _y - lab->o->dy);

	}
      for (l = box->entries; l; l = l->next)
	{
	  Elogin_Entry *ent;
	  ent = l->data;
	  elogin_entry_set_abs_pos (ent, _x - ent->o->dx, _y - ent->o->dy);

	}
      for (l = box->buttons; l; l = l->next)
	{
	  Elogin_Button *but;
	  but = l->data;
	  elogin_button_set_abs_pos (but, _x - but->o->dx, _y - but->o->dy);

	}
      elogin_box_set_pos (box, _x - box->o->dx, _y - box->o->dy);

    }
}



/* Elogin_Box funcs */

Elogin_Box *
elogin_box_new (void)
{
  Elogin_Object *obj;
  Elogin_Box *box;


  obj = elogin_object_new ();

  box = NEW (Elogin_Box, 1);
  ZERO (box, Elogin_Box, 1);

  box->o = obj;

  box->move = 0;

  box->labels = NULL;
  box->entries = NULL;
  box->buttons = NULL;

  box->b = ebits_load (PACKAGE_DATA_DIR "/elogin/data/elogin.bits.db");
  if (box->b)
    {

      ebits_add_to_evas (box->b, evas);
      ebits_set_layer (box->b, 9);

      ebits_set_classed_bit_callback (box->b, "Title_Bar",
				      CALLBACK_MOUSE_DOWN,
				      _elogin_box_mouse_down, box);
      ebits_set_classed_bit_callback (box->b, "Title_Bar",
				      CALLBACK_MOUSE_UP,
				      _elogin_box_mouse_up, box);
      ebits_set_classed_bit_callback (box->b, "Title_Bar",
				      CALLBACK_MOUSE_MOVE,
				      _elogin_box_mouse_move, box);
    }

  ebits_set_clip (box->b, box->o->clip);


  return box;
}


void
elogin_box_set_size (Elogin_Box * box, int h, int w)
{
  elogin_object_set_size (box->o, h, w);

  if (box->b)
    {
      ebits_resize (box->b, box->o->h, box->o->w);
    }
}

void
elogin_box_set_pos (Elogin_Box * box, int x, int y)
{
  elogin_object_set_pos (box->o, x, y);

  if (box->b)
    {
      ebits_move (box->b, box->o->x, box->o->y);
    }
}


void
elogin_box_show (Elogin_Box * box)
{
  ebits_show (box->b);
  elogin_object_show (box->o);
}

void
elogin_box_hide (Elogin_Box * box)
{
  ebits_hide (box->b);
  elogin_object_hide (box->o);
}

void
elogin_box_free (Elogin_Box * box)
{
  Evas_List l;

  for (l = box->labels; l; l = l->next)
    {
      Elogin_Label *lab;
      lab = l->data;
      FREE (lab);


    }
  for (l = box->entries; l; l = l->next)
    {
      Elogin_Entry *ent;
      ent = l->data;
      FREE (ent);


    }
  for (l = box->buttons; l; l = l->next)
    {
      Elogin_Button *but;
      but = l->data;
      FREE (but);


    }

  box->labels = evas_list_free (box->labels);
  box->entries = evas_list_free (box->entries);
  box->buttons = evas_list_free (box->buttons);
  if (box->b)
    ebits_free (box->b);
  FREE (box);
}
