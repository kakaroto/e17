#ifndef _EPSILON_PLUGIN_H_
#define _EPSILON_PLUGIN_H_

#include "Imlib2.h"
#include "Ecore_Data.h"

typedef struct _Epsilon_Plugin Epsilon_Plugin;

struct _Epsilon_Plugin
{
  Ecore_List* mime_types;
  Imlib_Image (*epsilon_generate_thumb)(Epsilon*);
};

#endif /* _EPSILON_PLUGIN_H_ */
