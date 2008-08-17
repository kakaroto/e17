#ifndef _EPSILON_PLUGIN_H_
#define _EPSILON_PLUGIN_H_

#include "Ecore_Data.h"

typedef struct _Epsilon_Image Epsilon_Image;
typedef struct _Epsilon_Plugin Epsilon_Plugin;

struct _Epsilon_Image
{
   int w, h;
   int alpha;
   void *data;
};

struct _Epsilon_Plugin
{
  Ecore_List* mime_types;
  Epsilon_Image *(*epsilon_generate_thumb)(Epsilon*);
};

#endif /* _EPSILON_PLUGIN_H_ */
