#include "Etox_private.h"
#include "Etox.h"

/* _etox_create_etox_objects() helper funcs */
static int __get_next_all_bits(Etox e, Ewd_List *bits_list, Etox_All_Bits bits);
static char *__my_strchr(char *str, int *c);
static Ewd_List *__get_etox_object_bits_list(Etox e);
static int __etox_get_next_obstacle_to_the_right(Etox e, Etox_Sort sort, 
                                                 double x, int *inside);
static Etox_Object __create_etox_object(Etox e, double *x, double *y);
static int __compare_obstacles(void *ptr1, void *ptr2);
static Ewd_List *__add_as_many_bits_as_possible_from_list(Etox e, 
                                                          Etox_Object obj, 
                                                          Ewd_List *list);

/* _etox_create_evas_objects() helper funcs */
static void __create_evas_objects(Etox e, Etox_Object obj);


static int
__get_next_all_bits(Etox e, Ewd_List *bits_list, Etox_All_Bits bits) 
{ 
  Etox_Bit bit; 
 
  if (!bits)
    return 0;
 
  while ((bit = (Etox_Bit) ewd_list_next(bits_list))) 
    { 
      if (bit->body) 
        { 
          switch (bit->type) 
            { 
            case ETOX_BIT_TYPE_ALIGN: 
              D_PRINT("Creating an align..\n"); 
              bits->align = (Etox_Align) bit->body; 
              break; 
            case ETOX_BIT_TYPE_CALLBACK: 
              D_PRINT("Creating a callback..\n"); 
              bits->callback = (Etox_Callback) bit->body; 
              break; 
            case ETOX_BIT_TYPE_COLOR: 
              D_PRINT("Creating a color..\n"); 
              bits->color = (Etox_Color) bit->body; 
              break; 
            case ETOX_BIT_TYPE_FONT: 
              D_PRINT("Creating a font..\n"); 
              bits->font = (Etox_Font) bit->body; 
              break; 
            case ETOX_BIT_TYPE_STYLE: 
              D_PRINT("Creating a style..\n"); 
              bits->style = (Etox_Style) bit->body; 
              break; 
            case ETOX_BIT_TYPE_TEXT: 
              D_PRINT("Creating a text..\n"); 
              bits->text = (Etox_Text) bit->body;
 
              return 1; 
 
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
              bits->align = e->def.align; 
              break; 
            case ETOX_BIT_TYPE_CALLBACK_END: 
              D_PRINT("Ending a callback..\n");  
              bits->callback = e->def.callback; 
              break; 
            case ETOX_BIT_TYPE_COLOR_END: 
              D_PRINT("Ending a color..\n");  
              bits->color = e->def.color; 
              break; 
            case ETOX_BIT_TYPE_FONT_END: 
              D_PRINT("Ending a font..\n");  
              bits->font = e->def.font; 
              break; 
            case ETOX_BIT_TYPE_STYLE_END: 
              D_PRINT("Ending a style..\n");  
              bits->style = e->def.style; 
              break; 
            default: 
              D_PRINT("Error while creating (2) [bit->type %d]..\n", bit->type); 
              break; 
            } 
        } 
    } 

  return 0;
}

static char *
__my_strchr(char *str, int *c)
{
  char *p = NULL;
  char chars[] = { '\n', '\t', '\0' };
  int i;

  if (!str)
    return NULL;

  for (p = str; *p; *p++)
    for (i = 0; i < strlen(chars); i++)
      if (*p == chars[i])
        {
          *c = chars[i];
          return p;
        }

  return NULL;
}

