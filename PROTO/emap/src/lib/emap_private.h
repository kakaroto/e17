#ifndef EMAP_PRIVATE_H_
#define EMAP_PRIVATE_H_

#include "EMap.h"


extern int LOG_DOMAIN;

#define WAR(...)           EINA_LOG_DOM_WARN(LOG_DOMAIN, __VA_ARGS__)
#define CRI(...)           EINA_LOG_DOM_CRIT(LOG_DOMAIN, __VA_ARGS__)
#define INF(...)           EINA_LOG_DOM_INFO(LOG_DOMAIN, __VA_ARGS__)
#define ERR(...)           EINA_LOG_DOM_ERR(LOG_DOMAIN, __VA_ARGS__)
#define DBG(...)           EINA_LOG_DOM_DBG(LOG_DOMAIN, __VA_ARGS__)



struct emap_route
{
   EMap_Route_Type type;

   const char *name;
   Eina_List *nodes; //list of EMap_Route_Node*

   struct {
      const char *file;

      Eina_Bool xml_is_name;
      Eina_Bool xml_is_ele;
      Eina_Bool xml_is_time;
      EMap_Route_Node *xml_current_node;
   } gpx;
};


#endif /* EMAP_PRIVATE_H_ */
