#include "Etox_private.h"
#include "Etox.h"

void
etox_set_evas(Etox e, Evas evas)
{
  if (!e) 
    return;

  e->evas = evas;

  _etox_rebuild(e);
}

void
etox_set_name(Etox e, char *name)
{
  if (!e) 
    return;

  if (e->name)
    free(e->name);

  if (name)
    e->name = strdup(name);
  else
    e->name = NULL;
}

void
etox_set_font(Etox e, char *font, int size)
{
  if (!e) 
    return;

  if (font)
    {
      IF_FREE(e->def.font->name);
      e->def.font->name = strdup(font);
    }
  else
    e->def.font->name = NULL;
  e->def.font->size = size;

  _etox_get_font_ascent_descent(e, e->def.font,
                                &(e->def.font->ascent),
                                &(e->def.font->descent));

  _etox_rebuild(e);
}

void
etox_set_style(Etox e, Etox_Style style)
{
  if (!e) 
    return;

  if (e->def.style)
    etox_style_free(e->def.style);

  e->def.style = style;

  _etox_rebuild(e);
}

void
etox_set_align(Etox e, Etox_Align_Type h_align, Etox_Align_Type v_align)
{
  if (!e) 
    return;

  e->def.align->h = h_align;
  e->def.align->v = v_align;

  _etox_rebuild(e);
}

void 
etox_set_layer(Etox e, int layer) 
{ 
  if (!e) 
    return; 
 
  e->layer = layer;

  _etox_refresh(e);
}

void
etox_set_padding(Etox e, double padding)
{
  if (!e) 
    return;

  e->padding = padding;

  _etox_rebuild(e);
}

void 
etox_set_text(Etox e, ...) 
{ 
  Etox_Bit bit;
  Etox_Bit_Type type;
  va_list ap;

  if (!e) 
    return;

  if (!e->bits)
    {
      e->bits = ewd_list_new();
      ewd_list_set_free_cb(e->bits, EWD_FREE_CB(_etox_bit_free));
    }
  else
    ewd_list_clear(e->bits);

  va_start(ap, e);

  while ((type = va_arg(ap, Etox_Bit_Type)))
    {
      bit = malloc(sizeof(struct _Etox_Bit));
      bit->type = type;

      switch (type)
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
	  bit->body = _etox_bit_callback_new(); 
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

      ewd_list_append(e->bits, bit);
    }

  va_end(ap);

  _etox_rebuild(e); 
}

void
etox_set_color(Etox e, Etox_Color color)
{
  if (!e) 
    return;

  if (e->def.color)
    etox_color_free(e->def.color);

  e->def.color = color;

  _etox_rebuild(e);                                                       
}

void
etox_set_alpha(Etox e, int alpha)
{
  if (!e) 
    return;

  e->a = alpha;

  _etox_rebuild(e);
/*
  _etox_create_evas_objects(e);
  _etox_refresh(e);
*/
}

void   
etox_set_clip(Etox e, Evas_Object clip)
{
  if (!e) 
    return; 
 
  e->clip = clip;

  _etox_refresh(e);
}

void   
etox_unset_clip(Etox e)
{
  if (!e) 
    return;

  e->clip = NULL;

  _etox_refresh(e);
}
