#include <stdlib.h>
#include <string.h>

#include "Epdf.h"


Epdf_Font_Info *
epdf_font_info_new (const char *font_name, const char *font_path, unsigned char is_embedded, unsigned char is_subset, Epdf_Font_Info_Type type)
{
  return NULL;
}

void
epdf_font_info_delete (Epdf_Font_Info *fi)
{
  return;
}

const char *
epdf_font_info_font_name_get (const Epdf_Font_Info *fi)
{
  return NULL;
}

const char *
epdf_font_info_font_path_get (const Epdf_Font_Info *fi)
{
  return NULL;
}

unsigned char
epdf_font_info_is_embedded_get (const Epdf_Font_Info *fi)
{
  return 0;
}

unsigned char
epdf_font_info_is_subset_get (const Epdf_Font_Info *fi)
{
  return 0;
}

Epdf_Font_Info_Type
epdf_font_info_type_get (const Epdf_Font_Info *fi)
{
  return EPDF_FONT_INFO_UNKNOWN;
}

const char *
epdf_font_info_type_name_get (const Epdf_Font_Info *fi)
{
  return NULL;
}
