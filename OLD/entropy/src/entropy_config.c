#include "entropy.h"
#include "Ecore_Config.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "entropy_gui.h"
#include <Eet.h>
#include <stdarg.h>

#define ENTROPY_CONFIG_VERSION 17

static Entropy_Config* _Entropy_Config = NULL;

#define HEADER_CONFIG_MAX 2048
static Eet_Data_Descriptor *_entropy_config_loaded_edd;
static Eet_Data_Descriptor *_entropy_config_mime_binding_edd;
static Eet_Data_Descriptor *_entropy_config_mime_binding_action_edd;
static Eet_Data_Descriptor *_entropy_config_structure_edd;
static Eet_Data_Descriptor *_entropy_config_item_edd;


void entropy_config_loaded_config_free()
{
	Eina_List *l, *l2;
	Entropy_Config_Mime_Binding* binding;
	Entropy_Config_Mime_Binding_Action* action;
	Entropy_Config_Structure* structure;


	
	/*Free MIME bindings*/
	for (l = _Entropy_Config->Loaded_Config->mime_bindings; l; ) {
		binding = l->data;

		for (l2 = binding->actions; l2; ) {
			action = l2->data;

			IF_FREE(action->app_description);
			IF_FREE(action->executable);
			IF_FREE(action->args);				

			l2 = l2->next;
		}

		IF_FREE(binding->desc);
		IF_FREE(binding->mime_type);
		eina_list_free(binding->actions);
		IF_FREE(binding);

		l = l->next;

	}
	eina_list_free(_Entropy_Config->Loaded_Config->mime_bindings);

	/*Free Structure bindings*/
	for (l = _Entropy_Config->Loaded_Config->structures; l; ) {
		structure = l->data;

		IF_FREE(structure->name);
		IF_FREE(structure->uri);
		IF_FREE(structure);
		
		l = l->next;
	}
	eina_list_free(_Entropy_Config->Loaded_Config->structures);

	
	free(_Entropy_Config->Loaded_Config);

}



