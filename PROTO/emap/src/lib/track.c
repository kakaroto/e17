
#include "EMap.h"

struct emap_track
{
   EMap_Track_Type type;
   Eina_List *nodes; //list of EMap_Track_Node*
};

EMap_Track *emap_track_new(EMap_Track_Type type)
{
   EMap_Track *track = calloc(1, sizeof(EMap_Track));
   track->type = EMAP_TRACK_TYPE_GPX;
   return track;
}

void map_track_node_add(EMap_Track *track, EMap_Track_Node *node)
{
   track->nodes = eina_list_append(track->nodes, node);
}