static Ewd_List *
__get_etox_object_bits_list(Etox e)
{
  Ewd_List *bit_list = NULL, *ret = NULL;
  void *temp;
  struct _Etox_All_Bits bits;
  Etox_Object_Bit obj_bit;
  char *p = NULL, *q = NULL, *r = NULL;
  int c;

  if (!e || !e->bits)
     return NULL;

  /* only rebuild this list when the underlying bit list changes. */
  if (e->etox_object_bits.dirty)
  { 
     bit_list = ewd_list_new();
     ewd_list_set_free_cb(bit_list,EWD_FREE_CB(_etox_object_bit_free));

     /* put defaults.. */
     bits.align = e->def.align;
     bits.callback = e->def.callback;
     bits.color = e->def.color;
     bits.font = e->def.font;
     bits.style = e->def.style;
     bits.text = e->def.text;

     ewd_list_goto_first(e->bits);
     while (__get_next_all_bits(e, e->bits, &bits))
     {
	if ((p = __my_strchr(bits.text->str, &c)))
	{
	   r = bits.text->str;
	   do
	   {
	      IF_FREE(q);
	      q = etox_str_chop_off_ending_string(r, p);
	      obj_bit = _etox_object_bit_new();
	      _etox_object_bit_set_body(e, obj_bit,
		    _etox_object_string_new(q,
		       bits.align,
		       bits.callback,
		       bits.color,
		       bits.font,
		       bits.style),
		    ETOX_OBJECT_BIT_TYPE_STRING);
	      ewd_list_append(bit_list, obj_bit);
	      switch (c)
	      {
		 case '\n':
		    obj_bit = _etox_object_bit_new();
		    _etox_object_bit_set_body(e, obj_bit, 
			  _etox_object_newline_new(),
			  ETOX_OBJECT_BIT_TYPE_NEWLINE);
		    ewd_list_append(bit_list, obj_bit);
		    break;
		 case '\t':
		    obj_bit = _etox_object_bit_new();
		    _etox_object_bit_set_body(e, obj_bit, 
			  _etox_object_tab_new(bits.align,
			     bits.callback,
			     bits.font),
			  ETOX_OBJECT_BIT_TYPE_TAB);
		    ewd_list_append(bit_list, obj_bit);
		    break;
		 default:
		    D_PRINT("Error!\n");
		    break;
	      }

	      r = ++p;

	   } while ((p = __my_strchr(r, &c)));

	   if ((p = strrchr(bits.text->str, c)) && strlen(p))
	   {
	      p++;
	      obj_bit = _etox_object_bit_new();
	      _etox_object_bit_set_body(e, obj_bit,
		    _etox_object_string_new(p,
		       bits.align,
		       bits.callback,
		       bits.color,
		       bits.font,
		       bits.style),
		    ETOX_OBJECT_BIT_TYPE_STRING);
	      ewd_list_append(bit_list, obj_bit);              
	   }
	}
	else
	{
	   obj_bit = _etox_object_bit_new();
	   _etox_object_bit_set_body(e, obj_bit,
		 _etox_object_string_new(bits.text->str,
		    bits.align,
		    bits.callback,
		    bits.color,
		    bits.font,
		    bits.style),
		 ETOX_OBJECT_BIT_TYPE_STRING);
	   ewd_list_append(bit_list, obj_bit);
	}
     }
     e->etox_object_bits.list = bit_list;
  }
  e->etox_object_bits.dirty = 0;
  
  /* 
   * Return a copy of the list because it will be consumed.
   * We don't need to free the bits because they'll be move to the
   * Etox_Objects..
   */
  ret = ewd_list_new();
  ewd_list_set_free_cb(ret, NULL);
  ewd_list_goto_first(e->etox_object_bits.list);
  while ((temp = ewd_list_next(e->etox_object_bits.list)))
     ewd_list_append(ret, temp);
  
  return ret;
}

static int
__compare_obstacles(void *ptr1, void *ptr2)
{
  Etox_Obstacle obst1, obst2;

  if (!ptr1 || !ptr2)
    return 0;

  obst1 = (Etox_Obstacle) ptr1;
  obst2 = (Etox_Obstacle) ptr2;

  if (obst1->x < obst2->x)
    return -1;
  else if (obst1->x > obst2->x)
    return 1;
  else
    return 0;
}

Ewd_List *
__etox_get_obstacles_at_height(Etox e, int height)
{
   Ewd_List *obst_list = NULL;
   Etox_Obstacle obst;

   obst_list = ewd_list_new();
   ewd_list_set_free_cb(obst_list, NULL);

   ewd_list_goto_first(e->obstacles);
   while ((obst = (Etox_Obstacle) ewd_list_next(e->obstacles)))
   {
      if ( ((ET_Y_TO_EV(height) > obst->y) &&
	       (ET_Y_TO_EV(height) < (obst->y + obst->h))) ||
	    ((ET_Y_TO_EV(height + e->etox_objects.h) > obst->y) &&
	     (ET_Y_TO_EV(height + e->etox_objects.h) < (obst->y + obst->h))) ||
	    ((ET_Y_TO_EV(height) < obst->y) &&
	     (ET_Y_TO_EV(height + e->etox_objects.h) > obst->y)) )
      {  
	 ewd_list_append(obst_list, obst);
      }
   }
   return obst_list;
}

