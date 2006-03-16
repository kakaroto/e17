#include "entropy.h"
#include "Ecore_Config.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "entropy_gui.h"
#include <Eet.h>
#include <stdarg.h>

static Entropy_Config* _Entropy_Config = NULL;

#define HEADER_CONFIG_MAX 2048
static Eet_Data_Descriptor *_entropy_config_mimes_edd;
static Eet_Data_Descriptor *_entropy_config_mime_binding_edd;
static Eet_Data_Descriptor *_entropy_config_mime_binding_action_edd;


Entropy_Config_Mime_Binding*
entropy_config_mime_binding_for_type_get(char* type)
{
	Entropy_Config_Mime_Binding* rbinding = NULL;
	Entropy_Config_Mime_Binding* binding = NULL;
	
	Evas_List* l;
	
	/*Do a scan for now, we should dynamically load this into a hash*/
	for (l = _Entropy_Config->Config_Mimes->mime_bindings; l; ) {
		binding = l->data;

		if (!strcmp(binding->mime_type, type)) {
			rbinding = binding;
		}

		l = l->next;

	}
	

	return rbinding;
}


void
entropy_config_mimes_print(Entropy_Config_Mime* mimes) 
{
	Entropy_Config_Mime_Binding* binding;
	Entropy_Config_Mime_Binding_Action* action;
	Evas_List* l, *l2;

	for (l = mimes->mime_bindings; l; ) {
		binding = l->data;
		
		printf("MIME Type: '%s' ->\n", binding->mime_type);
		for (l2 = binding->actions; l2; ) {
			action = l2->data;

			printf("   %s, %s, %s\n", action->app_description, action->executable, action->args);

			l2 = l2->next;
		}

		l = l->next;
	}
}

Entropy_Config_Mime_Binding*
entropy_config_binding_new(char* mime_type, Entropy_Config_Mime_Binding_Action* action1, ...)
{
	va_list args;
	Entropy_Config_Mime_Binding_Action* action;
	Entropy_Config_Mime_Binding* binding = calloc(1,sizeof(Entropy_Config_Mime_Binding));

	va_start(args, action1);
	for (action = action1; action; action = va_arg(args, Entropy_Config_Mime_Binding_Action*)) {
		binding->actions = evas_list_append(binding->actions, action);
	}
	
	va_end(args);

	binding->mime_type = strdup(mime_type);

	return binding;
}


Entropy_Config_Mime_Binding_Action*
entropy_config_binding_action_new(char* description, char* executable, char* args)
{
	Entropy_Config_Mime_Binding_Action* action = calloc(1,sizeof(Entropy_Config_Mime_Binding_Action));

	action->app_description = strdup(description);
	action->executable = strdup(executable);
	action->args = strdup(args);

	return action;
}

