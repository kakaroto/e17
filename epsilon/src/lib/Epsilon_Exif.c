#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "exiftags/jpeg.h"
#include "exiftags/exif.h"

#include "Epsilon.h"

int quiet = 1;
static void
_epsilon_exif_info_props_get (struct exifprop *list, unsigned short lvl,
			      int pas);
int
epsilon_info_exif_props_as_int_get (Epsilon_Info * ei, unsigned short lvl,
				    long prop)
{
  const char *n;
  struct exifprop *list;
  int pas = TRUE;

  if (!ei || !ei->eei)
    return (-1);
  list = ((struct exiftags *) ei->eei)->props;
  while (list)
    {

      /* Take care of point-and-shoot values. */

      if (list->lvl == ED_PAS)
	list->lvl = pas ? ED_CAM : ED_IMG;

      /* For now, just treat overridden & bad values as verbose. */

      if (list->lvl == ED_OVR || list->lvl == ED_BAD)
	list->lvl = ED_VRB;
      if (list->lvl == lvl)
	{
	  n = list->descr ? list->descr : list->name;
	  if (list->tag == prop)
	    {
	      return (list->value);
	    }
	}
      list = list->next;
    }
  return (-1);
}

const char *
epsilon_info_exif_props_as_string_get (Epsilon_Info * ei, unsigned short lvl,
				       long prop)
{
  const char *n;
  struct exifprop *list;
  int pas = TRUE;

  if (!ei || !ei->eei)
    return (NULL);
  list = ((struct exiftags *) ei->eei)->props;
  while (list)
    {

      /* Take care of point-and-shoot values. */

      if (list->lvl == ED_PAS)
	list->lvl = pas ? ED_CAM : ED_IMG;

      /* For now, just treat overridden & bad values as verbose. */

      if (list->lvl == ED_OVR || list->lvl == ED_BAD)
	list->lvl = ED_VRB;

      if (list->lvl == lvl)
	{
	  n = list->descr ? list->descr : list->name;
	  if (list->tag == prop)
	    {
	      if (list->str)
		return ((const char *) list->str);
	      else
		return (NULL);

	    }
	}
      list = list->next;
    }
  return (NULL);
}

void
epsilon_info_exif_props_print (Epsilon_Info * ei)
{
  if (ei && ei->eei)
    _epsilon_exif_info_props_get (((struct exiftags *) ei->eei)->props,
				  ED_IMG, TRUE);
}

static void
_epsilon_exif_info_props_get (struct exifprop *list, unsigned short lvl,
			      int pas)
{
  const char *n;

  while (list)
    {

      /* Take care of point-and-shoot values. */

      if (list->lvl == ED_PAS)
	list->lvl = pas ? ED_CAM : ED_IMG;

      /* For now, just treat overridden & bad values as verbose. */

      if (list->lvl == ED_OVR || list->lvl == ED_BAD)
	list->lvl = ED_VRB;
      if (list->lvl == lvl)
	{
	  n = list->descr ? list->descr : list->name;
	  switch (list->tag)
	    {
	    case EXIF_T_ORIENT:
	    case EXIF_T_FNUMBER:
	    case EXIF_T_EXPOSURE:
	    case EXIF_T_ISOSPEED:
	    case EXIF_T_SHUTTER:
	    case EXIF_T_FOCALLEN:
	    case EXIF_T_FOCALLEN35:
	    case EXIF_T_FLASH:
	    case EXIF_T_WHITEBAL:
	    case EXIF_T_EXPMODE:
	    case 0xa002:
	    case 0xa003:
	    case 0x010f:
	      if (list->str)
		printf ("%s%s%s\n", n, " : ", list->str);
	      else
		printf ("%s%s%d\n", n, " : ", list->value);
	      break;
	    default:
#if 0
	      printf ("%16x\n", list->tag);
#endif
	      break;
	    }
	}
      list = list->next;
    }
}

static Epsilon_Exif_Info *
epsilon_read_exif_data (FILE * fp, int dumplvl, int pas)
{
  int mark, gotapp1, first;
  unsigned int len, rlen;
  unsigned char *exifbuf;
  struct exiftags *t = NULL;

  gotapp1 = FALSE;
  first = 0;
  exifbuf = NULL;

  while (jpegscan (fp, &mark, &len, !(first++)))
    {

      if (mark != JPEG_M_APP1)
	{
	  if (fseek (fp, len, SEEK_CUR))
	    exifdie ((const char *) strerror (errno));
	  continue;
	}

      exifbuf = (unsigned char *) malloc (len);
      if (!exifbuf)
	exifdie ((const char *) strerror (errno));

      rlen = fread (exifbuf, 1, len, fp);
      if (rlen != len)
	{
	  exifwarn ("error reading JPEG (length mismatch)");
	  free (exifbuf);
	  return (NULL);
	}

      t = exifparse (exifbuf, len);

      if (t && t->props)
	{
	  gotapp1 = TRUE;

#if 0
	  if (dumplvl & ED_CAM)
	    _epsilon_exif_info_props_get (t->props, ED_CAM, pas);
	  if (dumplvl & ED_IMG)
	    _epsilon_exif_info_props_get (t->props, ED_IMG, pas);
	  if (dumplvl & ED_VRB)
	    _epsilon_exif_info_props_get (t->props, ED_VRB, pas);
	  if (dumplvl & ED_UNK)
	    _epsilon_exif_info_props_get (t->props, ED_UNK, pas);
#endif
	}
      free (exifbuf);
    }

  if (!gotapp1)
    {
      exifwarn ("couldn't find Exif data");
      if (t)
	exiffree (t);
      return (NULL);
    }
  return ((Epsilon_Exif_Info *) t);
}

Epsilon_Exif_Info *
epsilon_exif_info_get (Epsilon * e)
{
  FILE *fp = NULL;
  char *mode = "rb";
  Epsilon_Exif_Info *eei = NULL;

  if ((fp = fopen (epsilon_file_get (e), mode)) == NULL)
    {
      exifwarn2 (strerror (errno), epsilon_file_get (e));
    }
  else
    {
      eei = epsilon_read_exif_data (fp, ED_CAM | ED_IMG, TRUE);
      fclose (fp);
    }
  return (eei);
}

void
epsilon_exif_info_free (Epsilon_Exif_Info * eei)
{
  if (eei)
    {
      exiffree (eei);
    }
}

void
epsilon_exif_info_direction_get (Epsilon * e, Epsilon_Exif_Info * eei)
{
  if (e && eei)
    {

    }
}