static int
__etox_get_next_obstacle_to_the_right(Etox e, Etox_Sort sort, 
                                      double x, int *inside)
{
   Etox_Obstacle obst;
   int i;

  if (!e || !sort)
    return -1;

  /* iterate over obstacles and return the first one to the right of the
   * current coordinate. Set inside flag when we are inside the obstacle */

  for (i = 0; i <= _etox_sort_get_size(sort); i++)
  {
     obst = (Etox_Obstacle) _etox_sort_get_data(sort, i);

     if ( (ET_X_TO_EV(x) < obst->x) )
     {
	*inside = 0;
	return i;
     }
     else if ((ET_X_TO_EV(x) > obst->x) &&
	   (ET_X_TO_EV(x) < (obst->x + obst->w)))
     {
	/* we're in an obstacle */
	*inside = 1;
	return i;
     }
  }
  return -1;
}

static Etox_Object
__create_etox_object(Etox e, double *x, double *y)
{
  Ewd_List *obst_list = NULL;
  Etox_Object obj = NULL;
  Etox_Obstacle obst;
  Etox_Sort sort;
  int i;
  int inside= 0;

  if (!e)
    return NULL;

  obj = _etox_object_new(e, *x, *y);

  D_PRINT("xx Creating %p: -----\n", obj);
  /* Default is to advance to the next line */
  obj->w = e->w - obj->x;
  *x = 0.0;
  *y += e->etox_objects.h + e->padding;
  
  if (e->obstacles && !ewd_list_is_empty(e->obstacles))
    {
      D_PRINT("xx Obstacles..\n");
      obst_list = __etox_get_obstacles_at_height(e, obj->y);

      /* return whole row, there are no obstacles */
      if (!obst_list || ewd_list_is_empty(obst_list))
	 return obj;
             
      /* sort obstacles from left to right.. */
      sort = _etox_sort_new(ewd_list_nodes(obst_list) - 1);
      _etox_sort_set_data_from_list(sort, obst_list);
      _etox_sort_now(sort, 0, _etox_sort_get_size(sort), __compare_obstacles);	  
      i = __etox_get_next_obstacle_to_the_right(e, sort, obj->x, &inside);
      if (i >= 0)
      {
	 obst = (Etox_Obstacle) _etox_sort_get_data(sort, i);	 

	 /* the next obj will have to be to the right of obstacle */
	 *x = EV_X_TO_ET(obst->x) + obst->w;
	 *y -= e->etox_objects.h + e->padding;

	 if (inside)
	 {
	    /* inside, try again with new coordinates */
            printf("recures\n");
	    return __create_etox_object(e, x, y);   	 
	 }
	 else
	 {
	    obj->w = EV_X_TO_ET(obst->x) - obj->x;
	 }
      }
      ewd_list_destroy(obst_list);  
      _etox_sort_free(sort);
    }    
  D_PRINT("xx obj->w = %f\n", obj->w);
  D_PRINT("xx Created %p: -----\n", obj); 

  return obj;
}

