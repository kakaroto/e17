#include "eclair.h"
#include <Ecore_File.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

static xmlNodePtr _eclair_config_search_node(xmlNodePtr root_node, const char *node_name);

//Init the config manager
Evas_Bool eclair_config_init(Eclair_Config *config)
{
   char *home;

   if (!config)
      return 0;

   config->config_doc = NULL;
   config->config_dir_path[0] = 0;
   config->covers_dir_path[0] = 0;
   config->config_file_path[0] = 0;
   config->default_playlist_path[0] = 0;
   
   if (!(home = getenv("HOME")))
      return 0;

   sprintf(config->config_dir_path, "%s/.eclair/", home);
   ecore_file_mkdir(config->config_dir_path);
   sprintf(config->covers_dir_path, "%scovers/", config->config_dir_path);
   ecore_file_mkdir(config->covers_dir_path);
   sprintf(config->config_file_path, "%seclair.cfg", config->config_dir_path);
   sprintf(config->default_playlist_path, "%splaylist.m3u", config->config_dir_path);
   sprintf(config->database_path, "%seclair.db", config->config_dir_path);

   eclair_config_load(config);

   return 1;
}

//Load the config values from the config file
void eclair_config_load(Eclair_Config *config)
{
   if (!config)
      return;

   if (!(config->config_doc = xmlParseFile(config->config_file_path)))
      config->config_doc = xmlNewDoc("1.0");
   if (!(config->root_node = xmlDocGetRootElement(config->config_doc)) || xmlStrcmp(config->root_node->name, "eclair") != 0)
   {
      config->root_node = xmlNewNode(NULL, "eclair");
      xmlDocSetRootElement(config->config_doc, config->root_node);
      eclair_config_set_default(config);
      eclair_config_save(config);
      return;
   }
}

//Save the config values to the config file
void eclair_config_save(Eclair_Config *config)
{
   if (!config)
      return;

   if (xmlSaveFormatFile(config->config_file_path, config->config_doc, 1) < 0)
      fprintf(stderr, "Config: unable to save config in %s\n", config->config_file_path);
}

//Set the config default values
void eclair_config_set_default(Eclair_Config *config)
{
   if (!config || !config->config_doc || !config->root_node)
      return;

   eclair_config_set_prop_int(config, "gui_window", "x", 0);
   eclair_config_set_prop_int(config, "gui_window", "y", 0);
}

//Return the node called node_name
//NULL if failed
static xmlNodePtr _eclair_config_search_node(xmlNodePtr root_node, const char *node_name)
{
   xmlNodePtr n;

   if (!root_node)
      return NULL;

   for (n = root_node->xmlChildrenNode; n; n = n->next)
   {
      if (xmlStrcmp(n->name, node_name) == 0)
         return n;
   }
   return NULL;
}

//Set prop_name in node_name to the string prop_value
void eclair_config_set_prop_string(Eclair_Config *config, const char *node_name, const char *prop_name, const char *prop_value)
{
   xmlNodePtr node;

   if (!config || !config->config_doc || !config->root_node || !node_name || !prop_name || !prop_value)
      return;

   if (!(node = _eclair_config_search_node(config->root_node, node_name)))
      node = xmlNewChild(config->root_node, NULL, node_name, NULL);

   xmlSetProp(node, prop_name, prop_value);
}

//Set prop_name in node_name to the integer prop_value
void eclair_config_set_prop_int(Eclair_Config *config, const char *node_name, const char *prop_name, int prop_value)
{
   char string[40];
   if (snprintf(string, 40, "%d", prop_value) > 0)
      eclair_config_set_prop_string(config, node_name, prop_name, string);
}

//Set prop_name in node_name to the float prop_value
void eclair_config_set_prop_float(Eclair_Config *config, const char *node_name, const char *prop_name, float prop_value)
{
   char string[40];
   if (snprintf(string, 40, "%f", prop_value) > 0)
      eclair_config_set_prop_string(config, node_name, prop_name, string);
}

//Get the string prop_name in node_name
//Return 0 if failed
//The string will have to be freed if success
int eclair_config_get_prop_string(Eclair_Config *config, const char *node_name, const char *prop_name, char **prop_value)
{
   xmlNodePtr node;
   xmlChar *value;

   if (!config || !config->config_doc || !config->root_node || !node_name || !prop_name)
      return 0;

   if (!(node = _eclair_config_search_node(config->root_node, node_name)))
      return 0;
   if (!(value = xmlGetProp(node, prop_name)))
      return 0;
   *prop_value = strdup(value);
   xmlFree(value);
   return 1;
}

//Get the integer prop_name in node_name
//Return 0 if failed
int eclair_config_get_prop_int(Eclair_Config *config, const char *node_name, const char *prop_name, int *prop_value)
{
   char *value_string;

   if (!eclair_config_get_prop_string(config, node_name, prop_name, &value_string))
      return 0;

   if (sscanf(value_string, "%d", prop_value) != 1)
   {
      free(value_string);      
      return 0;
   }
   free(value_string);
   return 1;
}

//Get the float prop_name in node_name
//Return 0 if failed
int eclair_config_get_prop_float(Eclair_Config *config, const char *node_name, const char *prop_name, float *prop_value)
{
   char *value_string;

   if (!eclair_config_get_prop_string(config, node_name, prop_name, &value_string))
      return 0;

   if (sscanf(value_string, "%f", prop_value) != 1)
   {
      free(value_string);      
      return 0;
   }
   free(value_string);
   return 1;
}

//Shutdown the config manager
void eclair_config_shutdown(Eclair_Config *config)
{
   if (!config)
      return;

   eclair_config_save(config);
   xmlFreeDoc(config->config_doc);
}
