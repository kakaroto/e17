#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

#include <fcntl.h>

Ewl_Config *ewl_config = NULL;
Ewl_Config_Cache ewl_config_cache;

extern Ecore_List *ewl_embed_list;

static void ewl_config_load(Ewl_Config *cfg);
static void ewl_config_file_load(Ewl_Config *cfg, unsigned int is_system, 
							const char *file);
static void ewl_config_parse(Ewl_Config *cfg, Ecore_Hash *hash, char *data);

static Ecore_Hash *ewl_config_create_hash(void);
static void ewl_config_create_instance_hash(Ewl_Config *cfg);
static void ewl_config_create_system_hash(Ewl_Config *cfg);
static void ewl_config_create_user_hash(Ewl_Config *cfg);

static Ecore_Hash *ewl_config_set_hash_get(Ewl_Config *cfg, 
					Ewl_State_Type state);
static const char *ewl_config_get(Ewl_Config *cfg, const char *key);
static char *ewl_config_trim(char *v);

/**
 * @internal
 * @return Returns true on success, false on failure.
 * @brief Initialize the configuration system
 */
int
ewl_config_init(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_config = ewl_config_new("ewl");
	if (!ewl_config)
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @internal
 * @return Returns no value
 * @brief Shutdown the configuration system.
 */
void
ewl_config_shutdown(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	
	ewl_config_destroy(ewl_config);
	ewl_config = NULL;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @return Returns no value
 * @brief Initializes the debug system configuration
 */
void
ewl_config_cache_init(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_config_cache.enable = ewl_config_int_get(ewl_config, 
					EWL_CONFIG_DEBUG_ENABLE);
	ewl_config_cache.segv = ewl_config_int_get(ewl_config,
					EWL_CONFIG_DEBUG_SEGV);
	ewl_config_cache.backtrace = ewl_config_int_get(ewl_config,
					EWL_CONFIG_DEBUG_BACKTRACE);
	ewl_config_cache.evas_render = ewl_config_int_get(ewl_config,
					EWL_CONFIG_DEBUG_EVAS_RENDER);
	ewl_config_cache.gc_reap = ewl_config_int_get(ewl_config,
					EWL_CONFIG_DEBUG_GC_REAP);
	ewl_config_cache.level = ewl_config_int_get(ewl_config,
					EWL_CONFIG_DEBUG_LEVEL);

	ewl_config_cache.print_signals = ewl_config_int_get(ewl_config,
					EWL_CONFIG_THEME_PRINT_SIGNALS);
	ewl_config_cache.print_keys = ewl_config_int_get(ewl_config,
					EWL_CONFIG_THEME_PRINT_KEYS);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param app_name: The name of the app to open the config for
 * @return Returns the Ewl_Config struct for this app
 * @brief Creats the Ewl_Config file for the given application
 */
Ewl_Config *
ewl_config_new(const char *app_name)
{
	Ewl_Config *cfg;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("app_name", app_name, NULL);

	cfg = NEW(Ewl_Config, 1);
	cfg->app_name = strdup(app_name);

	ewl_config_load(cfg);

	/* XXX need to hookup to dbus here? */

	DRETURN_PTR(cfg, DLEVEL_STABLE);
}

/**
 * @param cfg: The Ewl_Config to destroy
 * @return Returns no value
 * @brief Destroys the given config structure
 */
void
ewl_config_destroy(Ewl_Config *cfg)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("cfg", cfg);
	
	if (cfg->data.user) ecore_hash_destroy(cfg->data.user);
	cfg->data.user = NULL;

	if (cfg->data.system) ecore_hash_destroy(cfg->data.system);
	cfg->data.system = NULL;

	if (cfg->data.instance) ecore_hash_destroy(cfg->data.instance);
	cfg->data.instance = NULL;

	IF_FREE(cfg->app_name);
	FREE(cfg);
	
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param cfg: The Ewl_Config to work with
 * @param k: the key to set in the configuration database
 * @param v: the string value that will be associated with the key
 * @return Returns no value.
 * @brief set the value of key to the specified string
 *
 * Sets the string value associated with the key @a k to @a v in the
 * configuration database.
 */
void
ewl_config_string_set(Ewl_Config *cfg, const char *k, const char *v,
						Ewl_State_Type state)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("k", k);

	ecore_hash_set(ewl_config_set_hash_get(cfg, state), 
						strdup(k), strdup(v));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param cfg: The Ewl_Config to work with
 * @param k: the key to search
 * @return Returns the found string value on success, NULL on failure.
 * @brief Retrieve string value associated with a key
 */
const char *
ewl_config_string_get(Ewl_Config *cfg, const char *k)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("k", k, NULL);

	DRETURN_PTR(ewl_config_get(cfg, k), DLEVEL_STABLE);
}

/**
 * @param cfg: The Ewl_Config to work with
 * @param k: the key to set in the configuration database
 * @param v: the integer value that will be associated with the key
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Set the value of key to the specified integer
 *
 * Sets the integer value associated with the key @a k to @a v in the
 * configuration database.
 */
void
ewl_config_int_set(Ewl_Config *cfg, const char *k, int v,
					Ewl_State_Type state)
{
	char buf[128];

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("k", k);

	snprintf(buf, sizeof(buf), "%d", v);
	ecore_hash_set(ewl_config_set_hash_get(cfg, state), 
					strdup(k), strdup(buf));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param cfg: The Ewl_Config to work with
 * @param k: the key to search
 * @return Returns the found integer value on success, 0 on failure.
 * @brief Retrieve integer value associated with a key
 */
int
ewl_config_int_get(Ewl_Config *cfg, const char *k)
{
	const char *val;
	int v = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("k", k, 0);

	val = ewl_config_get(cfg, k);
	if (val) v = atoi(val);

	DRETURN_INT(v, DLEVEL_STABLE);
}

/**
 * @param cfg: The Ewl_Config to work with
 * @param k: the key to set in the configuration database
 * @param v: the float value that will be associated with the key
 * @return Returns no value
 * @brief Set the value of key to the specified float
 *
 * Sets the float value associated with the key @a k to @a v in the
 * configuration database.
 */
void
ewl_config_float_set(Ewl_Config *cfg, const char *k, float v,
					Ewl_State_Type state)
{
	char buf[128];

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("k", k);

	snprintf(buf, sizeof(buf), "%f", v);
	ecore_hash_set(ewl_config_set_hash_get(cfg, state), 
					strdup(k), strdup(buf));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param cfg: The Ewl_Config to work with
 * @param k: the key to search
 * @return Returns the found float value on success, 0.0 on failure.
 * @brief Retrieve floating point value associated with a key
 */
float
ewl_config_float_get(Ewl_Config *cfg, const char *k)
{
	const char *val;
	float v = 0.0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("k", k, 0.0);

	val = ewl_config_get(cfg, k);
	if (val) v = atof(val);

	DRETURN_FLOAT(v, DLEVEL_STABLE);
}

static void
ewl_config_load(Ewl_Config *cfg)
{
	char *fname = NULL, *p;
	char cfg_filename[PATH_MAX];
	int is_ewl = FALSE;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("cfg", cfg);

	if (!strcmp(cfg->app_name, "ewl"))
		is_ewl = TRUE;

	/* convert all whitespace, / or \ characters to _ */
	fname = strdup(cfg->app_name);
	for(p = fname; *p != '\0'; p++)
	{
		if (isspace(*p) || (*p == '/') || (*p == '\\'))
			*p = '_';
	}

	/* load the global data */
	snprintf(cfg_filename, sizeof(cfg_filename),
			"%s/config/%s%s.cfg", PACKAGE_DATA_DIR, 
			(is_ewl ? "" : "apps/"), fname);
	ewl_config_file_load(cfg, TRUE, cfg_filename);

	/* load the user specific config data */
	snprintf(cfg_filename, sizeof(cfg_filename),
			"%s/.ewl/config/%s%s.cfg", 
			(getenv("HOME") ?  getenv("HOME") : "/tmp"), 
			(is_ewl ? "" : "apps/"),
			fname);
	ewl_config_file_load(cfg, FALSE, cfg_filename);

	/* update the evas info for the embeds */
	if (ewl_embed_list && !ecore_list_is_empty(ewl_embed_list)) 
	{
		Ewl_Embed *e;

		ecore_list_goto_first(ewl_embed_list);
		while ((e = ecore_list_next(ewl_embed_list)) != NULL) 
		{
			if (!e->evas) continue;

			evas_font_cache_flush(e->evas);
			evas_font_cache_set(e->evas, 
					ewl_config_int_get(ewl_config, 
						EWL_CONFIG_CACHE_EVAS_FONT));

			evas_image_cache_flush(e->evas);
			evas_image_cache_set(e->evas,
				 	ewl_config_int_get(ewl_config,
						EWL_CONFIG_CACHE_EVAS_IMAGE));
		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static Ecore_Hash *
ewl_config_create_hash(void)
{
	Ecore_Hash *hash;

	DENTER_FUNCTION(DLEVEL_STABLE);

	hash = ecore_hash_new(ecore_str_hash, ecore_str_compare);
	ecore_hash_set_free_key(hash, free);
	ecore_hash_set_free_value(hash, free);

	DRETURN_PTR(hash, DLEVEL_STABLE);
}

static void
ewl_config_create_user_hash(Ewl_Config *cfg)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("cfg", cfg);

	if (!cfg->data.user)
		cfg->data.user = ewl_config_create_hash();

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_config_create_system_hash(Ewl_Config *cfg)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("cfg", cfg);

	if (!cfg->data.system)
		cfg->data.system = ewl_config_create_hash();

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_config_create_instance_hash(Ewl_Config *cfg)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("cfg", cfg);

	if (!cfg->data.instance)
		cfg->data.instance = ewl_config_create_hash();

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static Ecore_Hash *
ewl_config_set_hash_get(Ewl_Config *cfg, Ewl_State_Type state)
{
	Ecore_Hash *hash = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("cfg", cfg, NULL);

	if (state == EWL_STATE_TRANSIENT)
	{
		ewl_config_create_instance_hash(cfg);
		hash = cfg->data.instance;
	}
	else
	{
		ewl_config_create_user_hash(cfg);
		hash = cfg->data.instance;
	}

	DRETURN_PTR(hash, DLEVEL_STABLE);
}

/* try to find the given key in our config. We check instance data first,
 * then user data, then system data. */
static const char *
ewl_config_get(Ewl_Config *cfg, const char *key)
{
	const char *val = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("cfg", cfg, NULL);
	DCHECK_PARAM_PTR_RET("key", key, NULL);

	if (cfg->data.instance 
			&& (val = ecore_hash_get(cfg->data.instance, key)))
	{
		/* we got our value, just NOP */
	}
	else if (cfg->data.user
			&& (val = ecore_hash_get(cfg->data.user, key)))
	{
		/* we got our value, just NOP */
	}
	else if (cfg->data.system
			&& (val = ecore_hash_get(cfg->data.system, key)))
	{
		/* we got our value, just NOP */
	}

	DRETURN_PTR(val, DLEVEL_STABLE);
}

/* open the given file and add it's key/value pairs to the config structure.
 * Overwrite any current values that are set.
 */
static void
ewl_config_file_load(Ewl_Config *cfg, unsigned int is_system, const char *file)
{
	Ecore_Hash *hash;
	int fd;
	char *data;
	struct flock fl;
	struct stat buf;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("cfg", cfg);
	DCHECK_PARAM_PTR("file", file);

	/* make sure the config file exists */
	if (!ecore_file_exists(file))
		DRETURN(DLEVEL_STABLE);

	fd = open(file, O_RDONLY, S_IRUSR);
	if (fd == -1) 
	{
		DWARNING("Unable to open cfg file %s\n", file);
		DRETURN(DLEVEL_STABLE);
	}

	fl.l_type = F_RDLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 0;

	if (fcntl(fd, F_SETLKW, &fl) == -1)
	{
		DWARNING("Unable to lock %s for read\n", file);
		DRETURN(DLEVEL_STABLE);
	}

	/* read the file into memory 
	 * 
	 * XXX we may want to do this in chunks as the config could be 
	 * large ... 
	 */
	stat(file, &buf);
	data = malloc(sizeof(char) * (buf.st_size + 1));
	read(fd, data, buf.st_size);
	data[buf.st_size] = '\0';

	/* release the lock as the file is in memory */
	fl.l_type = F_UNLCK;
	fcntl(fd, F_SETLK, &fl);
	close(fd);


	/* create the hash to store the values */
	if (is_system)
	{
		ewl_config_create_system_hash(cfg);
		hash = cfg->data.system;
	}
	else
	{
		ewl_config_create_user_hash(cfg);
		hash = cfg->data.user;
	}

	ewl_config_parse(cfg, hash, data);
	FREE(data);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_config_parse(Ewl_Config *cfg, Ecore_Hash *hash, char *data)
{
	char *start;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("cfg", cfg);
	DCHECK_PARAM_PTR("hash", hash);
	DCHECK_PARAM_PTR("data", data);

	start = data;
	while (start)
	{
		char *middle = NULL, *end, *key = NULL, *val = NULL;

		/* skip over blank space */
		while (isspace(*start) && (*start != '\0')) 
			start ++;
		if (*start == '\0') break;

		/* skip over comment lines */
		if (*start == '#')
		{
			while ((*start != '\n') && (*start != '\0')) 
				start ++;
			if (*start == '\0') break;

			start ++;

			continue;
		}

		/* at this point we should have an actual key/value pair */
		end = start;
		while ((*end != '\0') && (*end != '\n') && (*end != '\r'))
		{
			if (*end == '=')
			{
				middle = end;
				*middle = '\0';
			}
			end ++;
		}
		*end = '\0';

		if (start && middle && end)
		{
			key = strdup(start);
			key = ewl_config_trim(key);

			val = strdup(middle + 1);
			val = ewl_config_trim(val);

			ecore_hash_set(hash, key, val);
		}

		start = end + 1;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static char *
ewl_config_trim(char *v2)
{
	char *end, *old, *v;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("v2", v2, NULL);

	old = v2;
	v = v2;
	end = v + strlen(v);

	/* strip from beginning */
	while (isspace(*v) && (*v != '\0')) v++;
	while ((isspace(*end) || (*end == '\0')) && (end != v)) end --;
	*(++end) = '\0';

	v2 = strdup(v);
	FREE(old);

	DRETURN_PTR(v2, DLEVEL_STABLE);
}