static Ewd_List *
__add_as_many_bits_as_possible_from_list(Etox e, Etox_Object obj, 
                                         Ewd_List *list)
{
  /* FIXME: this func is _too long_ :)  -redalb */

  /*  Adds as many bits (Etox_Object_Bit) as possible from
   *  list to obj and returns the list of bits it couldn't add..
   *  The bits that could be added will be removed from list..
   */
  Ewd_List *todo = NULL;
  Etox_Object_Bit obj_bit;
  Etox_Object_String obj_str;
  char *p = NULL, *q = NULL, *r = NULL;
  double w = 0.0, my_w = 0.0;

  if (!e || !obj || !list || ewd_list_is_empty(list))
    return NULL;

  while (!ewd_list_is_empty(list))
    {
       obj_bit = _etox_object_bit_clone(
	     (Etox_Object_Bit) ewd_list_remove_first(list));
      if (!_etox_object_add_bit(obj, obj_bit))
        {
	 /* obj_bit didnt fit into the obj if there is space left in the obj,
	  * and it is a string bit, break the bit in two and try again with 
	  * the first part */
          if (_etox_object_get_available_size(obj, &w))
            switch (obj_bit->type)
	    {
               case ETOX_OBJECT_BIT_TYPE_STRING:
	       {
                Etox_Align align;
                Etox_Callback callback;
                Etox_Color color;
                Etox_Font font;
                Etox_Style style;

                obj_str = (Etox_Object_String) obj_bit->body;
                p = strdup(obj_str->str);
                r = strdup(obj_str->str);
                align = obj_str->align;
                callback = obj_str->callback;
                color = obj_str->color;
                font = obj_str->font;
                style = obj_str->style;

                do
                  {
                    IF_FREE(q);
                    if (!(q = etox_str_chop_off_ending_word(p)) || !strlen(q))
                      break;
                    if ((p = etox_str_remove_ending_spaces(q)))
                      {
                        IF_FREE(q);
                        q = strdup(p);
                        IF_FREE(p);
                      }
                    _etox_get_string_width(e, font, q, &my_w);
                    my_w += style->offset_w;
                    IF_FREE(p);
                    p = strdup(q);
                  } while (my_w > w);

                D_PRINT("* Fits: '%s'\n", q); 

                if (q && strlen(q))
                  {
                    _etox_object_bit_free(obj_bit);
                    obj_bit = _etox_object_bit_new();
                    _etox_object_bit_set_body(e, obj_bit,
                                              _etox_object_string_new(q,
                                                                      align,
                                                                      callback,
                                                                      color,
                                                                      font,
                                                                      style),
                                              ETOX_OBJECT_BIT_TYPE_STRING);
                    _etox_object_add_bit(obj, obj_bit);
                    D_PRINT("* Type = %d\n", obj_bit->type);
                    D_PRINT("* Added %p (%s) (body: %p)\n", obj_bit, q,
                            obj_bit->body);

                    IF_FREE(p);
                    p = etox_str_chop_off_beginning_string(r, q);
                    if ((q = etox_str_remove_beginning_spaces(p)))
                      {
                        IF_FREE(p);
                        p = strdup(q);
                        IF_FREE(q);
                      }
                    obj_bit = _etox_object_bit_new();
                    _etox_object_bit_set_body(e, obj_bit,
                                              _etox_object_string_new(p,
                                                                      align,
                                                                      callback,
                                                                      color,
                                                                      font,
                                                                      style),
                                              ETOX_OBJECT_BIT_TYPE_STRING);

                    if (!todo)
                      {
                        /* freeing is not necessary.. (see above) */
                        todo = ewd_list_new();
                        ewd_list_set_free_cb(todo, NULL);
                      }
                    ewd_list_append(todo, obj_bit);
                  }
                else
                  {
                    if (!todo)
                      {  
                        /* freeing is not necessary.. (see above) */
                        todo = ewd_list_new();
                        ewd_list_set_free_cb(todo, NULL);
                      }
                    ewd_list_append(todo, obj_bit);
                  } 

                IF_FREE(p);
                IF_FREE(q);
                IF_FREE(r);
                break;
	    }
            default:
              break;
            }
          else
            {
              /* add bit to todo if there isn't any space left.. */
              if (!todo)     
                {
                  /* freeing is not necessary.. (see above) */
                  todo = ewd_list_new();
                  ewd_list_set_free_cb(todo, NULL);
                }
              ewd_list_append(todo, obj_bit);
            }
          break;
        }
#ifdef DEBUG
      else
        {
          Etox_Object_String d_str;
          d_str = obj_bit->body;
          D_PRINT("* Type = %d\n", obj_bit->type);  
          D_PRINT("* Added %p (%s) (2)\n", obj_bit, d_str->str);
        }
#endif
    }

  return todo;
}