void entropy_config_edd_build() 
{
	/*Build the EDDs*/
	_entropy_config_mime_binding_action_edd = 
	eet_data_descriptor_new("entropy_config_mime_binding_action", sizeof(Entropy_Config_Mime_Binding_Action),
                              (void *(*)(void *))eina_list_next,
                              (void *(*)(void *, void *))eina_list_append,
                              (void *(*)(void *))eina_list_data_get,
                              (void *(*)(void *))eina_list_free,
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
                              (void *(*)(void *))eina_list_next,
                              (void *(*)(void *, void *))eina_list_append,
                              (void *(*)(void *))eina_list_data_get,
                              (void *(*)(void *))eina_list_free,
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
	EET_DATA_DESCRIPTOR_ADD_BASIC(_entropy_config_mime_binding_edd, Entropy_Config_Mime_Binding,
                                 "description", desc, EET_T_STRING);
	EET_DATA_DESCRIPTOR_ADD_LIST(_entropy_config_mime_binding_edd, Entropy_Config_Mime_Binding, "actions", actions, 
				_entropy_config_mime_binding_action_edd);

	_entropy_config_item_edd =
	eet_data_descriptor_new("entropy_config_item", sizeof(Entropy_Config_Item),
                              (void *(*)(void *))eina_list_next,
                              (void *(*)(void *, void *))eina_list_append,
                              (void *(*)(void *))eina_list_data_get,
                              (void *(*)(void *))eina_list_free,
                              (void (*)
                               (void *,
                                int (*)(void *, const char *, void *, void *),
                                void *))evas_hash_foreach, (void *(*)(void *,
                                                                      const char
                                                                      *,
                                                                      void *))
                              evas_hash_add, (void (*)(void *))evas_hash_free);
	EET_DATA_DESCRIPTOR_ADD_BASIC(_entropy_config_item_edd, Entropy_Config_Item,
                                 "name", name, EET_T_STRING);
	EET_DATA_DESCRIPTOR_ADD_BASIC(_entropy_config_item_edd, Entropy_Config_Item,
                                 "value", value, EET_T_STRING);

	_entropy_config_structure_edd = 
	eet_data_descriptor_new("entropy_config_mime_binding", sizeof(Entropy_Config_Structure),
                              (void *(*)(void *))eina_list_next,
                              (void *(*)(void *, void *))eina_list_append,
                              (void *(*)(void *))eina_list_data_get,
                              (void *(*)(void *))eina_list_free,
                              (void (*)
                               (void *,
                                int (*)(void *, const char *, void *, void *),
                                void *))evas_hash_foreach, (void *(*)(void *,
                                                                      const char
                                                                      *,
                                                                      void *))
                              evas_hash_add, (void (*)(void *))evas_hash_free);
	EET_DATA_DESCRIPTOR_ADD_BASIC(_entropy_config_structure_edd, Entropy_Config_Structure,
                                 "name", name, EET_T_STRING);
	EET_DATA_DESCRIPTOR_ADD_BASIC(_entropy_config_structure_edd, Entropy_Config_Structure,
                                 "uri", uri, EET_T_STRING);

	
	_entropy_config_loaded_edd =
	eet_data_descriptor_new("entropy_config_loaded", sizeof(Entropy_Config_Loaded),
                              (void *(*)(void *))eina_list_next,
                              (void *(*)(void *, void *))eina_list_append,
                              (void *(*)(void *))eina_list_data_get,
                              (void *(*)(void *))eina_list_free,
                              (void (*)
                               (void *,
                                int (*)(void *, const char *, void *, void *),
                                void *))evas_hash_foreach, (void *(*)(void *,
                                                                      const char
                                                                      *,
                                                                      void *))
                              evas_hash_add, (void (*)(void *))evas_hash_free);
	EET_DATA_DESCRIPTOR_ADD_LIST(_entropy_config_loaded_edd, Entropy_Config_Loaded, "mime_bindings", mime_bindings, 
				_entropy_config_mime_binding_edd);
	EET_DATA_DESCRIPTOR_ADD_LIST(_entropy_config_loaded_edd, Entropy_Config_Loaded, "structures", structures, 
				_entropy_config_structure_edd);
	EET_DATA_DESCRIPTOR_ADD_LIST(_entropy_config_loaded_edd, Entropy_Config_Loaded, "Misc_Config_Load", Misc_Config_Load, 
				_entropy_config_item_edd);
}


void entropy_config_version_check()
{
	char* data;
	int size_ret;
	Eet_File* conf_file;
	
	printf("Looking for version in '%s'..\n", _Entropy_Config->config_dir_and_file_eet);
	
	conf_file = eet_open(_Entropy_Config->config_dir_and_file_eet, EET_FILE_MODE_READ);
	if (conf_file) {
		data = eet_read(conf_file, "/config_version", &size_ret);
		printf("Size ret is: %d\n", size_ret);
		if (!data || atoi(data) < ENTROPY_CONFIG_VERSION) {
			if (data)
				printf("***** Config out of date, had to make new... %s < %d\n", data, ENTROPY_CONFIG_VERSION);
			else 
				printf("***** Config doesn't exist, creating..\n");

			/*Remove the config file, it's out of date..*/
			remove(_Entropy_Config->config_dir_and_file_eet);
		} else {
			printf("***** Config fine - %s matches %d\n", data, ENTROPY_CONFIG_VERSION);
		}
	}
	eet_close(conf_file);
}


Entropy_Config_Mime_Binding*
entropy_config_mime_binding_for_type_get(char* type)
{
	Entropy_Config_Mime_Binding* rbinding = NULL;
	Entropy_Config_Mime_Binding* binding = NULL;
	
	Eina_List* l;
	
	/*Do a scan for now, we should dynamically load this into a hash*/
	for (l = _Entropy_Config->Loaded_Config->mime_bindings; l; ) {
		binding = l->data;

		if (!strcmp(binding->mime_type, type)) {
			rbinding = binding;
		}

		l = l->next;

	}
	

	return rbinding;
}


void
entropy_config_mimes_print(Entropy_Config_Loaded* mimes) 
{
	Entropy_Config_Mime_Binding* binding;
	Entropy_Config_Mime_Binding_Action* action;
	Eina_List* l, *l2;

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


Entropy_Config_Structure*
entropy_config_structure_new(char* name, char* uri)
{
	Entropy_Config_Structure* structure = entropy_malloc(sizeof(Entropy_Config_Structure));

	structure->name = strdup(name);
	structure->uri = strdup(uri);

	return structure;
}

Entropy_Config_Mime_Binding*
entropy_config_binding_new(char* mime_type, char* description, Entropy_Config_Mime_Binding_Action* action1, ...)
{
	va_list args;
	Entropy_Config_Mime_Binding_Action* action;
	Entropy_Config_Mime_Binding* binding = entropy_malloc(sizeof(Entropy_Config_Mime_Binding));

	va_start(args, action1);
	for (action = action1; action; action = va_arg(args, Entropy_Config_Mime_Binding_Action*)) {
		binding->actions = eina_list_append(binding->actions, action);
	}
	
	va_end(args);

	binding->mime_type = strdup(mime_type);
	binding->desc = strdup(description);

	return binding;
}


Entropy_Config_Mime_Binding_Action*
entropy_config_binding_action_new(char* description, char* executable, char* args)
{
	Entropy_Config_Mime_Binding_Action* action = entropy_malloc(sizeof(Entropy_Config_Mime_Binding_Action));

	action->app_description = strdup(description);
	action->executable = strdup(executable);
	action->args = strdup(args);

	return action;
}

Entropy_Config* entropy_config_init(entropy_core* core) {
	struct stat config_dir_stat;
	struct stat eetstat;
	int i;
	Eet_File* conf_file;
	char* data;
	int size_ret;
	int ok;
	Eina_List* l;
	Entropy_Config_Item* item;
	Entropy_Config_Loaded* mimes;
	
	if (_Entropy_Config) return _Entropy_Config;
	
	_Entropy_Config = entropy_malloc(sizeof(Entropy_Config));
	mimes = entropy_malloc(sizeof(Entropy_Config_Loaded));

	i = strlen(entropy_core_home_dir_get()) + strlen("/.e/entropy") + 2;
	_Entropy_Config->config_dir = entropy_malloc(i * sizeof(char));
	snprintf(_Entropy_Config->config_dir, i, "%s/%s", entropy_core_home_dir_get(), "/.e/entropy");

	printf("Config dir is: '%s'\n", _Entropy_Config->config_dir);

	_Entropy_Config->config_dir_and_file = entropy_malloc((strlen(_Entropy_Config->config_dir) * sizeof(char)) + (13 * sizeof(char)));
	snprintf(_Entropy_Config->config_dir_and_file, strlen(_Entropy_Config->config_dir) + 13, "%s/%s", 
			_Entropy_Config->config_dir, "entropy.cfg");

	_Entropy_Config->config_dir_and_file_eet = entropy_malloc((strlen(_Entropy_Config->config_dir) * sizeof(char)) + (16 * sizeof(char)));
	snprintf(_Entropy_Config->config_dir_and_file_eet, strlen(_Entropy_Config->config_dir) + 16, "%s/%s", 
			_Entropy_Config->config_dir, "eetentropy.cfg");

	//Does the config dir exist?
	if (stat(_Entropy_Config->config_dir, &config_dir_stat)) {
		//Make the dir..
		mkdir(_Entropy_Config->config_dir, 0777);
	}

	/*Build the eet config data structures*/
	entropy_config_edd_build();

	/*Check the version of the config, and remove if necessary*/
	entropy_config_version_check();


	/*Check for existence of eet file*/
	//Does the config dir exist?
	if (stat(_Entropy_Config->config_dir_and_file_eet, &eetstat)) {
		//Make the dir..

		_Entropy_Config->Loaded_Config = mimes;
		entropy_config_defaults_populate(mimes);

		
	        /*ecore_list_append(mime_type_actions, "application/msword:abiword");
	        ecore_list_append(mime_type_actions, "application/vnd.ms-excel:gnumeric");*/
	
	
		conf_file = eet_open(_Entropy_Config->config_dir_and_file_eet, EET_FILE_MODE_WRITE);
		if (conf_file) {
			char buf[10];

			/*Write the config version..*/
			snprintf(buf,10,"%d", ENTROPY_CONFIG_VERSION);
			printf("Writing config version '%s'..\n", buf);
			if (!(ok = eet_write(conf_file, "/config_version", buf, 10, 0))) {
				printf("Error writing data!\n");
			}
	
			data = eet_data_descriptor_encode(_entropy_config_loaded_edd, mimes, &size_ret);
			if ( !(ok = eet_write(conf_file, "/config_action", data, 
		       		size_ret, 0))) {
				printf("Error writing data!\n");
				
			}
			free(data);

			eet_close(conf_file);
		}

		entropy_config_loaded_config_free();
	}

	/*Load the eet config*/
	conf_file = eet_open(_Entropy_Config->config_dir_and_file_eet, EET_FILE_MODE_READ);
	if (conf_file) {
		data = eet_read(conf_file, "/config_action", &size_ret);
		if (data) {
			_Entropy_Config->Loaded_Config = eet_data_descriptor_decode(_entropy_config_loaded_edd, data, size_ret);
			/*Print them out..*/	
			/*entropy_config_mimes_print(_Entropy_Config->Loaded_Config);*/
		}
	}
	/*------*/

	/*Xlate misc list->hash*/
	/*Init the misc hash*/
	entropy_config_items_init();

	_Entropy_Config->Loaded_Config->Misc_Config = ecore_hash_new(ecore_str_hash,ecore_str_compare);
	
	for (l =_Entropy_Config->Loaded_Config->Misc_Config_Load  ; l; ) {
		item = l->data;
		ecore_hash_set(_Entropy_Config->Loaded_Config->Misc_Config, item->name, item);
		/*printf("Loaded misc '%s' -> '%s'\n", item->name, item->value);*/
		
		l=l->next;
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


void entropy_config_eet_config_save()
{
	Eet_File* conf_file;
	char* data;
	int size_ret;
	int ok;
	Entropy_Config* config;
	char* key;
	Ecore_List* keys;
	Entropy_Config_Item* item;

	config = entropy_core_get_core()->config;
	
	/*Xlate misc hash->list*/
	if (config->Loaded_Config->Misc_Config_Load)
		eina_list_free(config->Loaded_Config->Misc_Config_Load);

	config->Loaded_Config->Misc_Config_Load = NULL;
	keys = ecore_hash_keys(config->Loaded_Config->Misc_Config);
	while ((key = ecore_list_first_remove(keys))) {
		item = ecore_hash_get(config->Loaded_Config->Misc_Config,key);
		
		if (item && item->name && item->value) {
			config->Loaded_Config->Misc_Config_Load = 
				eina_list_append(config->Loaded_Config->Misc_Config_Load, item);
			printf("CONFIG SAVE: Wrote '%s' for '%s'\n", item->name, item->value);
		}
	}
	ecore_list_destroy(keys);
	

		conf_file = eet_open(config->config_dir_and_file_eet, EET_FILE_MODE_WRITE);
	if (conf_file) {
		char buf[10];
		/*Write the config version..*/
		snprintf(buf,10,"%d", ENTROPY_CONFIG_VERSION);
		printf("Writing config version '%s'..\n", buf);
		if (!(ok = eet_write(conf_file, "/config_version", buf, 10, 0))) {
			printf("Error writing data!\n");
		}
		
		if (config->Loaded_Config) {
			data = eet_data_descriptor_encode(_entropy_config_loaded_edd, config->Loaded_Config, &size_ret);
			if (data) {
				if ( !(ok = eet_write(conf_file, "/config_action", data, 
		       		size_ret, 0))) {
					printf("Error writing data!\n");
				
				}
				free(data);
			}
		}
		eet_close(conf_file);
	}
}
	

void entropy_config_destroy(Entropy_Config* config)
{
	entropy_config_eet_config_save();
	
	ecore_config_file_save(config->config_dir_and_file);

}

/*Config helper functions*/
void entropy_config_defaults_populate(Entropy_Config_Loaded* config)
{
		char* home;
		char buf[PATH_MAX];
	
		config->config_version = ENTROPY_CONFIG_VERSION;
		config->mime_bindings = eina_list_append(config->mime_bindings, 
				entropy_config_binding_new("image/jpeg", "JPEG Image",
					entropy_config_binding_action_new("Exhibit (Single File)", "exhibit", "\%pf"),
					entropy_config_binding_action_new("Exhibit (Directory)", "exhibit", "\%p"),
					NULL
					));
		config->mime_bindings = eina_list_append(config->mime_bindings, 
				entropy_config_binding_new("image/png", "PNG Image",
					entropy_config_binding_action_new("Exhibit (Single File)", "exhibit", "\%pf"),
					entropy_config_binding_action_new("Exhibit (Directory)", "exhibit", "\%p"),
					NULL
					));

		config->mime_bindings = eina_list_append(config->mime_bindings, 
				entropy_config_binding_new("image/gif", "GIF Image",
					entropy_config_binding_action_new("Exhibit (Single File)", "exhibit", "\%pf"),
					entropy_config_binding_action_new("Exhibit (Directory)", "exhibit", "\%p"),
					NULL
					));

		config->mime_bindings = eina_list_append(config->mime_bindings, 
				entropy_config_binding_new("text/csrc", "C Source File",
					entropy_config_binding_action_new("Gvim", "gvim", "\%pf"),
					NULL
					));

		config->mime_bindings = eina_list_append(config->mime_bindings, 
				entropy_config_binding_new("text/html", "HTML Document",
					entropy_config_binding_action_new("Firefox", "firefox", "\%pf"),
					NULL
					));

		config->mime_bindings = eina_list_append(config->mime_bindings, 
				entropy_config_binding_new("audio/x-mp3", "MP3 Audio",
					entropy_config_binding_action_new("Xmms", "xmms", "\%pf"),
					entropy_config_binding_action_new("Mpg123 (via eVFS)", "evfscat \%u | mpg123 -", ""),
					NULL
					));

		config->mime_bindings = eina_list_append(config->mime_bindings, 
				entropy_config_binding_new("video/x-ms-wmv", "WMV Video",
					entropy_config_binding_action_new("MPlayer", "mplayer", "\%pf"),
					entropy_config_binding_action_new("MPlayer (via evfs)", "evfscat \%u | mplayer -cache 4096 -", ""),
					entropy_config_binding_action_new("Xine", "xine", "%pf"),
					NULL
					));

		config->mime_bindings = eina_list_append(config->mime_bindings, 
				entropy_config_binding_new("video/mpeg", "MPEG Video",
					entropy_config_binding_action_new("MPlayer", "mplayer", "\%pf"),
					entropy_config_binding_action_new("MPlayer (via evfs)", "evfscat \%u | mplayer -cache 4096 -", ""),
					entropy_config_binding_action_new("Xine", "xine", "%pf"),
					NULL
					));

		config->mime_bindings = eina_list_append(config->mime_bindings, 
				entropy_config_binding_new("video/x-msvideo", "AVI Video",
					entropy_config_binding_action_new("MPlayer", "mplayer", "\%pf"),
					entropy_config_binding_action_new("MPlayer (via evfs)", "evfscat \%u | mplayer -cache 4096 -", ""),
					entropy_config_binding_action_new("Xine", "xine", "%pf"),
					NULL
					));


		/*Add some default structures*/
		home = entropy_core_home_dir_get (entropy_core_get_core());
		snprintf(buf, PATH_MAX, "file://%s", home);
		
		config->structures = eina_list_append(config->structures, 
				entropy_config_structure_new("Computer", "file:///")
				);

		config->structures = eina_list_append(config->structures, 
				entropy_config_structure_new("Home", buf)
				);
		config->structures = eina_list_append(config->structures, 
				entropy_config_structure_new("Virtual Folders", "vfolder:///")
				);

		/*Default settings*/
		entropy_config_misc_item_str_set("general.listviewer", "1",ENTROPY_CONFIG_LOC_LIST);
		entropy_config_misc_item_str_set("general.iconviewer", "0",ENTROPY_CONFIG_LOC_LIST);
		entropy_config_misc_item_str_set("general.treeviewer", "1",ENTROPY_CONFIG_LOC_LIST);
		entropy_config_misc_item_str_set("general.trackback", "1",ENTROPY_CONFIG_LOC_LIST);
		entropy_config_misc_item_str_set("general.presortfolders", "1",ENTROPY_CONFIG_LOC_LIST);
		entropy_config_misc_item_str_set("general.hiddenbackup", "0",ENTROPY_CONFIG_LOC_LIST);
		entropy_config_misc_item_str_set("general.iconsize", "48",ENTROPY_CONFIG_LOC_LIST);

}

Eina_List *
entropy_config_standard_structures_parse (entropy_gui_component_instance * instance,
				char *config)
{
	return _Entropy_Config->Loaded_Config->structures;
}

void entropy_config_standard_structure_remove(Entropy_Config_Structure* structure)
{
	_Entropy_Config->Loaded_Config->structures = eina_list_remove(_Entropy_Config->Loaded_Config->structures, 
			structure);
}

Entropy_Config_Structure*
entropy_config_standard_structures_add (char *name, char *uri)
{
	Entropy_Config_Structure* structure = entropy_config_structure_new(name,uri);
	
	_Entropy_Config->Loaded_Config->structures = eina_list_append(_Entropy_Config->Loaded_Config->structures,
	structure);

	return structure;
}

void
entropy_config_standard_structures_create ()
{
  entropy_core* core = entropy_core_get_core();
	
  char *eg = entropy_malloc(HEADER_CONFIG_MAX * sizeof (char));

  snprintf (eg, HEADER_CONFIG_MAX,
	    "Computer;file:///|Home;file://%s|Samba Example (Don't use!);smb://username:password@/test/machine/folder",
	    entropy_core_home_dir_get (core));

  //printf("Setting default config string..\n");
  entropy_config_str_set ("layout_ewl_simple", "structure_bar", eg);

  free (eg);
}

void entropy_config_items_init()
{
	if (_Entropy_Config->Loaded_Config->Misc_Config)
		ecore_hash_destroy(_Entropy_Config->Loaded_Config->Misc_Config);

	_Entropy_Config->Loaded_Config->Misc_Config = ecore_hash_new(ecore_str_hash, ecore_str_compare);
}

void entropy_config_misc_item_str_set(char* item, char* value, int loc)
{
	Entropy_Config_Item* c_item;
	void (*cb)();
	Ecore_List* keys;

	if (loc == ENTROPY_CONFIG_LOC_HASH) {
		if (!(c_item=ecore_hash_get(_Entropy_Config->Loaded_Config->Misc_Config, item))) {
			c_item = calloc(1,sizeof(Entropy_Config_Item));
			c_item->name = strdup(item);
			if (value) c_item->value = strdup(value);
			
			ecore_hash_set(_Entropy_Config->Loaded_Config->Misc_Config, c_item->name, c_item);
			/*printf ("hash Set '%s' -> '%s'\n",c_item->name, c_item->value);*/
		} else {
			  if (c_item->value) free(c_item->value);
			  if (value) 
				  c_item->value = strdup(value);	
			  else
				  c_item->value = NULL;

			  /*printf ("hash Set (existing) '%s' -> '%s'\n",c_item->name, c_item->value);*/
		}

		if (c_item) {
			if (c_item->callback_hash) {
				keys = ecore_hash_keys(c_item->callback_hash);
				ecore_list_first_goto(keys);
				while ((cb = ecore_list_first_remove(keys))) {
					(*cb)(item, ecore_hash_get(c_item->callback_hash, cb));
				}
				ecore_list_destroy(keys);
			}
		}
	} else if (loc == ENTROPY_CONFIG_LOC_LIST) {
		c_item = calloc(1,sizeof(Entropy_Config_Item));
		c_item->name = strdup(item);
		if (value) c_item->value = strdup(value);		
		
		_Entropy_Config->Loaded_Config->Misc_Config_Load = eina_list_append(
			_Entropy_Config->Loaded_Config->Misc_Config_Load, c_item);
		/*printf ("Set '%s' -> '%s'\n",item, value);*/
	}
}

void entropy_config_misc_callback_register(char* item, void (*cb)(char*, void*), void* data)
{
	Entropy_Config_Item* c_item;	

	c_item = ecore_hash_get(_Entropy_Config->Loaded_Config->Misc_Config, item);
	if (c_item) {
		if (!c_item->callback_hash)
			c_item->callback_hash = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);

		ecore_hash_set(c_item->callback_hash, cb, data);
	} else {
		printf("Register called for non existent misc config item: '%s'\n", item);
	}
}

char* entropy_config_misc_item_str_get(char* item)
{
	Entropy_Config_Item* res;
	
	/*printf("Looking for item for '%s'\n", item);*/
	
	res = (Entropy_Config_Item*)ecore_hash_get(_Entropy_Config->Loaded_Config->Misc_Config, item);
	if (res) {
		return res->value;
	} else
		return NULL;
}

int entropy_config_misc_is_set(char* item)
{
	char* res;

	res = entropy_config_misc_item_str_get(item);
	if (res && !strcmp(res, "1"))
		return 1;
	else 
		return 0;
}