Entropy_Config* entropy_config_init(entropy_core* core) {
	struct stat config_dir_stat;
	int i;
	Eet_File* conf_file;
	Entropy_Config_Mime* mimes = calloc(1,sizeof(Entropy_Config_Mime));

	
	if (_Entropy_Config) return _Entropy_Config;
	
	_Entropy_Config = entropy_malloc(sizeof(Entropy_Config));


	i = strlen(entropy_core_home_dir_get()) + strlen("/.e/apps/entropy") + 2;
	_Entropy_Config->config_dir = entropy_malloc(i * sizeof(char));
	snprintf(_Entropy_Config->config_dir, i, "%s/%s", entropy_core_home_dir_get(), "/.e/apps/entropy");

	printf("Config dir is: '%s'\n", _Entropy_Config->config_dir);

	_Entropy_Config->config_dir_and_file = entropy_malloc((strlen(_Entropy_Config->config_dir) * sizeof(char)) + (13 * sizeof(char)));
	snprintf(_Entropy_Config->config_dir_and_file, strlen(_Entropy_Config->config_dir) + 13, "%s/%s", 
			_Entropy_Config->config_dir, "entropy.cfg");

	_Entropy_Config->config_dir_and_file_eet = entropy_malloc((strlen(_Entropy_Config->config_dir) * sizeof(char)) + (16 * sizeof(char)));
	snprintf(_Entropy_Config->config_dir_and_file_eet, strlen(_Entropy_Config->config_dir) + 16, "%s/%s", 
			_Entropy_Config->config_dir, "eetentropy.cfg");

	/*Build the EDDs*/
	_entropy_config_mime_binding_action_edd = 
	eet_data_descriptor_new("entropy_config_mime_binding_action", sizeof(Entropy_Config_Mime_Binding_Action),
                              (void *(*)(void *))evas_list_next,
                              (void *(*)(void *, void *))evas_list_append,
                              (void *(*)(void *))evas_list_data,
                              (void *(*)(void *))evas_list_free,
                              (void (*)
                               (void *,
                                int (*)(void *, const char *, void *, void *),
                                void *))evas_hash_foreach, (void *(*)(void *,
                                                                      const char
                                                                      *,
                                                                      void *))
                              evas_hash_add, (void (*)(void *))evas_hash_free);

	EET_DATA_DESCRIPTOR_ADD_BASIC(_entropy_config_mime_binding_action_edd, Entropy_Config_Mime_Binding_Action,
                                 "app_description", app_description, EET_T_STRING);
	EET_DATA_DESCRIPTOR_ADD_BASIC(_entropy_config_mime_binding_action_edd, Entropy_Config_Mime_Binding_Action,
                                 "executable", executable, EET_T_STRING);
	EET_DATA_DESCRIPTOR_ADD_BASIC(_entropy_config_mime_binding_action_edd, Entropy_Config_Mime_Binding_Action,
                                 "args", args, EET_T_STRING);


	_entropy_config_mime_binding_edd =
	eet_data_descriptor_new("entropy_config_mime_binding", sizeof(Entropy_Config_Mime_Binding),
                              (void *(*)(void *))evas_list_next,
                              (void *(*)(void *, void *))evas_list_append,
                              (void *(*)(void *))evas_list_data,
                              (void *(*)(void *))evas_list_free,
                              (void (*)
                               (void *,
                                int (*)(void *, const char *, void *, void *),
                                void *))evas_hash_foreach, (void *(*)(void *,
                                                                      const char
                                                                      *,
                                                                      void *))
                              evas_hash_add, (void (*)(void *))evas_hash_free);

	EET_DATA_DESCRIPTOR_ADD_BASIC(_entropy_config_mime_binding_edd, Entropy_Config_Mime_Binding,
                                 "mime_type", mime_type, EET_T_STRING);
	EET_DATA_DESCRIPTOR_ADD_LIST(_entropy_config_mime_binding_edd, Entropy_Config_Mime_Binding, "actions", actions, 
				_entropy_config_mime_binding_action_edd);


	_entropy_config_mimes_edd =
	eet_data_descriptor_new("entropy_config_mime", sizeof(Entropy_Config_Mime),
                              (void *(*)(void *))evas_list_next,
                              (void *(*)(void *, void *))evas_list_append,
                              (void *(*)(void *))evas_list_data,
                              (void *(*)(void *))evas_list_free,
                              (void (*)
                               (void *,
                                int (*)(void *, const char *, void *, void *),
                                void *))evas_hash_foreach, (void *(*)(void *,
                                                                      const char
                                                                      *,
                                                                      void *))
                              evas_hash_add, (void (*)(void *))evas_hash_free);
	EET_DATA_DESCRIPTOR_ADD_LIST(_entropy_config_mimes_edd, Entropy_Config_Mime, "mime_bindings", mime_bindings, 
				_entropy_config_mime_binding_edd);





	mimes->mime_bindings = evas_list_append(mimes->mime_bindings, 
			entropy_config_binding_new("image/jpeg",
				entropy_config_binding_action_new("Exhibit (Single File)", "exhibit", "\%pf"),
				entropy_config_binding_action_new("Exhibit (Directory)", "exhibit", "\%p"),
				NULL
				));
	mimes->mime_bindings = evas_list_append(mimes->mime_bindings, 
			entropy_config_binding_new("image/png",
				entropy_config_binding_action_new("Exhibit (Single File)", "exhibit", "\%pf"),
				entropy_config_binding_action_new("Exhibit (Directory)", "exhibit", "\%p"),
				NULL
				));

	mimes->mime_bindings = evas_list_append(mimes->mime_bindings, 
			entropy_config_binding_new("image/gif",
				entropy_config_binding_action_new("Exhibit (Single File)", "exhibit", "\%pf"),
				entropy_config_binding_action_new("Exhibit (Directory)", "exhibit", "\%p"),
				NULL
				));

	mimes->mime_bindings = evas_list_append(mimes->mime_bindings, 
			entropy_config_binding_new("text/csrc",
				entropy_config_binding_action_new("Gvim", "gvim", "\%pf"),
				NULL
				));

	mimes->mime_bindings = evas_list_append(mimes->mime_bindings, 
			entropy_config_binding_new("text/html",
				entropy_config_binding_action_new("Firefox", "firefox", "\%pf"),
				NULL
				));

	mimes->mime_bindings = evas_list_append(mimes->mime_bindings, 
			entropy_config_binding_new("audio/x-mp3",
				entropy_config_binding_action_new("Xmms", "xmms", "\%pf"),
				entropy_config_binding_action_new("Mpg123 (via eVFS)", "evfscat \"%u\" | mpg123 -", ""),
				NULL
				));

	mimes->mime_bindings = evas_list_append(mimes->mime_bindings, 
			entropy_config_binding_new("video/x-ms-wmv",
				entropy_config_binding_action_new("MPlayer", "mplayer", "\%pf"),
				entropy_config_binding_action_new("MPlayer (via evfs)", "evfscat \"%u\" | mplayer -cache 4096 -", ""),
				entropy_config_binding_action_new("Xine", "xine", "%pf"),
				NULL
				));

	mimes->mime_bindings = evas_list_append(mimes->mime_bindings, 
			entropy_config_binding_new("video/mpeg",
				entropy_config_binding_action_new("MPlayer", "mplayer", "\%pf"),
				entropy_config_binding_action_new("MPlayer (via evfs)", "evfscat \"%u\" | mplayer -cache 4096 -", ""),
				entropy_config_binding_action_new("Xine", "xine", "%pf"),
				NULL
				));

	mimes->mime_bindings = evas_list_append(mimes->mime_bindings, 
			entropy_config_binding_new("video/x-msvideo",
				entropy_config_binding_action_new("MPlayer", "mplayer", "\%pf"),
				entropy_config_binding_action_new("MPlayer (via evfs)", "evfscat \"%u\" | mplayer -cache 4096 -", ""),
				entropy_config_binding_action_new("Xine", "xine", "%pf"),
				NULL
				));

        /*ecore_list_append(mime_type_actions, "application/msword:abiword");
        ecore_list_append(mime_type_actions, "application/vnd.ms-excel:gnumeric");*/

	
	conf_file = eet_open(_Entropy_Config->config_dir_and_file_eet, EET_FILE_MODE_WRITE);
	if (conf_file) {
		char* data;
		int size_ret;
		int ok;
			
		printf("Conf file loaded to: %p\n", conf_file);

		data =
	        eet_data_descriptor_encode(_entropy_config_mimes_edd, mimes, &size_ret);
		
		printf("Write data: %p\n", data);
		
		if ( !(ok = eet_write(conf_file, "/config_action", data, 
	       		size_ret, 0))) {
			printf("Error writing data!\n");
			
		}
					
		free(data);
		eet_close(conf_file);

		conf_file = eet_open(_Entropy_Config->config_dir_and_file_eet, EET_FILE_MODE_READ);
			

		data = eet_read(conf_file, "/config_action", &size_ret);
		printf("Data is %p\n", data);
			
		_Entropy_Config->Config_Mimes = eet_data_descriptor_decode(_entropy_config_mimes_edd, data, size_ret);

	}

	/*Print them out..*/
	entropy_config_mimes_print(_Entropy_Config->Config_Mimes);


	//Does the config dir exist?
	if (stat(_Entropy_Config->config_dir, &config_dir_stat)) {
		//Make the dir..
		mkdir(_Entropy_Config->config_dir, 0777);
	}

	//Init ecore_config
	ecore_config_init("entropy_config");
	ecore_config_file_load(_Entropy_Config->config_dir_and_file);

	return _Entropy_Config;
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



void entropy_config_destroy(Entropy_Config* config) {
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

