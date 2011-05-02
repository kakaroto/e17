
#include "EMap.h"


struct emap_track_node
{
   double lon;
   double lat;
};


EMap_Track_Node *emap_track_node_new()
{
   EMap_Track_Node *node = calloc(1, sizeof(EMap_Track_Node));
   return node;
}

void emap_track_node_lon_set(EMap_Track_Node *node, double lon)
{
   node->lon = lon;
}

void emap_track_node_lat_set(EMap_Track_Node *node, double lat)
{
   node->lat = lat;
}
