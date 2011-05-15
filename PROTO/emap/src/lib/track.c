
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

Eina_List *emap_track_nodes_get(EMap_Track *track)
{
   if(!track) return ;

   return track->nodes;
}

double emap_track_distance_get(EMap_Track *track)
{
   Eina_List *l;
   EMap_Track_Node *node, *prev = NULL;
   double dist = 0;

   if(!track) return -1;

   EINA_LIST_FOREACH(track->nodes, l, node)
   {
      if(prev)
      {
         double ret = emap_track_nodes_distance_get(prev, node);
         if(ret < 0) return -1;
         dist += ret;
      }
      prev = node;
   }
   return dist;
}

//TODO: not working
//double emap_track_time_get(EMap_Track *track)
//{
//   if(!track) return -1;
//   if(!track->nodes) return -1;
//
//   EMap_Track_Node *start = eina_list_data_get(track->nodes);
//   EMap_Track_Node *end = eina_list_data_get(eina_list_last(track->nodes));
//
//   return difftime( emap_track_node_time_get(end), emap_track_node_time_get(start) );
//}
//
//double emap_track_average_speed_get(EMap_Track *track)
//{
//   if(!track) return -1;
//   return emap_track_distance_get(track) / emap_track_time_get(track) * 60;
//}




