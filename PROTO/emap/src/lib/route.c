
#include "emap_private.h"

int LOG_DOMAIN;

EMap_Route *emap_route_new(EMap_Route_Type type)
{
   EMap_Route *route = calloc(1, sizeof(EMap_Route));
   route->type = type;
   return route;
}

void emap_route_free(EMap_Route *route)
{
   EMap_Route_Node *node;

   if(!route) return;

   switch(route->type)
   {
   case EMAP_TRACK_TYPE_GPX:
      if(route->gpx.file)
         eina_stringshare_del(route->gpx.file);
      break;
   }

   if(route->name)
      eina_stringshare_del(route->name);
   EINA_LIST_FREE(route->nodes, node)
   {
      emap_route_node_free(node);
   }

   free(route);
}


void emap_route_name_set(EMap_Route *route, const char *name)
{
   if(!route) return ;

   eina_stringshare_replace(&route->name, name);
}

const char *emap_route_name_get(EMap_Route *route)
{
   if(!route) return NULL;
   return route->name;
}

void emap_route_node_add(EMap_Route *route, EMap_Route_Node *node)
{
   if(!route || !node) return ;

   route->nodes = eina_list_append(route->nodes, node);
}

Eina_List *emap_route_nodes_get(EMap_Route *route)
{
   if(!route) return NULL;

   return route->nodes;
}

double emap_route_distance_get(EMap_Route *route)
{
   Eina_List *l;
   EMap_Route_Node *node, *prev = NULL;
   double dist = 0;

   if(!route) return -1;

   EINA_LIST_FOREACH(route->nodes, l, node)
   {
      if(prev)
      {
         double ret = emap_route_nodes_distance_get(prev, node);
         if(ret < 0) return -1;
         dist += ret;
      }
      prev = node;
   }
   return dist;
}

//TODO: not working
//double emap_route_time_get(EMap_Route *route)
//{
//   if(!route) return -1;
//   if(!route->nodes) return -1;
//
//   EMap_Route_Node *start = eina_list_data_get(route->nodes);
//   EMap_Route_Node *end = eina_list_data_get(eina_list_last(route->nodes));
//
//   return difftime( emap_route_node_time_get(end), emap_route_node_time_get(start) );
//}
//
//double emap_route_average_speed_get(EMap_Route *route)
//{
//   if(!route) return -1;
//   return emap_route_distance_get(route) / emap_route_time_get(route) * 60;
//}




