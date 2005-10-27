#include "entropy.h"
#include "Ecore_Config.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>



entropy_config* entropy_config_init(entropy_core* core) {
	entropy_config* config = entropy_malloc(sizeof(entropy_config));
	struct stat config_dir_stat;
	int i;

	i = strlen(entropy_core_home_dir_get(core) + strlen("/.e/apps/entropy"));
	config->config_dir = entropy_malloc(i * sizeof(char));
	snprintf(config->config_dir, i, "%s/%s", entropy_core_home_dir_get(core), "/.e/apps/entropy");

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
