#include "Etox_private.h"
#include "Etox.h"

Etox_Align
_etox_bit_align_new(Etox_Align_Type v, Etox_Align_Type h)
{
  Etox_Align align;

  align = malloc(sizeof(struct _Etox_Align));
  align->v = v;
  align->h = h;

  return align;
}

void
_etox_bit_align_free(Etox_Align align)
{
  IF_FREE(align);
}

Etox_Callback
_etox_bit_callback_new()
{
  Etox_Callback cb;

  cb = malloc(sizeof(struct _Etox_Callback));
  /* FIXME: finish this.. */

  return cb;
}

void
_etox_bit_callback_free(Etox_Callback cb)
{
  IF_FREE(cb);
}

Etox_Font
_etox_bit_font_new(char *name, int size, Etox e)
{
  Etox_Font font;

  font = malloc(sizeof(struct _Etox_Font));
  if (name)
    font->name = strdup(name);
  else
    font->name = NULL;
  font->size = size;
  _etox_get_font_ascent_descent(e, font, &(font->ascent), &(font->descent));

  return font;
}

void
_etox_bit_font_free(Etox_Font font)
{
  IF_FREE(font->name);
  IF_FREE(font);
}

Etox_Text
_etox_bit_text_new(char *str)
{
  Etox_Text text;

  text = malloc(sizeof(struct _Etox_Text));
  if (str)
    text->str = strdup(str);
  else
    text->str = NULL;
  
  return text;
}

void
_etox_bit_text_free(Etox_Text text)
{
  IF_FREE(text->str);
  IF_FREE(text);
}

void
_etox_bit_free(Etox_Bit bit)
{
  if (!bit)
    return;

  switch (bit->type)
    {
    case ETOX_BIT_TYPE_ALIGN:
      _etox_bit_align_free((Etox_Align) bit->body);
      break;
    case ETOX_BIT_TYPE_CALLBACK:
      _etox_bit_callback_free((Etox_Callback) bit->body);
      break;
    case ETOX_BIT_TYPE_COLOR:
      return;
    case ETOX_BIT_TYPE_FONT:
      _etox_bit_font_free((Etox_Font) bit->body);
      break;
    case ETOX_BIT_TYPE_STYLE:
      return;
    case ETOX_BIT_TYPE_TEXT:
      _etox_bit_text_free((Etox_Text) bit->body);
      break;
    default:
      return;;
    }
  
  FREE(bit);
}