void
_etox_create_etox_objects(Etox e)
{
  Ewd_List *et_obj_bits, *l;
  Ewd_List *todo = NULL;
  Etox_Object obj;
  Etox_Object_Bit obj_bit;
  double x = 0.0, y = 0.0;

  if (!e || !e->bits || ewd_list_is_empty(e->bits) || !e->etox_objects.dirty)
    return;

  if (e->etox_objects.list)
    ewd_list_destroy(e->etox_objects.list);

  e->etox_objects.list = ewd_list_new();
  ewd_list_set_free_cb(e->etox_objects.list, EWD_FREE_CB(_etox_object_free)); 

  et_obj_bits = __get_etox_object_bits_list(e);

  /* set e->etox_objects.h to the height of the tallest object_bit */
  e->etox_objects.h = 0.0;
  ewd_list_goto_first(et_obj_bits);
  while ((obj_bit = (Etox_Object_Bit) ewd_list_next(et_obj_bits)))
    if (obj_bit->h > e->etox_objects.h)
      e->etox_objects.h = obj_bit->h;

  ewd_list_goto_first(et_obj_bits);
  while (y <= e->h)
    {
      if (!(obj = __create_etox_object(e, &x, &y)))
        break;
      ewd_list_append(e->etox_objects.list, obj);

      D_PRINT("* Adding to %p: -------\n", obj);

      if (todo)
        {
          D_PRINT("TODO: %d bit(s) todo\n", ewd_list_nodes(todo));
          l = __add_as_many_bits_as_possible_from_list(e, obj, todo);
          ewd_list_destroy(todo);
          todo = l;
          if (todo && !ewd_list_is_empty(todo))
            continue;
        }
      todo = __add_as_many_bits_as_possible_from_list(e, obj, et_obj_bits);
    }
  ewd_list_destroy(et_obj_bits);
  e->etox_objects.dirty = 0;
  _etox_create_evas_objects(e);
}


static void
__create_evas_objects(Etox e, Etox_Object obj)
{
  Evas_Object ev_obj, prev = NULL;
  Etox_Style_Bit style_bit;
  Etox_Color_Bit cb;
  Etox_Object_Bit bit;
  Etox_Object_String string;
  Etox_Object_Tab tab;

  if (!e || !obj || !obj->bits)
    return;

  D_PRINT("__create_evas_objects(): (obj = %p)\n", obj);

  D_PRINT("------ ALIGNING %p ------\n", obj);
  _etox_align_etox_object(e, obj);
  D_PRINT("-------------------------\n");

  ewd_list_goto_first(obj->bits);
  while ((bit = (Etox_Object_Bit) ewd_list_next(obj->bits)))
    {
      D_PRINT(" bit->type = %d\n", bit->type);
      switch (bit->type)
        {
        case ETOX_OBJECT_BIT_TYPE_STRING:
          string = (Etox_Object_String) bit->body;
          ewd_list_goto_first(string->style->bits);
          while ((style_bit = (Etox_Style_Bit) 
	          ewd_list_next(string->style->bits)))
	    {                                    
              ev_obj = evas_add_text(e->evas, string->font->name,
                                     string->font->size, string->str);
              D_PRINT(" Adding evas '%s'\n", string->str);
              evas_stack_above(e->evas, ev_obj, prev);
              prev = ev_obj;
              if (!e->evas_objects.list)
                {
                  e->evas_objects.list = ewd_list_new();
                  ewd_list_set_free_cb(e->evas_objects.list, NULL);
                }
	      ewd_list_append(e->evas_objects.list, ev_obj);

	      evas_move(e->evas, ev_obj,
		        ET_X_TO_EV(bit->x) + style_bit->x,
		        ET_Y_TO_EV(bit->y) + style_bit->y);
              
  	      switch (style_bit->type)                
	        {
	        case ETOX_STYLE_TYPE_FOREGROUND:
	        default:
	          if ((cb = _etox_color_get_bit(string->color, "fg")))
  		    evas_set_color(e->evas, ev_obj, cb->r, cb->g, cb->b,
		                   ((style_bit->a * e->a * cb->a) / 65025));
                  if (!bit->evas_objects_list.fg)
                    {
                      bit->evas_objects_list.fg = ewd_list_new();
                      ewd_list_set_free_cb(bit->evas_objects_list.fg, NULL);
                    }
                  ewd_list_append(bit->evas_objects_list.fg, ev_obj);
	          break;
	        case ETOX_STYLE_TYPE_SHADOW:
	          if ((cb = _etox_color_get_bit(string->color, "sh")))
		    evas_set_color(e->evas, ev_obj, cb->r, cb->g, cb->b,
		                   ((style_bit->a * e->a * cb->a) / 65025));
                  if (!bit->evas_objects_list.sh)
                    {
                      bit->evas_objects_list.sh = ewd_list_new();
                      ewd_list_set_free_cb(bit->evas_objects_list.sh, NULL);
                    }
                  ewd_list_append(bit->evas_objects_list.sh, ev_obj);
	          break;
	        case ETOX_STYLE_TYPE_OUTLINE:
	          if ((cb = _etox_color_get_bit(string->color, "ol")))
		  evas_set_color(e->evas, ev_obj, cb->r, cb->g, cb->b,
		                 ((style_bit->a * e->a * cb->a) / 65025));
                  if (!bit->evas_objects_list.ol)
                    {
                      bit->evas_objects_list.ol = ewd_list_new();
                      ewd_list_set_free_cb(bit->evas_objects_list.ol, NULL);
                    }
                  ewd_list_append(bit->evas_objects_list.ol, ev_obj);
	          break;
	        }
            }
          break;
        case ETOX_OBJECT_BIT_TYPE_NEWLINE:
        default:
          break;
        case ETOX_OBJECT_BIT_TYPE_TAB:
          tab = (Etox_Object_Tab) bit->body;
          ev_obj = evas_add_text(e->evas, tab->font->name,
                                 tab->font->size, "    ");
          D_PRINT(" Adding evas '%s'\n", "    ");
          if (!e->evas_objects.list)
            {
              e->evas_objects.list = ewd_list_new();
              ewd_list_set_free_cb(e->evas_objects.list, NULL);
            }
          ewd_list_append(e->evas_objects.list, ev_obj);
          if (!bit->evas_objects_list.fg)
            {
              bit->evas_objects_list.fg = ewd_list_new();
              ewd_list_set_free_cb(bit->evas_objects_list.fg, NULL);
            }
          ewd_list_append(bit->evas_objects_list.fg, ev_obj);
          evas_move(e->evas, ev_obj,
                    ET_X_TO_EV(bit->x), ET_Y_TO_EV(bit->y));          
          break;
	}         
    }                                                  
}

