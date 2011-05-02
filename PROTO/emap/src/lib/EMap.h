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

EAPI    EMap_Track *            emap_track_new();
EAPI    void                    emap_track_node_add(EMap_Track *track, EMap_Track_Node *node);

EAPI    EMap_Track_Node *       emap_track_node_new();
EAPI    void                    emap_track_node_lon_set(EMap_Track_Node *node, double lon);
EAPI    void                    emap_track_node_lat_set(EMap_Track_Node *node, double lat);

EAPI    EMap_Track *            emap_track_gpx_new(const char *file);


#endif /* EMAP_H_ */
