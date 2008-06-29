/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

#include <unistd.h>
#if HAVE_FCNTL_H
# include <fcntl.h>
#endif /* HAVE_FCNTL_H */
#include <libgen.h>

#ifdef HAVE_EVIL
# include <Evil.h>
#endif

#include <Evas.h>

/*
 * On Windows, if the file is not opened in binary mode,
 * read does not return the correct size, because of
 * CR / LF translation.
 */
#ifndef O_BINARY
# define O_BINARY 0
#endif

Ewl_Config *ewl_config = NULL;
Ewl_Config_Cache ewl_config_cache;

/*
 * @internal
 * @def EWL_CONFIG_KEY_NOMATCH
 * Marker used to set when a config key has no matching value
 */
#define EWL_CONFIG_KEY_NOMATCH ((char *)0xdeadbeef)

extern Ecore_List *ewl_embed_list;

static int ewl_config_file_read_lock(int fd, long size);
static int ewl_config_file_write_lock(int fd, long size);
static int ewl_config_file_unlock(int fd, long size);
static int ewl_config_load(Ewl_Config *cfg);
static int ewl_config_file_load(Ewl_Config *cfg, unsigned int is_system,
                                                        const char *file);
static void ewl_config_parse(Ewl_Config *cfg, Ecore_Hash *hash, char *data);

static Ecore_Hash *ewl_config_create_hash(void);
static void ewl_config_create_instance_hash(Ewl_Config *cfg);
static void ewl_config_create_system_hash(Ewl_Config *cfg);
static void ewl_config_create_user_hash(Ewl_Config *cfg);

static Ecore_Hash *ewl_config_set_hash_get(Ewl_Config *cfg,
                                        Ewl_State_Type state);
static const char *ewl_config_get(Ewl_Config *cfg, const char *key);
static const char * ewl_config_env_get(Ewl_Config *cfg, const char *key);
static char *ewl_config_trim(char *v);
static char *ewl_config_file_name_user_get(Ewl_Config *cfg);
static char *ewl_config_file_name_system_get(Ewl_Config *cfg);
static char *ewl_config_file_name_clean(Ewl_Config *cfg);
static int ewl_config_save(Ewl_Config *cfg, Ecore_Hash *hash,
                                                const char *file);

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

        if ((!ewl_config_string_get(ewl_config, EWL_CONFIG_THEME_NAME))
                        || (!ewl_config_string_get(ewl_config,
                                                EWL_CONFIG_ENGINE_NAME)))
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
        DCHECK_PARAM_PTR_RET(app_name, NULL);

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
        DCHECK_PARAM_PTR(cfg);

        IF_FREE_HASH(cfg->data.user);
        IF_FREE_HASH(cfg->data.system);
        IF_FREE_HASH(cfg->data.instance);

        IF_FREE(cfg->app_name);
        FREE(cfg);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param cfg: The Ewl_Config to work with
 * @param k: the key to set in the configuration database
 * @param v: the string value that will be associated with the key
 * @param state: The state of the key being set, persistent or transient
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
        DCHECK_PARAM_PTR(k);

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
        DCHECK_PARAM_PTR_RET(k, NULL);

        DRETURN_PTR(ewl_config_get(cfg, k), DLEVEL_STABLE);
}

/**
 * @param cfg: The Ewl_Config to work with
 * @param k: the key to set in the configuration database
 * @param v: the integer value that will be associated with the key
 * @param state: The state of the key being set, persistent or transient
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
        DCHECK_PARAM_PTR(k);

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
        DCHECK_PARAM_PTR_RET(k, 0);

        val = ewl_config_get(cfg, k);
        if (val) v = atoi(val);

        DRETURN_INT(v, DLEVEL_STABLE);
}

/**
 * @param cfg: The Ewl_Config to work with
 * @param k: the key to set in the configuration database
 * @param v: the float value that will be associated with the key
 * @param state: The state of the key being set, persistent or transient
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
        DCHECK_PARAM_PTR(k);

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
        DCHECK_PARAM_PTR_RET(k, 0.0);

        val = ewl_config_get(cfg, k);
        if (val) v = atof(val);

        DRETURN_FLOAT(v, DLEVEL_STABLE);
}

/**
 * @param cfg: The Ewl_Config to work with
 * @param k: the key to set in the configuration database
 * @param r: the red value that will be associated with the key
 * @param g: the green value that will be associated with the key
 * @param b: the blue value that will be associated with the key
 * @param a: the alpha value that will be associated with the key
 * @param state: The state of the key being set, persistent or transient
 * @return Returns no value
 * @brief Set the value of key to the specified color
 *
 * Sets the color value associated with the key @a k to @a v in the
 * configuration database.
 */
