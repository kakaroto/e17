#include "Etox_private.h"
#include "Etox.h"

int __check_if_fits(Etox e, Etox_Font font, char *str, double w,
                    double offset);

int
__check_if_fits(Etox e, Etox_Font font, char *str, double w, double
offset)
{
  double my_w;
  static Evas_Object text = NULL;
 
  if (!text)
    text = evas_add_text(e->evas, font->name, font->size, str);
  else
    {
      evas_set_text(e->evas, text, str);
      evas_set_font(e->evas, text, font->name, font->size);
    }

  evas_get_geometry(e->evas, text, NULL, NULL, &my_w, NULL);

  if ((my_w + offset) > w)
    return 0;

  return 1;
}


Etox_Object
_etox_object_new(double x, double y, Etox_All_Bits bits)
{
  Etox_Object obj;

  obj = malloc(sizeof(struct _Etox_Object));
  obj->x = x;
  obj->y = y;
  obj->w = 0.0;
  obj->h = 0.0;
  obj->str = NULL;
  obj->ev_objects = NULL;

  obj->bit.align = bits->align;
  obj->bit.callback = bits->callback;
  obj->bit.color = bits->color;
  obj->bit.font = bits->font;
  obj->bit.style = bits->style;
  obj->bit.text = bits->text;

  return obj;  
}

void
_etox_object_free(Etox_Object obj)
{
  IF_FREE(obj->str);
  if (obj->ev_objects)
    ewd_list_destroy(obj->ev_objects);
  IF_FREE(obj);
}

int
_etox_object_get_available_size(Etox e, Etox_Object obj)
{
  Etox_Obstacle obst;
  double w = 0.0;

  obj->w = e->w;
  obj->h = (obj->bit.font->ascent - obj->bit.font->descent) + e->padding;

  if (!e->obstacles)
    return 1;

  ewd_list_goto_first(e->obstacles);
  while ((obst = (Etox_Obstacle) ewd_list_next(e->obstacles)))
    {
      if ( ((ET_Y_TO_EV(obj->y) > obst->y) &&
            (ET_Y_TO_EV(obj->y) < (obst->y + obst->h))) ||
           ((ET_Y_TO_EV(obj->y + obj->h) > obst->y) &&
            (ET_Y_TO_EV(obj->y + obj->h) < (obst->y + obst->h))) ||
           ((ET_Y_TO_EV(obj->y) < obst->y) &&
            (ET_Y_TO_EV(obj->y + obj->h) > obst->y)) )
        {
          /* we're at the hight of an obstacle */
          if (ET_X_TO_EV(obj->x) < obst->x)
            {
              /* we're at the left of the obstacle */
              w = obst->x - ET_X_TO_EV(obj->x);
            }
          else                                                             
            if (ET_X_TO_EV(obj->x) > obst->x)
              {                              
                /* we're at the right of the obstacle */
                w = (e->x + e->w) - (obst->x + obst->w);
              }
          if (w && (w < obj->w))                  
            obj->w = w;
        }
    }

  return 1;
}

int 
_etox_object_get_string_that_fits(Etox e, Etox_Object obj)
{
  char *str, *p, *q;

  if (!obj->str)
    return 0;    

  str = strdup(obj->str);

  /* If the first word of the string doesn't fit in the etox, the
   * nothing will..  -redalb
   */
  if ((p = etox_str_remove_beginning_spaces(str)))
    {                                             
      if (strchr(p, ' '))                             
        {
          for (q = p; *q != ' '; *q++);
          q = etox_str_chop_off_ending_string(p, q);
        } 
      else
        q = strdup(p);
      if (!__check_if_fits(e, obj->bit.font, q, obj->w,
                           obj->bit.style->offset_w))
        {
          FREE(p);
          IF_FREE(q);
          IF_FREE(str);
          return 0;
        }
      FREE(p);
      IF_FREE(q);
    }

  while (!__check_if_fits(e, obj->bit.font, str, obj->w,
                          obj->bit.style->offset_w))
    {
      q = etox_str_chop_off_ending_word(str);
      IF_FREE(str);                             
      str = etox_str_remove_ending_spaces(q);
      IF_FREE(q);

      if (!strlen(str))
        {                          
          IF_FREE(str);
          return 0;                  
        }
    }

  if (str && (p = strchr(str, '\n')))
    {
      q = etox_str_chop_off_ending_string(str, p);
      IF_FREE(str);                                  
      str = q;
    }

  IF_FREE(obj->str);
  obj->str = str;

  return 1;
}

void
_etox_object_move(Etox e, Etox_Object obj)
{
  Etox_Obstacle obst;

  if (e->obstacles)
    {
      ewd_list_goto_first(e->obstacles);
      while ((obst = ewd_list_next(e->obstacles)))
	{
	  if ( ((ET_Y_TO_EV(obj->y) > obst->y) &&
		(ET_Y_TO_EV(obj->y) < (obst->y + obst->h))) ||
	       ((ET_Y_TO_EV(obj->y + obj->h) > obst->y) &&
		(ET_Y_TO_EV(obj->y + obj->h) < (obst->y + obst->h))) ||
	       ((ET_Y_TO_EV(obj->y) < obst->y) &&
		(ET_Y_TO_EV(obj->y + obj->h) > obst->y)) )
	    {
	      /* we're at the hight of an obstacle */
	      if (!(ET_X_TO_EV(obj->x) > obst->x))
		{
		  obj->x = EV_X_TO_ET(obst->x + obst->w);
		  obj->w = (e->x + e->w) - (obst->x + obst->w);
		  
		  if (!(obj->x >= (e->x + e->w)))                              
		    return;
		}                                
	    }
	}
    }

  obj->x = 0.0;
  obj->y += (obj->bit.font->ascent - obj->bit.font->descent) + e->padding;
  obj->w = e->w;
}
