#include "exif.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Evas.h>
#include <Edje.h>
#include <Epsilon.h>

Evas_Hash *
entice_exif_edje_init(Evas_Object * edje)
{
   const char *str = NULL;
   int i = 0, count = 0, j = 0;
   Entice_Exif *exif = NULL;
   Evas_Hash *result = NULL;
   char *exifstr[] = {
      "ENTICE_EXIF_CAMERA_MAKE",
      "ENTICE_EXIF_CAMERA_MODEL",
      "ENTICE_EXIF_PHOTO_TAKEN_ON",
      "ENTICE_EXIF_FOCAL_LENGTH",
      "ENTICE_EXIF_EXPOSURE_TIME",
      "ENTICE_EXIF_ISO_SPEED",
      "ENTICE_EXIF_APERTURE",
      "ENTICE_EXIF_WHITEBALANCE",
      "ENTICE_EXIF_FLASH"
   };
   int exifkey[] = {
      0x010f, EPSILON_ED_CAM,
      0x0110, EPSILON_ED_CAM,
      0x0132, EPSILON_ED_IMG,
      0x920a, EPSILON_ED_IMG,
      0x829a, EPSILON_ED_IMG,
      0x8827, EPSILON_ED_IMG,
      0x9202, EPSILON_ED_IMG,
      0x0004, EPSILON_ED_IMG,
      0x9209, EPSILON_ED_IMG
   };
   count = sizeof(exifstr) / sizeof(char *);

   for (i = 0, j = 0; j < count; j++)
   {
      if ((str = edje_object_data_get(edje, exifstr[j])))
      {
         if (edje_object_part_exists(edje, str))
         {
#if 0
            fprintf(stderr, "EXIF: FOUND: %s : %s : 0x%04x\n", exifstr[i],
                    str, exifkey[i]);
#endif
            if (strlen(str) > 0)
            {
               exif = (Entice_Exif *) malloc(sizeof(Entice_Exif));
               memset(exif, 0, sizeof(exif));
               exif->part = (char *) str;
               exif->tag = exifkey[i];
               exif->lvl = exifkey[i + 1];
               result = evas_hash_add(result, exifstr[j], exif);
            }
         }
         else
         {
            fprintf(stderr, "EXIF TEXT PART NOT FOUND: %s\n", str);
            fprintf(stderr, "PLEASE FIX YOUR THEME\n");
         }
      }
      else
      {
#if 0
         fprintf(stderr, "EXIF: NOT FOUND: %s\n", exifstr[i]);
#endif
      }
      i += 2;
   }
   return (result);
}