void
_etox_create_evas_objects(Etox e)
{
  Etox_Object et_obj;
  Etox_Object_Bit et_obj_bit;
  Evas_Object ev_obj;

  if (!e || !e->etox_objects.list || ewd_list_is_empty(e->etox_objects.list))
    return;

  D_PRINT("_etox_create_evas_objects():\n");

  if (e->evas_objects.list)
    { 
      ewd_list_goto_first(e->evas_objects.list);
      while ((ev_obj = (Evas_Object) ewd_list_next(e->evas_objects.list)))
        evas_del_object(e->evas, ev_obj);
      ewd_list_destroy(e->evas_objects.list);
      e->evas_objects.list = NULL;

      ewd_list_goto_first(e->etox_objects.list);
      while ((et_obj = (Etox_Object) ewd_list_next(e->etox_objects.list)))
        {
          if (!et_obj->bits)
            continue;
          ewd_list_goto_first(et_obj->bits);
          while ((et_obj_bit = (Etox_Object_Bit) ewd_list_next(et_obj->bits)))
            {
              if (et_obj_bit->evas_objects_list.fg)
                {
                  D_PRINT("detroying et_obj_bit->evas_objects_list.fg\n");
                  ewd_list_destroy(et_obj_bit->evas_objects_list.fg);
                  et_obj_bit->evas_objects_list.fg = NULL;
                }
              if (et_obj_bit->evas_objects_list.sh)
                {
                  D_PRINT("detroying et_obj_bit->evas_objects_list.sh\n");
                  ewd_list_destroy(et_obj_bit->evas_objects_list.sh);
                  et_obj_bit->evas_objects_list.sh = NULL;
                }
              if (et_obj_bit->evas_objects_list.ol)
                {
                  D_PRINT("detroying et_obj_bit->evas_objects_list.ol\n");
                  ewd_list_destroy(et_obj_bit->evas_objects_list.ol);
                  et_obj_bit->evas_objects_list.ol = NULL;
                }
            }
        }
    } 

  ewd_list_goto_first(e->etox_objects.list);
  while ((et_obj = (Etox_Object) ewd_list_next(e->etox_objects.list)))
    {
      /* little optimisation.. */
      if (!et_obj->bits)
        continue;
      D_PRINT("Creating evas objects.. (%p)\n", et_obj);
      __create_evas_objects(e, et_obj);
    }
}
