#include "Etox_private.h"
#include "Etox.h"

void
_etox_part_add_bit(Etox_Part part, Etox_Bit bit)
{
  if (!part)
    return;
  if (!part->bits)
    {
      part->bits = ewd_list_new();
      ewd_list_set_free_cb(part->bits, NULL);
    }
  ewd_list_append(part->bits, bit);
}

Etox_Part
etox_part_new(void)
{
  Etox_Part part;

  part = (Etox_Part) malloc(sizeof(struct _Etox_Part));
  part->bits = NULL;
  part->prepended = NULL;
  part->appended = NULL;
  return part;
}

void
etox_part_free(Etox_Part part)
{
  if (!part)
    return;
  if (part->bits)
    ewd_list_destroy(part->bits);
  FREE(part);
}

void
etox_part_set_from_callback(Etox e, Etox_Part part, Etox_Callback callback)
{
  Etox_Bit et_bit;
  Etox_Callback_Bit cb_bit;

  if (!e || !e->bits || ewd_list_is_empty(e->bits))
    return;
  if (!part)
    return;
  if (!callback || !callback->bits || ewd_list_is_empty(callback->bits))
    return;

  /* add Etox_Bits, on which the callback applies, to the part */
  ewd_list_goto_first(e->bits);
  while ((et_bit = (Etox_Bit) ewd_list_next(e->bits)))
    if (et_bit->type == ETOX_BIT_TYPE_CALLBACK)
      break;
  /* were at the position of the callback.. */
  while ((et_bit = (Etox_Bit) ewd_list_next(e->bits)))
    {
      /* stop when ET_CALLBACK_END (body == NULL) appears.. */
      if ((et_bit->type == ETOX_BIT_TYPE_CALLBACK) && !et_bit->body)
	break;
      _etox_part_add_bit(part, et_bit);
    }
}

void
etox_part_prepend(Etox e, Etox_Part part, ...)
{
  Etox_Bit bit;
  Etox_Bit_Type type;
  va_list ap;

  if (!e || !e->bits || ewd_list_is_empty(e->bits) ||
      !part || !part->bits || ewd_list_is_empty(part->bits))
    return;

  bit = (Etox_Bit) ewd_list_goto_first(part->bits);
  if (!ewd_list_goto(e->bits, bit))
    return;

  if (!part->prepended)
    {
      part->prepended = ewd_list_new();
      ewd_list_set_free_cb(part->prepended, EWD_FREE_CB(_etox_bit_free));
    }

  va_start(ap, part);
  while ((type = va_arg(ap, Etox_Bit_Type)))
    {
      bit = malloc(sizeof(struct _Etox_Bit));
      bit->type = type;
      switch (bit->type)
	{
	case ETOX_BIT_TYPE_ALIGN:
	  {
	    Etox_Align_Type v, h;

	    D_PRINT("Setting an align..\n");
	    v = va_arg(ap, Etox_Align_Type);
	    h = va_arg(ap, Etox_Align_Type);
	    bit->body = _etox_bit_align_new(v, h);
	    D_PRINT("-> v=%d, h=%d\n", v, h);
	    break;
	  }
	case ETOX_BIT_TYPE_CALLBACK:
	  D_PRINT("Setting a callback..\n");
	  bit->body = va_arg(ap, Etox_Callback);
	  break;
	case ETOX_BIT_TYPE_COLOR:
	  D_PRINT("Setting a color..\n");
	  bit->body = va_arg(ap, Etox_Color);
	  break;
	case ETOX_BIT_TYPE_FONT:
	  {
	    char *name;
	    int size;

	    D_PRINT("Setting a font..\n");
	    name = va_arg(ap, char *);
	    size = va_arg(ap, int);
	    bit->body = _etox_bit_font_new(name, size, e);
	    D_PRINT("-> name=%s, size=%d\n", name, size);
	    break;
	  }
	case ETOX_BIT_TYPE_STYLE:
	  D_PRINT("Setting a style..\n");
	  bit->body = va_arg(ap, Etox_Style);
	  break;
	case ETOX_BIT_TYPE_TEXT:
	  D_PRINT("Setting a text..\n");
	  bit->body = _etox_bit_text_new(va_arg(ap, char *));
	  break;
	default:
	  D_PRINT("Setting (null)..\n");
	  bit->body = NULL;
	  break;
	}
      ewd_list_insert(e->bits, bit);
      ewd_list_next(e->bits);

      ewd_list_append(part->prepended, bit);
    }
  va_end(ap);

  e->etox_objects.dirty = 1;
  e->etox_objects.bits.dirty = 1;
  _etox_update(e);
}

