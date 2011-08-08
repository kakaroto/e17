/*
 * EMap.h
 *
 *  Created on: May 2, 2011
 *      Author: watchwolf
 */

#ifndef EMAP_H_
#define EMAP_H_

#include <Eina.h>

// TODO: watchwolf, please fix this. this is temporary code for fixing elm build error.
#define EMAP_LON_MAX 180.0
#define EMAP_LON_MIN -180.0
#define EMAP_LAT_MAX 90.0
#define EMAP_LAT_MIN -90.0

typedef struct emap_route EMap_Route;
typedef struct emap_route_node EMap_Route_Node;

typedef enum emap_route_type
{
   EMAP_TRACK_TYPE_GPX
} EMap_Route_Type;

EAPI int               emap_init();
EAPI int               emap_shutdown();

EAPI EMap_Route       *emap_route_new();
EAPI void              emap_route_node_add(EMap_Route *route, EMap_Route_Node *node);
EAPI void              emap_route_free(EMap_Route *route);
EAPI void              emap_route_name_set(EMap_Route *route, const char *name);
EAPI const char       *emap_route_name_get(EMap_Route *route);
EAPI Eina_List        *emap_route_nodes_get(EMap_Route *route);
EAPI double            emap_route_distance_get(EMap_Route *route);
//EAPI double            emap_route_time_get(EMap_Route *route);
//EAPI double            emap_route_average_speed_get(EMap_Route *route);

EAPI EMap_Route_Node  *emap_route_node_new();
EAPI void              emap_route_node_lon_set(EMap_Route_Node *node, double lon);
EAPI void              emap_route_node_lat_set(EMap_Route_Node *node, double lat);
EAPI double            emap_route_node_lon_get(EMap_Route_Node *node);
EAPI double            emap_route_node_lat_get(EMap_Route_Node *node);
EAPI void              emap_route_node_elevation_set(EMap_Route_Node *node, double elevation);
EAPI double            emap_route_node_elevation_get(EMap_Route_Node *node);
EAPI void              emap_route_node_time_set(EMap_Route_Node *node, time_t time);
EAPI time_t            emap_route_node_time_get(EMap_Route_Node *node);
EAPI double            emap_route_nodes_distance_get(EMap_Route_Node *A, EMap_Route_Node *B);

EAPI void              emap_route_node_free(EMap_Route_Node *node);

EAPI EMap_Route *      emap_route_gpx_new(const char *file);

#endif /* EMAP_H_ */
