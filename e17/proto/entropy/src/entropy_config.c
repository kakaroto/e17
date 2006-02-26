#include "entropy.h"
#include "Ecore_Config.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "entropy_gui.h"

#define HEADER_CONFIG_MAX 2048


entropy_config* entropy_config_init(entropy_core* core) {
	entropy_config* config = entropy_malloc(sizeof(entropy_config));
	struct stat config_dir_stat;
	int i;

	i = strlen(entropy_core_home_dir_get()) + strlen("/.e/apps/entropy") + 2;
	config->config_dir = entropy_malloc(i * sizeof(char));
	snprintf(config->config_dir, i, "%s/%s", entropy_core_home_dir_get(), "/.e/apps/entropy");

	printf("Config dir is: '%s'\n", config->config_dir);

	config->config_dir_and_file = entropy_malloc((strlen(config->config_dir) * sizeof(char)) + (13 * sizeof(char)));
	snprintf(config->config_dir_and_file, strlen(config->config_dir) + 13, "%s/%s", config->config_dir, "entropy.cfg");
	

	//printf ("Config dir set to '%s'\n", config->config_dir);
	//printf ("Config file at '%s'\n", config->config_dir_and_file);

	//Does the config dir exist?
	if (stat(config->config_dir, &config_dir_stat)) {
		//Make the dir..
		mkdir(config->config_dir, 0777);
	}

	//Init ecore_config
	ecore_config_init("entropy_config");
	ecore_config_file_load(config->config_dir_and_file);

	return config;
}

int entropy_config_int_get(char* module, char* key) {
	int i = (strlen(module) * sizeof(char)) + sizeof(char) + (strlen(key)*sizeof(char)) + 1;
	int val;
	char* tmp = entropy_malloc( i);
	snprintf(tmp, i, "%s.%s", module,key);

	val = ecore_config_int_get(tmp);
	entropy_free(tmp);

	return val;
}

void entropy_config_int_set(char* module, char* key, int value) {
	int i = (strlen(module) * sizeof(char)) + sizeof(char) + (strlen(key)*sizeof(char)) + 1;
	char* tmp = entropy_malloc( i);
	snprintf(tmp, i, "%s.%s", module,key);

	ecore_config_int_set(tmp, value);
	entropy_free(tmp);


}

char* entropy_config_str_get(char* module, char* key) {
	int i = (strlen(module) * sizeof(char)) + sizeof(char) + (strlen(key)*sizeof(char)) + 1;
	char* val;
	char* tmp = entropy_malloc( i);
	snprintf(tmp, i, "%s.%s", module,key);

	val = ecore_config_string_get(tmp);
	entropy_free(tmp);

	return val;
	
}

void entropy_config_str_set(char* module, char* key, char* value) {
	int i = (strlen(module) * sizeof(char)) + sizeof(char) + (strlen(key)*sizeof(char)) + 1;
	char* tmp = entropy_malloc( i);
	snprintf(tmp, i, "%s.%s", module,key);

	ecore_config_string_set(tmp, value);
	entropy_free(tmp);


}



void entropy_config_destroy(entropy_config* config) {
	//printf("Saving config to '%s'\n", config->config_dir_and_file);
	ecore_config_file_save(config->config_dir_and_file);
	
	//printf("STUB\n");
}





/*Config helper functions*/


Ecore_Hash *
entropy_config_standard_structures_parse (entropy_gui_component_instance * instance,
				char *config)
{
  Ecore_Hash *ret = ecore_hash_new (ecore_str_hash, ecore_str_compare);

  if (!strstr (config, "|")) {
    char *name;
    char *uri;

    //printf("Simple case - only one object...\n");

    name = strtok (config, ";");
    uri = strtok (NULL, ";");

    ecore_hash_set(ret, strdup(name), strdup(uri));

  }
  else {
    Ecore_List *objects = ecore_list_new ();
    char *object;
    char *name;
    char *uri;

    //printf("Complex case, multiple objects...\n");

    object = strtok (config, "|");
    ecore_list_append (objects, strdup (object));
    while ((object = strtok (NULL, "|"))) {
      ecore_list_append (objects, strdup (object));
    }

    ecore_list_goto_first (objects);
    while ((object = ecore_list_next (objects))) {
      name = strtok (object, ";");
      uri = strtok (NULL, ";");

      

      ecore_hash_set(ret, strdup(name), strdup(uri));
      free (object);
    }
    ecore_list_destroy (objects);

  }

  return ret;
}

void
entropy_config_standard_structures_add (entropy_gui_component_instance *
				       instance, char *name, char *uri)
{
  char *current_uri =
    entropy_config_str_get ("layout_ewl_simple", "structure_bar");
  char new_uri[HEADER_CONFIG_MAX];

  snprintf (new_uri, HEADER_CONFIG_MAX, "%s|%s;%s", current_uri, name, uri);
  entropy_config_str_set ("layout_ewl_simple", "structure_bar", new_uri);

  entropy_free (current_uri);
}

void
entropy_config_standard_structures_create ()
{
  entropy_core* core = entropy_core_get_core();
	
  char *eg = calloc (HEADER_CONFIG_MAX, sizeof (char));

  snprintf (eg, HEADER_CONFIG_MAX,
	    "Computer;file:///|Home;file://%s|Samba Example (Don't use!);smb://username:password@/test/machine/folder",
	    entropy_core_home_dir_get (core));

  //printf("Setting default config string..\n");
  entropy_config_str_set ("layout_ewl_simple", "structure_bar", eg);

  free (eg);
}