void
etox_part_append(Etox e, Etox_Part part, ...)
{
  Etox_Bit bit;
  Etox_Bit_Type type;
  va_list ap;

  if (!e || !e->bits || ewd_list_is_empty(e->bits) ||
      !part || !part->bits || ewd_list_is_empty(part->bits))
    return;

  bit = (Etox_Bit) ewd_list_goto_last(part->bits);
  if (!ewd_list_goto(e->bits, bit))
    return;
  ewd_list_next(e->bits);

  if (!part->appended)
    {
      part->appended = ewd_list_new();
      ewd_list_set_free_cb(part->appended, EWD_FREE_CB(_etox_bit_free));
    }

  va_start(ap, part);
  while ((type = va_arg(ap, Etox_Bit_Type)))
    {
      bit = malloc(sizeof(struct _Etox_Bit));
      bit->type = type;
      switch (bit->type)
	{
	case ETOX_BIT_TYPE_ALIGN:
	  {
	    Etox_Align_Type v, h;

	    D_PRINT("Setting an align..\n");
	    v = va_arg(ap, Etox_Align_Type);
	    h = va_arg(ap, Etox_Align_Type);
	    bit->body = _etox_bit_align_new(v, h);
	    D_PRINT("-> v=%d, h=%d\n", v, h);
	    break;
	  }
	case ETOX_BIT_TYPE_CALLBACK:
	  D_PRINT("Setting a callback..\n");
	  bit->body = va_arg(ap, Etox_Callback);
	  break;
	case ETOX_BIT_TYPE_COLOR:
	  D_PRINT("Setting a color..\n");
	  bit->body = va_arg(ap, Etox_Color);
	  break;
	case ETOX_BIT_TYPE_FONT:
	  {
	    char *name;
	    int size;

	    D_PRINT("Setting a font..\n");
	    name = va_arg(ap, char *);
	    size = va_arg(ap, int);
	    bit->body = _etox_bit_font_new(name, size, e);
	    D_PRINT("-> name=%s, size=%d\n", name, size);
	    break;
	  }
	case ETOX_BIT_TYPE_STYLE:
	  D_PRINT("Setting a style..\n");
	  bit->body = va_arg(ap, Etox_Style);
	  break;
	case ETOX_BIT_TYPE_TEXT:
	  D_PRINT("Setting a text..\n");
	  bit->body = _etox_bit_text_new(va_arg(ap, char *));
	  break;
	default:
	  D_PRINT("Setting (null)..\n");
	  bit->body = NULL;
	}
      ewd_list_insert(e->bits, bit);
      ewd_list_next(e->bits);

      ewd_list_append(part->appended, bit);
    }
  va_end(ap);

  e->etox_objects.dirty = 1;
  e->etox_objects.bits.dirty = 1;
  _etox_update(e);
}

void
etox_part_remove_changes(Etox e, Etox_Part part)
{
  Etox_Bit bit;

  if (!e || !e->bits || ewd_list_is_empty(e->bits) ||
      !part || !part->bits || ewd_list_is_empty(part->bits))
    return;

  if (part->prepended)
    {
      ewd_list_goto_first(part->prepended);
      while ((bit = (Etox_Bit) ewd_list_next(part->prepended)))
	if (ewd_list_goto(e->bits, bit))
	  ewd_list_remove(e->bits);
      ewd_list_destroy(part->prepended);
      part->prepended = NULL;
    }
  if (part->appended)
    {
      ewd_list_goto_first(part->appended);
      while ((bit = (Etox_Bit) ewd_list_next(part->appended)))
	if (ewd_list_goto(e->bits, bit))
	  ewd_list_remove(e->bits);
      ewd_list_destroy(part->appended);
      part->appended = NULL;
    }

  e->etox_objects.dirty = 1;
  e->etox_objects.bits.dirty = 1;
  _etox_update(e);
}
