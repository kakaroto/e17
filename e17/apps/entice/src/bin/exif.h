#ifndef ENTICE_EXIF_H
#define ENTICE_EXIF_H

#include<Evas.h>

struct _Entice_Exif
{
   char *part;
   int tag;
   int lvl;
};
typedef struct _Entice_Exif Entice_Exif;

Evas_Hash *entice_exif_edje_init(Evas_Object * edje);

#endif
