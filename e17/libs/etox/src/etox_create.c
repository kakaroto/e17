#include "Etox_private.h"
#include "Etox.h"

int __check_if_fits(Etox e, Etox_Font font, char *str, double w,
                    double offset);

static void
__create_etox_objects(Etox e, Etox_All_Bits bits,
                      double *x, double *y, double *w, double *h);
static void
__create_evas_objects(Etox e, Etox_Object obj);

static void
__create_etox_objects(Etox e, Etox_All_Bits bits, 
                      double *x, double *y, double *w, double *h)
{
  Etox_Object et_obj;
  char *todo = NULL, *p;

  if (!e || !bits)
    return;

  todo = strdup(bits->text->str);

  et_obj = _etox_object_new(*x, *y, bits);
  et_obj->str = strdup(todo);

  while (1)
    {
      if (!_etox_object_get_available_size(e, et_obj))
        {
          _etox_object_free(et_obj);
          break;
        }
      if (_etox_object_get_string_that_fits(e, et_obj))
        {
          D_PRINT("Appending: %p\n", et_obj);
          ewd_dlist_append(e->etox_objects, et_obj);
        
	  p = etox_str_chop_off_beginning_string(todo, et_obj->str);
          IF_FREE(todo);
          if (p[0] == '\n')
            p[0] = ' ';
          todo = etox_str_remove_beginning_spaces(p);
          IF_FREE(p);

          *x = et_obj->x;
          *y = et_obj->y;
          *w = et_obj->w;
          *h = et_obj->h;

          if (!strlen(todo))
            return;

          et_obj = _etox_object_new(*x, *y, bits);
          et_obj->str = strdup(todo);
        }

      _etox_object_move(e, et_obj);
    }

  IF_FREE(todo);
}

static void
__create_evas_objects(Etox e, Etox_Object obj)
{
  Evas_Object ev_obj, prev = NULL;
  Etox_Style_Bit style_bit;
  Etox_Color_Bit cb;
  double x, y, real_w, real_h;

  if (!e || !obj)
    return;

  if (obj->ev_objects)
    ewd_list_destroy(obj->ev_objects);

  obj->ev_objects = ewd_list_new();
  ewd_list_set_free_cb(obj->ev_objects, NULL);

  _etox_get_string_width(e, obj->bit.font, obj->str, &real_w);
  real_w += obj->bit.style->offset_w;
  real_h = (obj->bit.font->ascent - obj->bit.font->descent)
           + obj->bit.style->offset_h;

  switch (obj->bit.align->v)
    {                   
    case ETOX_ALIGN_TYPE_TOP:
      y = obj->y;            
      break;     
    case ETOX_ALIGN_TYPE_CENTER:
    default:
      y = obj->y + ((obj->h - real_h) / 2);
      break;                               
    case ETOX_ALIGN_TYPE_BOTTOM:
      y = obj->y + obj->h - real_h;
      break;                       
    }

  switch (obj->bit.align->h)
    {                   
    case ETOX_ALIGN_TYPE_LEFT:
      x = obj->x;
      break;                               
    case ETOX_ALIGN_TYPE_CENTER:
    default:
      x = obj->x + ((obj->w - real_w) / 2);
      break;                              
    case ETOX_ALIGN_TYPE_RIGHT:           
      x = obj->x + obj->w - real_w;
      break;
    }

  ewd_list_goto_first(obj->bit.style->bits);
  ewd_dlist_goto_first(e->evas_objects);
  while ((style_bit = (Etox_Style_Bit) ewd_list_next(obj->bit.style->bits)))
    {                                    
      ev_obj = evas_add_text(e->evas, obj->bit.font->name,
                             obj->bit.font->size, obj->str);
      evas_stack_above(e->evas, ev_obj, prev);
      prev = ev_obj;
      ewd_dlist_append(e->evas_objects, ev_obj);

      evas_move(e->evas, ev_obj,
                ET_X_TO_EV(x) + style_bit->x,
                ET_Y_TO_EV(y) + style_bit->y);
              
      switch (style_bit->type)                
        {
        case ETOX_STYLE_TYPE_FOREGROUND:
        default:
          if ((cb = _etox_color_get_bit(obj->bit.color, "fg")))
            evas_set_color(e->evas, ev_obj, cb->r, cb->g, cb->b,
                           ((style_bit->a * e->a * cb->a) / 65025));
          break;
        case ETOX_STYLE_TYPE_SHADOW:
          if ((cb = _etox_color_get_bit(obj->bit.color, "sh")))
            evas_set_color(e->evas, ev_obj, cb->r, cb->g, cb->b,
                           ((style_bit->a * e->a * cb->a) / 65025));
          break;
        case ETOX_STYLE_TYPE_OUTLINE:
          if ((cb = _etox_color_get_bit(obj->bit.color, "ol")))
            evas_set_color(e->evas, ev_obj, cb->r, cb->g, cb->b,
                           ((style_bit->a * e->a * cb->a) / 65025));
          break;
        }

      ewd_list_append(obj->ev_objects, ev_obj);
    }                                                           
}


