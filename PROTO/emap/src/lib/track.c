
#include "emap_private.h"

EMap_Track *emap_track_new(EMap_Track_Type type)
{
   EMap_Track *track = calloc(1, sizeof(EMap_Track));
   track->type = type;
   return track;
}

void emap_track_free(EMap_Track *track)
{
   EMap_Track_Node *node;

   if(!track) return;

   switch(track->type)
   {
   case EMAP_TRACK_TYPE_GPX:
      if(track->gpx.file)
         eina_stringshare_del(track->gpx.file);
      break;
   }

   if(track->name)
      eina_stringshare_del(track->name);
   EINA_LIST_FREE(track->nodes, node)
   {
      emap_track_node_free(node);
   }

   free(track);
}


void emap_track_name_set(EMap_Track *track, const char *name)
{
   if(!track) return ;

   eina_stringshare_replace(&track->name, name);
}

const char *emap_track_name_get(EMap_Track *track)
{
   if(!track) return NULL;
   return track->name;
}

void emap_track_node_add(EMap_Track *track, EMap_Track_Node *node)
{
   if(!track || !node) return ;

   track->nodes = eina_list_append(track->nodes, node);
}
