#include "Etox_private.h"
#include "Etox.h"
#include <string.h>

Etox_Color_Bit 
_etox_color_get_bit(Etox_Color color, char *member) 
{ 
  Evas_List l; 
  Etox_Color_Bit cb;    
 
  if (!color) 
    return NULL; 
  if (!color->bit_list) 
    return NULL; 
 
  for (l = color->bit_list; l; l = l->next) 
    if (cb = l->data) 
      if (!strcmp(cb->name, member))  
	return cb; 
 
  return NULL; 
}


Etox_Color
etox_color_new(void)
{
  Etox_Color color;

  color = malloc(sizeof(struct _Etox_Color));
  color->bit_list = NULL;
  return color;
}

void
etox_color_free(Etox_Color color)
{
  Evas_List l;

  if (!color)
    return;

  for (l = color->bit_list; l; l = l->next)
    if (l->data)
      free(l->data);

  if (color->bit_list)
    evas_list_free(color->bit_list);
  free(color);

  color = NULL;
}

void
etox_color_set_member(Etox_Color color, char *member, 
                      int r, int g, int b, int a)
{
  Etox_Color_Bit cb, tmp;

  if (!color || !member) 
    return;

  cb = malloc(sizeof(struct _Etox_Color_Bit));
  cb->name = strdup(member);
  cb->r = r;
  cb->g = g;
  cb->b = b;
  cb->a = a;

  if (tmp = _etox_color_get_bit(color, cb->name))
    {
      color->bit_list = evas_list_remove(color->bit_list, tmp);
      free(tmp);
    }
  color->bit_list = evas_list_append(color->bit_list, cb);
}

