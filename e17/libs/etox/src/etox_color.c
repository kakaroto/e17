#include "Etox_private.h"
#include "Etox.h"
#include <string.h>

Etox_Color_Bit 
_etox_color_get_bit(Etox_Color color, char *member) 
{ 
  Etox_Color_Bit cb;    
 
  if (!color) 
    return NULL; 
  if (!color->bits) 
    return NULL; 
 
  ewd_list_goto_first(color->bits); 
  while ((cb = (Etox_Color_Bit) ewd_list_next(color->bits))) 
    if (!strcmp(cb->name, member))  
      return cb; 
 
  return NULL; 
}

void
_etox_color_bit_free(Etox_Color_Bit bit)
{
  IF_FREE(bit->name);
  IF_FREE(bit);
}

Etox_Color
etox_color_new(void)
{
  Etox_Color color;

  color = malloc(sizeof(struct _Etox_Color));
  color->bits = NULL;
  return color;
}

void
etox_color_free(Etox_Color color)
{
  if (!color)
    return;

  ewd_list_destroy(color->bits);
  FREE(color);
}

void
etox_color_set_member(Etox_Color color, char *member, 
                      int r, int g, int b, int a)
{
  Etox_Color_Bit cb, tmp;

  if (!color || !member) 
    return;

  if (!color->bits)
    {
      color->bits = ewd_list_new();
      ewd_list_set_free_cb(color->bits, EWD_FREE_CB(_etox_color_bit_free));
    }

  cb = malloc(sizeof(struct _Etox_Color_Bit));
  cb->name = strdup(member);
  cb->r = r;
  cb->g = g;
  cb->b = b;
  cb->a = a;

  if ((tmp = _etox_color_get_bit(color, cb->name)))
    {
      ewd_list_goto(color->bits, tmp);
      ewd_list_remove(color->bits);
      FREE(tmp);
    }

  ewd_list_append(color->bits, cb);
}

int
etox_color_get_member(Etox_Color color, char *member, 
                      int *r, int *g, int *b, int *a)
{
  Etox_Color_Bit cb = NULL;

  if (!color || !member) 
    return 0;

  if (!color->bits)
    {
      color->bits = ewd_list_new();
      ewd_list_set_free_cb(color->bits, EWD_FREE_CB(_etox_color_bit_free));
    }

  if ((cb = _etox_color_get_bit(color, cb->name)))
    {
      *r = cb->r;
      *g = cb->g;
      *b = cb->b;
      *a = cb->a;
    }
  else
    {
      *r = 0;
      *g = 0;
      *b = 0;
      *a = 255;

      return 0;
    }

  return 1;
}
