#include <stdlib.h>
#include <string.h>

#include "Epdf.h"
#include "epdf_private.h"


Epdf_Font_Info *
epdf_font_info_new (const char *font_name, const char *font_path, unsigned char is_embedded, unsigned char is_subset, Epdf_Font_Info_Type type)
{
  Epdf_Font_Info *fi;

  fi = (Epdf_Font_Info *)malloc (sizeof (Epdf_Font_Info));
  memset (fi, 0, sizeof (Epdf_Font_Info));
  if (!fi)
    return NULL;

  if (font_name)
    fi->font_name = strdup (font_name);
  if (font_path)
    fi->font_path = strdup (font_path);
  fi->is_embedded = is_embedded;
  fi->is_subset = is_subset;
  fi->type = type;

  return fi;
}

void
epdf_font_info_delete (Epdf_Font_Info *fi)
{
  if (!fi)
    return;

  if (fi->font_name)
    free (fi->font_name);

  if (fi->font_path)
    free (fi->font_path);

  free(fi);
}

const char *
epdf_font_info_font_name_get (const Epdf_Font_Info *fi)
{
  if (!fi)
    return NULL;

  return (const char *)fi->font_name;
}

const char *
epdf_font_info_font_path_get (const Epdf_Font_Info *fi)
{
  if (!fi)
    return NULL;

  return (const char *)fi->font_path;
}

unsigned char
epdf_font_info_is_embedded_get (const Epdf_Font_Info *fi)
{
  if (!fi)
    return 0;

  return fi->is_embedded;
}

unsigned char
epdf_font_info_is_subset_get (const Epdf_Font_Info *fi)
{
  if (!fi)
    return 0;

  return fi->is_subset;
}

Epdf_Font_Info_Type
epdf_font_info_type_get (const Epdf_Font_Info *fi)
{
  if (!fi)
    return EPDF_FONT_INFO_UNKNOWN;

  return fi->type;
}

const char *
epdf_font_info_type_name_get (const Epdf_Font_Info *fi)
{
  const char *type_name;

  if (!fi)
    return NULL;

  switch (fi->type) {
  case EPDF_FONT_INFO_UNKNOWN:
    type_name = "Type unknown";
    break;
  case EPDF_FONT_INFO_TYPE1:
    type_name = "Type 1";
    break;
  case EPDF_FONT_INFO_TYPE1C:
    type_name = "Type 1C";
    break;
  case EPDF_FONT_INFO_TYPE3:
    type_name = "Type 3";
    break;
  case EPDF_FONT_INFO_TRUETYPE:
    type_name = "TrueType";
    break;
  case EPDF_FONT_INFO_CID_TYPE0:
    type_name = "CID Type 0";
    break;
  case EPDF_FONT_INFO_CID_TYPE0C:
    type_name = "CID Type 0C";
    break;
  case EPDF_FONT_INFO_CID_TRUETYPE:
    type_name = "CID TrueType";
    break;
  default:
    type_name = "Type unknown";
    break;
  }

  return (type_name);
}
