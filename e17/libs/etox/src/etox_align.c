#include "Etox_private.h"
#include "Etox.h"

static Etox_Align_Type __get_horizontal_align_type(Etox_Object obj);
static void __align_horizontal(Etox_Object obj);
static void __align_vertical(Etox e, Etox_Object obj);

static Etox_Align_Type
__get_horizontal_align_type(Etox_Object obj)
{
  Etox_Align_Type align_type = ETOX_ALIGN_TYPE_NULL;
  Etox_Object_Bit obj_bit;

  if (!(obj_bit = (Etox_Object_Bit) ewd_list_next(obj->bits)))
    return align_type;

  switch (obj_bit->type)
    {
    case ETOX_OBJECT_BIT_TYPE_STRING:
      {
        Etox_Object_String string;

        string = (Etox_Object_String) obj_bit->body;
        align_type = string->align->h;
        D_PRINT("++ align for '%s' is %d\n", string->str, align_type);

        /* empty strings don't count! :) -redalb */
        if (!strlen(string->str))
          align_type = __get_horizontal_align_type(obj);
        break;
      }
    case ETOX_OBJECT_BIT_TYPE_TAB:
      {
        Etox_Object_Tab tab;

        tab = (Etox_Object_Tab) obj_bit->body;
        align_type = tab->align->h;
        D_PRINT("++ align for tab is %d\n", align_type);
        break;
      }
    default:
      D_PRINT("++ align == NULL (WRONG!)\n");
      align_type = __get_horizontal_align_type(obj);
      break;
    }

  return align_type;
}

static void
__align_horizontal(Etox_Object obj)
{
  Etox_Object_Bit obj_bit;
  double x, prev_w = 0.0;
  Etox_Align_Type align_type;

  ewd_list_goto_first(obj->bits);
  align_type = __get_horizontal_align_type(obj);
  switch (align_type)
    {
    case ETOX_ALIGN_TYPE_LEFT:
    default:
      x = obj->x;
      break;
    case ETOX_ALIGN_TYPE_CENTER:
      {
        double total_w = 0.0;

        ewd_list_goto_first(obj->bits);
        while ((obj_bit = (Etox_Object_Bit) ewd_list_next(obj->bits)))
          total_w += obj_bit->w;
        x = obj->x + ((obj->w - total_w) / 2); 
        break;
      }
    case ETOX_ALIGN_TYPE_RIGHT:
      {
        double total_w = 0.0;

        ewd_list_goto_first(obj->bits);
        while ((obj_bit = (Etox_Object_Bit) ewd_list_next(obj->bits)))
          total_w += obj_bit->w;
        x = obj->x + (obj->w - total_w);
        break;
      }
    }
  
  ewd_list_goto_first(obj->bits);
  while ((obj_bit = (Etox_Object_Bit) ewd_list_next(obj->bits)))
    {
      obj_bit->x = x + prev_w;
      prev_w += obj_bit->w;
    }
}

static void
__align_vertical(Etox e, Etox_Object obj)
{
  Etox_Object_Bit obj_bit;

  if (!e || !obj)
    return;

  ewd_list_goto_first(obj->bits);
  while ((obj_bit = (Etox_Object_Bit) ewd_list_next(obj->bits)))
    {
      switch (obj_bit->type)
        {
        case ETOX_OBJECT_BIT_TYPE_STRING:
          {
            Etox_Object_String string;

            string = (Etox_Object_String) obj_bit->body;
            switch (string->align->v)
              {
                case ETOX_ALIGN_TYPE_TOP:
                default:
                  obj_bit->y = obj->y;
                  break;
                case ETOX_ALIGN_TYPE_CENTER:
                  obj_bit->y = obj->y + ((e->etox_objects.h - obj_bit->h) / 2);
                  break;
                case ETOX_ALIGN_TYPE_BOTTOM:
                  obj_bit->y = obj->y + (e->etox_objects.h - obj_bit->h);
                  break;
              }
            break;
          }
        default:
          obj_bit->y = obj->y;
          break;
        } 
    }
}

void
_etox_align_etox_object(Etox e, Etox_Object obj)
{
  if (!e || !obj || !obj->bits)
    return;

  __align_horizontal(obj);
  __align_vertical(e, obj);
}

