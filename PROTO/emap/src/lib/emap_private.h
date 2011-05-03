#ifndef EMAP_PRIVATE_H_
#define EMAP_PRIVATE_H_

#include "EMap.h"

struct emap_track
{
   EMap_Track_Type type;

   const char *name;
   Eina_List *nodes; //list of EMap_Track_Node*

   union {
      const char *file;

      Eina_Bool xml_is_name;
   } gpx;
};


#endif /* EMAP_PRIVATE_H_ */
