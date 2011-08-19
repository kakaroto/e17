
#include "emap_private.h"
#include <time.h>


#define GPX_NAME "name>"
#define GPX_COORDINATES "trkpt "
#define GPX_LON "lon"
#define GPX_LAT "lat"
#define GPX_ELE "ele>"
#define GPX_TIME "time>"

static  void             _parse(EMap_Route *route);
static  Eina_Bool        _parser_cb(void *data, Eina_Simple_XML_Type type,
                                    const char *content, unsigned offset, unsigned length);
static  Eina_Bool        _parser_attributes_cb(void *data, const char *key, const char *value);

EMap_Route *emap_route_gpx_new(const char *file)
{
   if(!file)
   {
      ERR("File is null");
      return NULL;
   }

   EMap_Route *route = emap_route_new(EMAP_TRACK_TYPE_GPX);
   route->gpx.file = eina_stringshare_add(file);

   _parse(route);

   return route;
}

static void _parse(EMap_Route *route)
{
   if(!route) return;
   if(!route->type == EMAP_TRACK_TYPE_GPX) return;

   DBG("[Start] Load GPX file %s", route->gpx.file);

   FILE *f = fopen(route->gpx.file, "rb");
   if (f)
   {
      long sz;

      fseek(f, 0, SEEK_END);
      sz = ftell(f);
      if (sz > 0)
      {
         char *buf;

         fseek(f, 0, SEEK_SET);
         buf = malloc(sz);
         if (buf)
         {
            eina_simple_xml_parse(buf, sz, EINA_TRUE, _parser_cb, route);

            free(buf);
         }
      }
      fclose(f);
   }
   else
   {
      ERR("can't open the file %s", route->gpx.file);
      return ;
   }

   DBG("[Done] Load GPX file %s", route->gpx.file);
}

static Eina_Bool _parser_cb(void *data, Eina_Simple_XML_Type type,
                            const char *content, unsigned offset, unsigned length)
{
   EMap_Route *route = data;
   double d;
   struct tm time;
   time_t timet;
   char buf[length+1];

   if(type == EINA_SIMPLE_XML_OPEN)
   {
      if(!strncmp(GPX_NAME, content, strlen(GPX_NAME)))
      {
         route->gpx.xml_is_name = EINA_TRUE;
      }
      else if(!strncmp(GPX_ELE, content, strlen(GPX_ELE)))
      {
         route->gpx.xml_is_ele = EINA_TRUE;
      }
      else if(!strncmp(GPX_TIME, content, strlen(GPX_TIME)))
      {
         route->gpx.xml_is_time = EINA_TRUE;
      }
      else if(!strncmp(GPX_COORDINATES, content, strlen(GPX_COORDINATES)))
      {
         EMap_Route_Node *node = emap_route_node_new(eina_list_count(route->nodes));
         route->gpx.xml_current_node = node;
         emap_route_node_add(route, node);

         const char *tags = eina_simple_xml_tag_attributes_find(content, length);
         eina_simple_xml_attributes_parse(tags, length - (tags - content), _parser_attributes_cb, node);
      }
   }
   else if(type == EINA_SIMPLE_XML_DATA)
   {
      if(route->gpx.xml_is_name)
      {
         eina_stringshare_replace_length(&route->name, content, length);
         route->gpx.xml_is_name = EINA_FALSE;
      }
      else if(route->gpx.xml_is_ele)
      {
         sscanf(content, "%lf", &d);
         emap_route_node_elevation_set(route->gpx.xml_current_node, d);

         route->gpx.xml_is_ele = EINA_FALSE;
      }
      else if(route->gpx.xml_is_time)
      {
         snprintf(buf, length + 1, content);
         strptime(buf, "%Y-%m-%dT%H:%M:%S%Z", &time);
         timet = mktime(&time);
         emap_route_node_time_set(route->gpx.xml_current_node, timet);
         route->gpx.xml_is_time = EINA_FALSE;
      }
   }
   return EINA_TRUE;
}


static Eina_Bool _parser_attributes_cb(void *data, const char *key, const char *value)
{
   EMap_Route_Node *node = data;
   double d;

   if(!strcmp(GPX_LAT, key))
   {
      sscanf(value, "%lf", &d);
      emap_route_node_lat_set(node, d);
   }
   else if(!strcmp(GPX_LON, key))
   {
      sscanf(value, "%lf", &d);
      emap_route_node_lon_set(node, d);
   }

   return EINA_TRUE;
}