void
ewl_config_color_set(Ewl_Config *cfg, const char *k, int r, int g,
                                        int b, int a,
                                        Ewl_State_Type state)
{
        char buf[128];

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(k);

        snprintf(buf, sizeof(buf), "%i %i %i %i", r, g, b, a);
        ecore_hash_set(ewl_config_set_hash_get(cfg, state),
                                        strdup(k), strdup(buf));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param cfg: The Ewl_Config to work with
 * @param k: the key to search
 * @param r: the red value that is associated with the key
 * @param g: the green value that is associated with the key
 * @param b: the blue value that is associated with the key
 * @param a: the alpha value that is associated with the key
 * @return Returns no value
 * @brief Retrieve color associated with a key
 */
void
ewl_config_color_get(Ewl_Config *cfg, const char *k, int *r, int *g,
                                                int *b, int *a)
{
        const char *val;
        int tmp_r = 0, tmp_g = 0, tmp_b = 0, tmp_a = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(k);

        val = ewl_config_get(cfg, k);
        if (val) sscanf(val, "%i %i %i %i", &tmp_r, &tmp_g, &tmp_b, &tmp_a);

        if (r) *r = tmp_r;
        if (g) *g = tmp_g;
        if (b) *b = tmp_b;
        if (a) *a = tmp_a;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param cfg: The Ewl_Config to work with
 * @param k: The key to remove
 * @return Returns no value
 * @brief Removes key from configuration
 */
void
ewl_config_key_remove(Ewl_Config *cfg, const char *k)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(cfg);
        DCHECK_PARAM_PTR(k);

        ewl_config_instance_key_remove(cfg, k);
        ewl_config_system_key_remove(cfg, k);
        ewl_config_user_key_remove(cfg, k);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param cfg: The Ewl_Config to work with
 * @param k: The key to remove
 * @return Returns no value
 * @brief Removes the given key from the instance configuration data
 */
void
ewl_config_instance_key_remove(Ewl_Config *cfg, const char *k)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(cfg);
        DCHECK_PARAM_PTR(k);

        if (cfg->data.instance && (ecore_hash_get(cfg->data.instance, k)))
                ecore_hash_remove(cfg->data.instance, k);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param cfg: The Ewl_Config to work with
 * @param k: The key to remove
 * @return Returns no value
 * @brief Removes the given key from the system configuration data
 */
void
ewl_config_system_key_remove(Ewl_Config *cfg, const char *k)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(cfg);
        DCHECK_PARAM_PTR(k);

        if (cfg->data.system && (ecore_hash_get(cfg->data.system, k)))
                ecore_hash_remove(cfg->data.system, k);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param cfg: The Ewl_Config to work with
 * @param k: The key to remove
 * @return Returns no value
 * @brief Removes the given key from the user configuration data
 */
void
ewl_config_user_key_remove(Ewl_Config *cfg, const char *k)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(cfg);
        DCHECK_PARAM_PTR(k);

        if (cfg->data.user && (ecore_hash_get(cfg->data.user, k)))
                ecore_hash_remove(cfg->data.user, k);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param cfg: The Ewl_Config to work with
 * @return Returns TRUE if the user can write to the system conf file, FALSE
 * otherwise
 * @brief Determines if the user can write the system config file
 */
int
ewl_config_can_save_system(Ewl_Config *cfg)
{
        char *fname;
        int ret;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(cfg, FALSE);

        fname = ewl_config_file_name_system_get(cfg);
        ret = access(fname, W_OK);
        FREE(fname);

        DRETURN_INT((!ret ? TRUE : FALSE), DLEVEL_STABLE);
}

/**
 * @param cfg: The Ewl_Config to work with
 * @return Returns TRUE on success or FALSE on failure
 * @brief Writes out the user config to the users config file
 */
int
ewl_config_user_save(Ewl_Config *cfg)
{
        char *fname;
        int ret;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(cfg, FALSE);

        fname = ewl_config_file_name_user_get(cfg);
        ret = ewl_config_save(cfg, cfg->data.user, fname);
        FREE(fname);

        DRETURN_INT(ret, DLEVEL_STABLE);
}

/**
 * @param cfg: The Ewl_Config to save
 * @return Returns TRUE on success, FALSE on failure
 * @brief Writes out the system and user data to the system config file
 */
int
ewl_config_system_save(Ewl_Config *cfg)
{
        char *fname, *key;
        int ret;
        Ecore_Hash *hash;
        Ecore_List *keys;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(cfg, FALSE);

        if (!ewl_config_can_save_system(cfg))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        hash = ecore_hash_new(ecore_str_hash, ecore_str_compare);

        /* add all the system data */
        keys = ecore_hash_keys(cfg->data.system);
        ecore_list_first_goto(keys);
        while ((key = ecore_list_next(keys)))
        {
                ecore_hash_set(hash, key,
                                ecore_hash_get(cfg->data.system, key));
        }
        IF_FREE_LIST(keys);

        /* set the user data over top */
        keys = ecore_hash_keys(cfg->data.user);
        ecore_list_first_goto(keys);
        while ((key = ecore_list_next(keys)))
        {
                ecore_hash_set(hash, key,
                                ecore_hash_get(cfg->data.user, key));
        }
        IF_FREE_LIST(keys);

        fname = ewl_config_file_name_system_get(cfg);
        ret = ewl_config_save(cfg, hash, fname);
        FREE(fname);

        IF_FREE_HASH(hash);

        DRETURN_INT(ret, DLEVEL_STABLE);
}

static int
ewl_config_file_read_lock(int fd, long size)
{
#if defined(F_SETLKW)
        struct flock fl;

        fl.l_type = F_RDLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start = 0;
        fl.l_len = 0;

        return (fcntl(fd, F_SETLKW, &fl) == 0);
#else
# if HAVE__LOCKING
        return (_locking(fd, _LK_LOCK, size) == 0);
# endif /* HAVE__LOCKING */
#endif /* !defined(F_SETLKW) */
        size = size;
}

static int
ewl_config_file_write_lock(int fd, long size)
{
#if defined(F_SETLKW)
        struct flock fl;

        fl.l_type = F_WRLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start = 0;
        fl.l_len = 0;

        return (fcntl(fd, F_SETLKW, &fl) == 0);
#else
# if HAVE__LOCKING
        return (_locking(fd, _LK_LOCK, size) == 0);
# endif /* HAVE__LOCKING */
#endif /* !defined(F_SETLKW) */
        size = size;
}

static int
ewl_config_file_unlock(int fd, long size)
{
#if defined(F_SETLKW)
        struct flock fl;

        fl.l_type = F_UNLCK;
        fl.l_whence = SEEK_SET;
        fl.l_start = 0;
        fl.l_len = 0;

        return (fcntl(fd, F_SETLK, &fl) == 0);
#else
# if HAVE__LOCKING
        return (_locking(fd, _LK_UNLCK, size) == 0);
# endif /* HAVE__LOCKING */
#endif /* !defined(F_SETLKW) */
        size = size;
}

static int
ewl_config_save(Ewl_Config *cfg, Ecore_Hash *hash, const char *file)
{
        Ecore_List *keys;
        char *key, data[512], *path;
        long size;
        int fd;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(cfg, FALSE);
        DCHECK_PARAM_PTR_RET(file, FALSE);

        /* make sure the config directory exists */
        path = strdup(file);
        key = dirname(path);
        if (!ecore_file_exists(key) && !ecore_file_mkpath(key))
        {
                DWARNING("Unable to create %s directory path.", key);
                DRETURN_INT(FALSE, DLEVEL_STABLE);
        }
        FREE(path);

        /* if the hash doesn't exist then treat it is empty */
        if (!hash)
                DRETURN_INT(TRUE, DLEVEL_STABLE);

        fd = open(file, O_CREAT | O_WRONLY | O_TRUNC | O_BINARY,
                        S_IRWXU | S_IRGRP | S_IROTH);
        if (fd == -1)
        {
                DWARNING("Unable to open cfg file %s.", file);
                DRETURN_INT(FALSE, DLEVEL_STABLE);
        }

        size = ecore_file_size(file);

        if (!ewl_config_file_write_lock(fd, size))
        {
                DWARNING("Unable to lock %s for write.", file);
                close(fd);

                DRETURN_INT(FALSE, DLEVEL_STABLE);
        }

        keys = ecore_hash_keys(hash);
        ecore_list_first_goto(keys);
        while ((key = ecore_list_next(keys)))
        {
                int len;

                len = snprintf(data, sizeof(data), "%s = %s\n", key,
                                        (char *)ecore_hash_get(hash, key));

                write(fd, data, len);
        }

        /* release the lock */
        ewl_config_file_unlock(fd, size);
        close(fd);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static char *
ewl_config_file_name_clean(Ewl_Config *cfg)
{
        char *fname = NULL, *p;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(cfg, NULL);

        /* convert all whitespace, / or \ characters to _ */
        fname = strdup(cfg->app_name);
        for(p = fname; *p != '\0'; p++)
        {
                if (isspace(*p) || (*p == '/') || (*p == '\\'))
                        *p = '_';
        }

        DRETURN_PTR(fname, DLEVEL_STABLE);
}

static char *
ewl_config_file_name_build_get(Ewl_Config *cfg)
{
        char cfg_filename[PATH_MAX], *fname;
        int is_ewl = FALSE;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(cfg, NULL);

        if (!getenv("srcdir"))
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!strcmp(cfg->app_name, "ewl"))
                is_ewl = TRUE;

        fname = ewl_config_file_name_clean(cfg);
        snprintf(cfg_filename, sizeof(cfg_filename),
                        "%s/../../data/config/%s%s.cfg", getenv("srcdir"),
                        (is_ewl ? "" : "apps/"), fname);

        FREE(fname);

        DRETURN_PTR(strdup(cfg_filename), DLEVEL_STABLE);
}

static char *
ewl_config_file_name_system_get(Ewl_Config *cfg)
{
        char cfg_filename[PATH_MAX], *fname;
        int is_ewl = FALSE;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(cfg, NULL);

        if (!strcmp(cfg->app_name, "ewl"))
                is_ewl = TRUE;

        fname = ewl_config_file_name_clean(cfg);
        snprintf(cfg_filename, sizeof(cfg_filename),
                        "%s/ewl/%s%s.cfg", PACKAGE_SYSCONF_DIR,
                        (is_ewl ? "" : "apps/"), fname);

        FREE(fname);

        DRETURN_PTR(strdup(cfg_filename), DLEVEL_STABLE);
}

static char *
ewl_config_file_name_user_get(Ewl_Config *cfg)
{
        char cfg_filename[PATH_MAX], *fname;
        int is_ewl = FALSE;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(cfg, NULL);

        if (!strcmp(cfg->app_name, "ewl"))
                is_ewl = TRUE;

        fname = ewl_config_file_name_clean(cfg);
        snprintf(cfg_filename, sizeof(cfg_filename),
                        "%s/.ewl/config/%s%s.cfg",
                        (getenv("HOME") ?  getenv("HOME") : "/tmp"),
                        (is_ewl ? "" : "apps/"),
                        fname);

        FREE(fname);

        DRETURN_PTR(strdup(cfg_filename), DLEVEL_STABLE);
}

static int
ewl_config_load(Ewl_Config *cfg)
{
        char *fname = NULL;
        int sys_ret, user_ret;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(cfg, FALSE);

        /*
         * Attempt to load a build relative config file first. This allows for
         * testing without installing.
         */
        fname = ewl_config_file_name_build_get(cfg);
        if (!fname) fname = ewl_config_file_name_system_get(cfg);
        sys_ret = ewl_config_file_load(cfg, TRUE, fname);
        FREE(fname);

        fname = ewl_config_file_name_user_get(cfg);
        user_ret = ewl_config_file_load(cfg, FALSE, fname);
        FREE(fname);

        /* if we failed to load both config files we're in trouble */
        if (!sys_ret && !user_ret)
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        /* XXX deal with the colour classes */

        /* XXX not sure if this is in the right spot ... */
        /* XXX This is very much the wrong spot for this. This will happen
         * on all config inits, we just want this to happen on EWL config
         * init ... */
        /* update the evas info for the embeds */
        if (ewl_embed_list && !ecore_list_empty_is(ewl_embed_list))
        {
                Ewl_Embed *e;

                ecore_list_first_goto(ewl_embed_list);
                while ((e = ecore_list_next(ewl_embed_list)) != NULL)
                {
                        if (!e->canvas) continue;

                        evas_font_cache_flush(e->canvas);
                        evas_font_cache_set(e->canvas,
                                        ewl_config_int_get(ewl_config,
                                                EWL_CONFIG_CACHE_EVAS_FONT));

                        evas_image_cache_flush(e->canvas);
                        evas_image_cache_set(e->canvas,
                                         ewl_config_int_get(ewl_config,
                                                EWL_CONFIG_CACHE_EVAS_IMAGE));
                }
        }

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static Ecore_Hash *
ewl_config_create_hash(void)
{
        Ecore_Hash *hash;

        DENTER_FUNCTION(DLEVEL_STABLE);

        hash = ecore_hash_new(ecore_str_hash, ecore_str_compare);
        ecore_hash_free_key_cb_set(hash, free);
        ecore_hash_free_value_cb_set(hash, free);

        DRETURN_PTR(hash, DLEVEL_STABLE);
}

static void
ewl_config_create_user_hash(Ewl_Config *cfg)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(cfg);

        if (!cfg->data.user)
                cfg->data.user = ewl_config_create_hash();

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_config_create_system_hash(Ewl_Config *cfg)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(cfg);

        if (!cfg->data.system)
                cfg->data.system = ewl_config_create_hash();

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_config_create_instance_hash(Ewl_Config *cfg)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(cfg);

        if (!cfg->data.instance)
                cfg->data.instance = ewl_config_create_hash();

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static Ecore_Hash *
ewl_config_set_hash_get(Ewl_Config *cfg, Ewl_State_Type state)
{
        Ecore_Hash *hash = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(cfg, NULL);

        if (state == EWL_STATE_TRANSIENT)
        {
                ewl_config_create_instance_hash(cfg);
                hash = cfg->data.instance;
        }
        else
        {
                ewl_config_create_user_hash(cfg);
                hash = cfg->data.user;
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
        DCHECK_PARAM_PTR_RET(cfg, NULL);
        DCHECK_PARAM_PTR_RET(key, NULL);

        if (cfg->data.instance
                        && (val = ecore_hash_get(cfg->data.instance, key)))
        {
                /* we got our value, just NOP */
        }
        else if ((val = ewl_config_env_get(cfg, key)))
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

/* Look up hash values in the environment and cache the results. */
static const char *
ewl_config_env_get(Ewl_Config *cfg, const char *key)
{
        int kpos, epos = 0;
        const char *val = NULL;
        char var_name[PATH_MAX];

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(cfg, NULL);
        DCHECK_PARAM_PTR_RET(key, NULL);

        /* Skip all leading non-alphanumeric characters */
        for (kpos = 0; (key[kpos] && !isalnum(key[kpos])); kpos++);

        /*
         * Copy the config variable name in all upper case and substitute _ for
         * any non-alphanumeric characters.
         */
        while (epos < PATH_MAX && key[kpos])
        {
                if (isalnum(key[kpos]))
                        var_name[epos] = toupper(key[kpos]);
                else
                        var_name[epos] = '_';
                epos++;
                kpos++;
        }

        var_name[epos] = '\0';

        if (var_name[0]) val = getenv(var_name);

        DRETURN_PTR(val, DLEVEL_STABLE);
}

/* open the given file and add it's key/value pairs to the config structure.
 * Overwrite any current values that are set.
 */
static int
ewl_config_file_load(Ewl_Config *cfg, unsigned int is_system, const char *file)
{
        Ecore_Hash *hash;
        int fd;
        long size;
        char *data;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(cfg, FALSE);
        DCHECK_PARAM_PTR_RET(file, FALSE);

        /* make sure the config file exists */
        if (!ecore_file_exists(file))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        fd = open(file, O_RDONLY | O_BINARY, S_IRUSR);
        if (fd == -1)
        {
                DWARNING("Unable to open cfg file %s.", file);
                DRETURN_INT(FALSE, DLEVEL_STABLE);
        }

        size = ecore_file_size(file);

        if (!ewl_config_file_read_lock(fd, size))
        {
                DWARNING("Unable to lock %s for read.", file);

                close(fd);
                DRETURN_INT(FALSE, DLEVEL_STABLE);
        }

        /* read the file into memory
         *
         * XXX we may want to do this in chunks as the config could be
         * large ...
         */
        data = malloc(sizeof(char) * (size + 1));
        read(fd, data, size);
        data[size] = '\0';

        /* release the lock as the file is in memory */
        ewl_config_file_unlock(fd, size);
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

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static void
ewl_config_parse(Ewl_Config *cfg, Ecore_Hash *hash, char *data)
{
        char *start;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(cfg);
        DCHECK_PARAM_PTR(hash);
        DCHECK_PARAM_PTR(data);

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
        DCHECK_PARAM_PTR_RET(v2, NULL);

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


