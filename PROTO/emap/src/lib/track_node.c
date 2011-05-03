
#include "emap_private.h"

struct emap_track_node
{
   unsigned int idx;

   double lon;
   double lat;
};


EMap_Track_Node *emap_track_node_new(int idx)
{
   EMap_Track_Node *node = calloc(1, sizeof(EMap_Track_Node));
   node->idx = idx;

   return node;
}

void emap_track_node_free(EMap_Track_Node *node)
{
   if(!node) return ;

   free(node);
}

void emap_track_node_lon_set(EMap_Track_Node *node, double lon)
{
   if(!node) return;
   node->lon = lon;
}

void emap_track_node_lat_set(EMap_Track_Node *node, double lat)
{
   if(!node) return;
   node->lat = lat;
}
