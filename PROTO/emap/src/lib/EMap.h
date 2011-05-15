/*
 * EMap.h
 *
 *  Created on: May 2, 2011
 *      Author: watchwolf
 */

#ifndef EMAP_H_
#define EMAP_H_

#include <Eina.h>

typedef enum emap_track_type EMap_Track_Type;
typedef struct emap_track EMap_Track;
typedef struct emap_track_node EMap_Track_Node;


enum emap_track_type
{
   EMAP_TRACK_TYPE_GPX
};


EAPI 	int 			emap_init();
EAPI 	int 			emap_shutdown();

EAPI    EMap_Track *            emap_track_new();
EAPI    void                    emap_track_node_add(EMap_Track *track, EMap_Track_Node *node);
EAPI    void                    emap_track_free(EMap_Track *track);
EAPI    void                    emap_track_name_set(EMap_Track *track, const char *name);
EAPI    const char *            emap_track_name_get(EMap_Track *track);
EAPI 	Eina_List *		emap_track_nodes_get(EMap_Track *track);
EAPI 	double 			emap_track_distance_get(EMap_Track *track);
//EAPI 	double 			emap_track_time_get(EMap_Track *track);
//EAPI 	double 			emap_track_average_speed_get(EMap_Track *track);




EAPI    EMap_Track_Node *       emap_track_node_new();
EAPI    void                    emap_track_node_lon_set(EMap_Track_Node *node, double lon);
EAPI    void                    emap_track_node_lat_set(EMap_Track_Node *node, double lat);
EAPI 	double 			emap_track_node_lon_get(EMap_Track_Node *node);
EAPI 	double 			emap_track_node_lat_get(EMap_Track_Node *node);
EAPI 	void 			emap_track_node_elevation_set(EMap_Track_Node *node, double elevation);
EAPI 	double 			emap_track_node_elevation_get(EMap_Track_Node *node);
EAPI	void 			emap_track_node_time_set(EMap_Track_Node *node, time_t time);
EAPI	time_t 			emap_track_node_time_get(EMap_Track_Node *node);
EAPI 	double 			emap_track_nodes_distance_get(EMap_Track_Node *A, EMap_Track_Node *B);

EAPI    void                    emap_track_node_free(EMap_Track_Node *node);

EAPI    EMap_Track *            emap_track_gpx_new(const char *file);


#endif /* EMAP_H_ */
