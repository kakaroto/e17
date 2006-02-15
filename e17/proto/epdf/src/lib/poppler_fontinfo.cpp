#include <stdlib.h>
#include <string.h>

#include "poppler_enum.h"
#include "poppler_private.h"
#include "poppler_fontinfo.h"


Evas_Poppler_Font_Info *
evas_poppler_font_info_new (const char *font_name, unsigned char is_embedded, unsigned char is_subset, Evas_Poppler_Font_Info_Type type)
{
  Evas_Poppler_Font_Info *fi;

  fi = (Evas_Poppler_Font_Info *)malloc (sizeof (Evas_Poppler_Font_Info));
  if (!fi)
    return NULL;

  fi->font_name = strdup (font_name);
  fi->is_embedded = is_embedded;
  fi->is_subset = is_subset;
  fi->type = type;

  return fi;
}

void
evas_poppler_font_info_delete (Evas_Poppler_Font_Info *fi)
{
  if (!fi)
    return;

  if (fi->font_name)
    free (fi->font_name);

  free(fi);
}

const char *
evas_poppler_font_info_font_name_get (Evas_Poppler_Font_Info *fi)
{
  if (!fi)
    return NULL;

  return (const char *)fi->font_name;
}

unsigned char
evas_poppler_font_info_is_embedded_get (Evas_Poppler_Font_Info *fi)
{
  if (!fi)
    return 0;

  return fi->is_embedded;
}

unsigned char
evas_poppler_font_info_is_subset_get (Evas_Poppler_Font_Info *fi)
{
  if (!fi)
    return 0;

  return fi->is_subset;
}

Evas_Poppler_Font_Info_Type
evas_poppler_font_info_type_get (Evas_Poppler_Font_Info *fi)
{
  if (!fi)
    return EVAS_POPPLER_FONT_INFO_UNKNOWN;

  return fi->type;
}

const char *
evas_poppler_font_info_type_name_get (Evas_Poppler_Font_Info *fi)
{
  const char *type_name;
  if (!fi)
    return NULL;

  switch (fi->type)
    {
    case EVAS_POPPLER_FONT_INFO_UNKNOWN:
      type_name = "Type unknown";
      break;
    case EVAS_POPPLER_FONT_INFO_TYPE1:
      type_name = "Type 1";
      break;
    case EVAS_POPPLER_FONT_INFO_TYPE1C:
      type_name = "Type 1C";
      break;
    case EVAS_POPPLER_FONT_INFO_TYPE3:
      type_name = "Type 3";
      break;
    case EVAS_POPPLER_FONT_INFO_TRUETYPE:
      type_name = "TrueType";
      break;
    case EVAS_POPPLER_FONT_INFO_CID_TYPE0:
      type_name = "CID Type 0";
      break;
    case EVAS_POPPLER_FONT_INFO_CID_TYPE0C:
      type_name = "CID Type 0C";
      break;
    case EVAS_POPPLER_FONT_INFO_CID_TRUETYPE:
      type_name = "CID TrueType";
      break;
    default:
      type_name = "Type unknown";
      break;
    }

  return (type_name);
}
