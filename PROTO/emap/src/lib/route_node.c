
#include "emap_private.h"
#include <math.h>

struct emap_route_node
{
   unsigned int idx;

   double lon;
   double lat;
   double elevation;
   time_t time;
};


EMap_Route_Node *emap_route_node_new(int idx)
{
   EMap_Route_Node *node = calloc(1, sizeof(EMap_Route_Node));
   node->idx = idx;

   return node;
}

void emap_route_node_free(EMap_Route_Node *node)
{
   if(!node) return ;

   free(node);
}

void emap_route_node_lon_set(EMap_Route_Node *node, double lon)
{
   if(!node) return;
   node->lon = lon;
}

double emap_route_node_lon_get(EMap_Route_Node *node)
{
   if(!node) return 0;
   return node->lon;
}

void emap_route_node_lat_set(EMap_Route_Node *node, double lat)
{
   if(!node) return;
   node->lat = lat;
}

double emap_route_node_lat_get(EMap_Route_Node *node)
{
   if(!node) return 0;
   return node->lat;
}

void emap_route_node_elevation_set(EMap_Route_Node *node, double elevation)
{
   if(!node) return;
   node->elevation = elevation;
}

double emap_route_node_elevation_get(EMap_Route_Node *node)
{
   if(!node) return 0;
   return node->elevation;
}


void emap_route_node_time_set(EMap_Route_Node *node, time_t time)
{
   if(!node) return;
   node->time = time;
}

time_t emap_route_node_time_get(EMap_Route_Node *node)
{
   if(!node) return 0;
   return node->time;
}

static double radian(double d)
{
   return 3.14 * d / 180;
}

double emap_route_nodes_distance_get(EMap_Route_Node *A, EMap_Route_Node *B)
{
   if(!A || !B) return -1;

   int r = 6371; //km
   double dist = r * (acos(
            sin(radian(B->lat)) * sin(radian(A->lat)) + cos(radian(B->lon) - radian(A->lon))
            * cos(radian(B->lat)) * cos(radian(A->lat))));
   if(dist < 0)
      dist = -dist;
   return dist;
}