void
_etox_create_etox_objects(Etox e)
{
  struct _Etox_All_Bits bits;
  Etox_Bit bit;
  Etox_Text text;
  double x, y, w, h;

  if (!e || !e->bits)
    return;

  if (e->etox_objects)
    ewd_dlist_destroy(e->etox_objects); 

  e->etox_objects = ewd_dlist_new();
  ewd_dlist_set_free_cb(e->etox_objects, EWD_FREE_CB(_etox_object_free));

  /* put defaults.. */
  bits.align = e->def.align;
  bits.callback = e->def.callback;
  bits.color = e->def.color;
  bits.font = e->def.font;
  bits.style = e->def.style;
  bits.text = e->def.text;

  x = 0.0;
  y = 0.0;
  w = 0.0;
  h = 0.0;

  ewd_list_goto_first(e->bits);
  while ((bit = (Etox_Bit) ewd_list_next(e->bits)))
    {
      if (bit->body)
	{
	  switch (bit->type)
	    {
	    case ETOX_BIT_TYPE_ALIGN:
              D_PRINT("Creating an align..\n");
	      bits.align = (Etox_Align) bit->body;
	      break;
	    case ETOX_BIT_TYPE_CALLBACK:
	      D_PRINT("Creating a callback..\n");
	      bits.callback = (Etox_Callback) bit->body;
	      break;
	    case ETOX_BIT_TYPE_COLOR:
              D_PRINT("Creating a color..\n");
	      bits.color = (Etox_Color) bit->body;
	      break;
	    case ETOX_BIT_TYPE_FONT:
	      D_PRINT("Creating a font..\n");
	      bits.font = (Etox_Font) bit->body;
	      break;
	    case ETOX_BIT_TYPE_STYLE:
	      D_PRINT("Creating a style..\n");
	      bits.style = (Etox_Style) bit->body;
	      break;
	    case ETOX_BIT_TYPE_TEXT:
	      D_PRINT("Creating a text..\n");
              text = (Etox_Text) bit->body;
	      bits.text = _etox_bit_text_new(text->str);
              /* FIXME: this never gets free'ed */

              if (!ewd_dlist_is_empty(e->etox_objects))
                {
                  static Etox_Text prev = NULL;
                  Etox_Object obj;
                  char *p = NULL, *q = NULL;
                  int index = 0;

                  /* If we're still working with the same alignment
                   * and font(size), don't go to the next line but 
                   * continue where we stopped..
                   * So..
                   *    ET_TEXT(..), ET_TEXT(..), ET_TEXT(..)
                   * should show the same result as..
                   *    ET_TEXT(......)  
                   * instead of going to a new line 2 times.
                   *
                   * Kinda hard to explain it here in english, mail /me
                   * if you don't understand it.. :)  -redalb
                   */

                  /* FIXME: it's impossible to have different colors etc at
 		   *        the same line this way, so it should be rewritten
                   *        at some point..
                   */

                  ewd_dlist_goto_last(e->etox_objects); 
                  obj = (Etox_Object) ewd_dlist_current(e->etox_objects);
                  if (!strcmp(bits.font->name, obj->bit.font->name) &&
                      (bits.font->size == obj->bit.font->size)      &&
                      (bits.align->v == obj->bit.align->v)          &&
                      (bits.align->h == obj->bit.align->h)          &&
                      (prev != NULL) && strlen(prev->str)           )       
                    {
                      ewd_dlist_goto_last(e->etox_objects);
                      while ((obj = (Etox_Object) 
                              ewd_dlist_previous(e->etox_objects)))
                        {  
                          if ((obj->x == 0.0) && strlen(obj->str))
                            {
                              x = obj->x;
                              y = obj->y;
                              w = obj->w;
                              index = ewd_dlist_index(e->etox_objects) + 1;
                              p = strstr(prev->str, obj->str);
                              break;
                            }
                        } 

                      q = malloc((sizeof(char) * strlen(p)) +
                                 (sizeof(char) * strlen(bits.text->str)) + 1);
                      memset(q, 0, (sizeof(char) * strlen(p)) +
                                   (sizeof(char) * strlen(bits.text->str)) + 1);
                      strcpy(q, p);
                      strcat(q, bits.text->str);
                      IF_FREE(bits.text->str);
                      bits.text->str = q;

                      while ((obj = (Etox_Object) 
                              ewd_dlist_remove_last(e->etox_objects)))
                        {
                          _etox_object_free(obj);
                          if (ewd_dlist_nodes(e->etox_objects) < index)
                            break;
                        }
                    }
                  else
                    {
                      x = 0.0;
                      y += (obj->bit.font->ascent - obj->bit.font->descent)
                           + e->padding;                      
                      w = e->w;
                    }

                  prev = bits.text;
                }
	      __create_etox_objects(e, &bits, &x, &y, &w, &h);
	      break;
	    default:
	      D_PRINT("Error while creating (1) [bit->type %d]..\n", bit->type);
	      break;
	    }
	}
      else
	{
	  switch (bit->type)
	    {
	    case ETOX_BIT_TYPE_ALIGN_END:
              D_PRINT("Ending an align..\n"); 
	      bits.align = e->def.align;
	      break;
	    case ETOX_BIT_TYPE_CALLBACK_END:
              D_PRINT("Ending a callback..\n"); 
	      bits.callback = e->def.callback;
	      break;
	    case ETOX_BIT_TYPE_COLOR_END:
              D_PRINT("Ending a color..\n"); 
	      bits.color = e->def.color;
	      break;
	    case ETOX_BIT_TYPE_FONT_END:
              D_PRINT("Ending a font..\n"); 
	      bits.font = e->def.font;
	      break;
	    case ETOX_BIT_TYPE_STYLE_END:
              D_PRINT("Ending a style..\n"); 
	      bits.style = e->def.style;
	      break;
	    default:
	      D_PRINT("Error while creating (2) [bit->type %d]..\n", bit->type);
	      break;
	    }
	}
    }
}

void
_etox_create_evas_objects(Etox e)
{
  Etox_Object et_obj;
  Evas_Object ev_obj;

  if (!e || !e->etox_objects)
    return;

  if (e->evas_objects)
    {                   
      ewd_dlist_goto_first(e->evas_objects);
      while ((ev_obj = (Evas_Object) ewd_dlist_next(e->evas_objects)))
        evas_del_object(e->evas, ev_obj);
      ewd_dlist_destroy(e->evas_objects);
    } 

  e->evas_objects = ewd_dlist_new();
  ewd_dlist_set_free_cb(e->evas_objects, NULL);

  ewd_dlist_goto_first(e->etox_objects);
  while ((et_obj = (Etox_Object) ewd_dlist_next(e->etox_objects)))
    __create_evas_objects(e, et_obj);
}
