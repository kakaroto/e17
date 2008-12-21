/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

#include <Edje.h>

extern Ecore_List *ewl_embed_list;
static char *ewl_theme_path = NULL;

static Ecore_List *ewl_theme_font_paths = NULL;
static Ecore_Hash *ewl_theme_def_data = NULL;

static void ewl_theme_font_path_init(void);
static char *ewl_theme_path_find(const char *name);
static void ewl_theme_data_free(void *data);

#define DEFAULT_THEME "e17"

/**
 * @internal
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Initialize the themeing  system
 *
 * Initializes the data structures involved with theme handling. Involves
 * finding the specified theme file. This is called by ewl_init, and is not
 * necessary for the end programmer to call.
 */
int
ewl_theme_init(void)
{
        const char *theme;

        DENTER_FUNCTION(DLEVEL_STABLE);

        theme = ewl_config_string_get(ewl_config, EWL_CONFIG_THEME_NAME);
        if (!ewl_theme_theme_set(theme))
        {
                if ((!strcmp(theme, DEFAULT_THEME)) || 
                                (!ewl_theme_theme_set(DEFAULT_THEME)))
                {
                        DWARNING("No usable theme found, exiting.");
                        DRETURN_INT(FALSE, DLEVEL_STABLE);
                }
        }

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @internal
 * @return Returns no value.
 * @brief Shutdown the EWL themeing subsystem
 */
void
ewl_theme_shutdown(void)
{
        DENTER_FUNCTION(DLEVEL_STABLE);

        IF_FREE_LIST(ewl_theme_font_paths);
        IF_FREE_HASH(ewl_theme_def_data);

        FREE(ewl_theme_path);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param theme_name: The name of the theme to set
 * @return Returns TRUE if successfully set, FALSE otherwise
 * @brief Sets the current theme to use.
 * Set the current theme
 */
int
ewl_theme_theme_set(const char *theme_name)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(theme_name, FALSE);

        /* Allocate and clear the default theme */
        IF_FREE_HASH(ewl_theme_def_data);

        ewl_theme_def_data = ecore_hash_new(ecore_str_hash, ecore_str_compare);
        if (!ewl_theme_def_data)
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ecore_hash_free_key_cb_set(ewl_theme_def_data, ewl_theme_data_free);
        ecore_hash_free_value_cb_set(ewl_theme_def_data, ewl_theme_data_free);

        /* clean up the font path list */
        if (ewl_theme_font_paths)
                ecore_list_clear(ewl_theme_font_paths);

        /* get the new theme path and setup the font path */
        IF_FREE(ewl_theme_path);
        ewl_theme_path = ewl_theme_path_find(theme_name);
        if (!ewl_theme_path) DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_theme_font_path_init();

        /* Hide all embeds. If the embed was previously shown we re-show it
         * again. This should cause everything to reset it's theme values to the
         * new values */
        ecore_list_first_goto(ewl_embed_list);
        while ((w = ecore_list_next(ewl_embed_list)))
        {
                int vis;
                int vis2;

                vis = REALIZED(w);
                vis2 = VISIBLE(w);

                ewl_widget_hide(w);
                ewl_widget_unrealize(w);
                if (vis) ewl_widget_realize(w);
                if (vis2) ewl_widget_show(w);
        }

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: the widget to initialize theme information
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Initialize a widgets theme information to the default
 *
 * Sets the widget @a w's theme information to the default values.
 */
int
ewl_theme_widget_init(Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(w, FALSE);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, FALSE);

        w->theme = NULL;

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: the widget to remove theme information
 * @return Returns no value.
 * @brief remove the theme information from a widget
 *
 * Removes and frees the theme information from the widget @a w.
 */
void
ewl_theme_widget_shutdown(Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        /*
         * We only want to destroy the hash if its not ewl_theme_def_data
         * We destroy ewl_theme_def_data from else where..
         */
        if (w->theme && w->theme != ewl_theme_def_data)
                IF_FREE_HASH(w->theme);

        w->theme = NULL;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @brief Return the path of the current theme
 * @return Returns the current theme path on success, NULL on failure
 */
const char *
ewl_theme_path_get(void)
{
        DENTER_FUNCTION(DLEVEL_STABLE);

        DRETURN_PTR(ewl_theme_path, DLEVEL_STABLE);
}

/**
 * @return Returns the font path of widget @a w on success, NULL on failure.
 * @brief retrieve the path of a widgets theme's fonts
 */
Ecore_List *
ewl_theme_font_path_get(void)
{
        DENTER_FUNCTION(DLEVEL_STABLE);

        DRETURN_PTR(ewl_theme_font_paths, DLEVEL_STABLE);
}

/**
 * @param path: the font to add to the search path
 * @return Returns no value.
 * @brief Add a specified path to the font search path
 *
 * Duplicates the string pointed to by @a path and adds it to
 * the list of paths that are searched for fonts.
 */
void
ewl_theme_font_path_add(const char *path)
{
        char *temp;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(path);

        temp = strdup(path);
        ecore_list_append(ewl_theme_font_paths, temp);
        ewl_embed_font_path_add(temp);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to search
 * @param k: the image to search for
 * @return Returns the path of image key @a k on success, NULL on failure.
 * @brief retrieve the path to an image from a widgets theme
 */
char *
ewl_theme_image_get(Ewl_Widget *w, const char *k)
{
        const char *data;
        char *ret;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(w, NULL);
        DCHECK_PARAM_PTR_RET(k, NULL);
        DCHECK_TYPE_RET(w, EWL_WIDGET_TYPE, NULL);

        data = ewl_theme_data_str_get(w, k);
        if (!data)
        {
                if (!ewl_theme_path)
                {
                        DRETURN_PTR(NULL, DLEVEL_STABLE);
                }
                else
                        data = ewl_theme_path;
        }

        /*
         * Convert a relative path to an absolute path
         */
#ifndef _WIN32
        if (*data != '/') {
#else
        if (*(data + 1) != ':') {
#endif /* _WIN32 */
                char path[PATH_MAX];

                snprintf(path, PATH_MAX, "%s/%s", ewl_theme_path, data);
                ret = strdup(path);
        }
        else
                ret = strdup(data);

        DRETURN_PTR(ret, DLEVEL_STABLE);
}

static void
ewl_theme_lookup_cache(Ecore_Hash *cache, const char *k, const char *v)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(cache);
        DCHECK_PARAM_PTR(k);

        /*
         * The v value should already be a string instance at this point so we
         * can set the value directly whether its a match or no match.
         */
        ecore_hash_set(cache, (void *)ecore_string_instance(k), (void *)v);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static const char *
ewl_theme_lookup_key(Ecore_Hash *cache, const char *path, const char *k)
{
        const char *ret;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(cache, NULL);
        DCHECK_PARAM_PTR_RET(k, NULL);

        ret = ecore_hash_get(cache, k);
        if (!ret) {
                char *tmp = NULL;

                /*
                 * Resort to looking in the edje.
                 */
                if (path)
                        tmp = edje_file_data_get(path, k);

                /*
                 * Get a string instance for the value so we can treat the
                 * return type consistently and not leak memory.
                 */
                if (tmp) {
                        ret = ecore_string_instance(tmp);
                        FREE(tmp);
                }
        }
        else
                ret = ecore_string_instance(ret);

        DRETURN_PTR(ret, DLEVEL_STABLE);
}

/**
 * @param w: the widget to reset theme settings
 * @return Returns no value.
 * @brief Reset the theme settings for the widget @a w.
 */
void
ewl_theme_data_reset(Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        IF_FREE_HASH(w->theme);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to search
 * @param k: the key to search for
 * @return Returns the string associated with @a k on success, NULL on failure.
 * @brief Retrieve an string value from a widgets theme
 */
const char *
ewl_theme_data_str_get(Ewl_Widget *w, const char *k)
{
        Ecore_Hash *cache = NULL;
        const char *ret = NULL;
        char *temp = NULL;
        char key[PATH_MAX];

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(k, NULL);

        /*
         * Use the widget's appearance string to build a relative theme key.
         */
        if (w) {
                int len;
                char *tmp;

                len = ewl_widget_appearance_path_size_get(w);
                tmp = alloca(len);
                if (tmp) {
                        int used;

                        used = ewl_widget_appearance_path_copy(w, tmp, len);
                        if (used == (len - 1)) {
                                used = ecore_strlcpy(key, tmp, PATH_MAX);
                                if (used < PATH_MAX) {
                                        *(key + used) = '/';
                                        used++;
                                }

                                ecore_strlcpy(key + used, k, PATH_MAX - used);
                        }
                }
                else {
                        len += strlen(k) + 1;
                        ecore_strlcpy(key, k, len);
                }

        } else
                snprintf(key, PATH_MAX, "%s", k);

        if (ewl_config_cache.print_keys)
                printf("%s\n", key);

        /*
         * Loop up the widget heirarchy looking for this key.
         */
        temp = key;
        while (w && temp && !ret) {

                /*
                 * Find a widget with theme data.
                 */
                while (w && !w->theme)
                        w = w->parent;

                if (w && w->theme)
                        ret = ewl_theme_lookup_key(w->theme, w->theme_path, temp);

                if (ret) {
                        cache = w->theme;
                        break;
                }

                temp++;
                temp = strchr(temp, '/');
                if (!temp && w && w->parent) {
                        temp = key;
                        w = w->parent;
                }
        }

        /*
         * No key found in widgets, look in the default theme and edje.
         */
        if (!ret) {
                temp = key;
                while (temp && !ret) {
                        ret = ewl_theme_lookup_key(ewl_theme_def_data,
                                        ewl_theme_path, temp);
                        if (ret) {
                                cache = ewl_theme_def_data;
                                break;
                        }

                        temp++;
                        temp = strchr(temp, '/');
                }

        }

        if (!cache)
                cache = ewl_theme_def_data;

        /*
         * Mark all keys matched and unmatched in the cache.
         */
        ewl_theme_lookup_cache(cache, key, ret);

        /*
         * Fixup unmatched keys in the cache.
         */
        if (ret == EWL_THEME_KEY_NOMATCH)
                ret = NULL;

        DRETURN_PTR(ret, DLEVEL_STABLE);
}

/**
 * @param w: the widget to search
 * @param k: the key to search for
 * @return Returns the integer associated with key @a k on success, 0 on failure.
 * @brief Retrieve an integer value from a widgets theme
 */
int
ewl_theme_data_int_get(Ewl_Widget *w, const char *k)
{
        const char *temp;
        int ret = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(k, FALSE);

        temp = ewl_theme_data_str_get(w, k);
        if (temp) ret = atoi(temp);

        DRETURN_INT(ret, DLEVEL_STABLE);
}

/**
 * @param w: the widget to change theme data
 * @param k: the key to change
 * @param v: the data to assign to the key
 * @return Returns no value.
 * @brief Store data into a widgets theme
 *
 * Changes the theme data in widget @a w so that key @a k now is
 * associated with value @a v.
 */
void
ewl_theme_data_str_set(Ewl_Widget *w, const char *k, const char *v)
{
        int was_realized = FALSE;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(k);

        if (REALIZED(w))
        {
                ewl_widget_unrealize(w);
                was_realized = TRUE;
        }

        if (!w->theme || w->theme == ewl_theme_def_data) {
                w->theme = ecore_hash_new(ecore_str_hash, ecore_str_compare);

                ecore_hash_free_key_cb_set(w->theme, ewl_theme_data_free);
                ecore_hash_free_value_cb_set(w->theme, ewl_theme_data_free);
        }

        if (v && v != EWL_THEME_KEY_NOMATCH)
                ecore_hash_set(w->theme, (void *)ecore_string_instance(k),
                                                (void *)ecore_string_instance(v));
        else
                ecore_hash_set(w->theme, (void *)ecore_string_instance(k),
                                                EWL_THEME_KEY_NOMATCH);

        if (was_realized)
                ewl_widget_realize(w);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param w: the widget to change theme data
 * @param k: the key to change
 * @param v: the data to assign to the key
 * @return Returns no value.
 * @brief Store data into a widgets theme
 *
 * Changes the theme data in widget @a w so that key @a k now is
 * associated with value @a v.
 */
void
ewl_theme_data_int_set(Ewl_Widget *w, const char *k, int v)
{
        char value[16];

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(k);

        snprintf(value, 16, "%d", v);
        ewl_theme_data_str_set(w, k, value);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Private function for finding the theme path given a theme name, If no theme
 * of name is found we will return null.
 */
static char *
ewl_theme_path_find(const char *name)
{
        struct stat st;
        char *theme_found_path = NULL;
        char theme_tmp_path[PATH_MAX];
        char *home;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(name, NULL);

        /*
         * Get the users home directory. This environment variable should
         * always be set.
         */
        home = getenv("HOME");
        if (!home) {
                DERROR("Environment variable HOME not defined\n"
                       "Try export HOME=/home/user in a bash like environemnt or\n"
                       "setenv HOME=/home/user in a csh like environment.\n");
        }

        /*
         * Build a path to the theme if it is the users home dir and use it if
         * available.
         */
        if (home) {
                snprintf(theme_tmp_path, PATH_MAX, "%s/.ewl/themes/%s.edj",
                        home, name);
                if (((stat(theme_tmp_path, &st)) == 0) && S_ISREG(st.st_mode))
                        theme_found_path = strdup(theme_tmp_path);
        }

        /*
         * No user theme, so we try the system-wide theme.
         */
        if (!theme_found_path) {
                snprintf(theme_tmp_path, PATH_MAX, PACKAGE_DATA_DIR
                                "/ewl/themes/%s.edj", name);
                if (((stat(theme_tmp_path, &st)) == 0) && S_ISREG(st.st_mode))
                        theme_found_path = strdup(theme_tmp_path);
        }

        /*
         * see if they gave a full path to the theme
         */
        if (!theme_found_path) {
                if (name[0] != '/') {
                        char *cwd;

                        cwd = getenv("PWD");
                        if (cwd != NULL)
                                snprintf(theme_tmp_path, PATH_MAX, "%s/%s", cwd, name);
                        else
                                snprintf(theme_tmp_path, PATH_MAX, "%s", name);

                } else
                        snprintf(theme_tmp_path, PATH_MAX, "%s", name);

                if (((stat(theme_tmp_path, &st)) == 0) && S_ISREG(st.st_mode))
                        theme_found_path = strdup(theme_tmp_path);
        }

        DRETURN_PTR(theme_found_path, DLEVEL_STABLE);
}

/*
 * Initializes the font path based on the theme. Also called by ewl_init, and
 * is not recommended to be called separately.
 */
static void
ewl_theme_font_path_init(void)
{
        const char *font_path;
        char key[PATH_MAX];

        DENTER_FUNCTION(DLEVEL_STABLE);

        /*
         * Setup the default font paths
         */
        ewl_theme_font_paths = ecore_list_new();
        if (!ewl_theme_font_paths)
                DRETURN(DLEVEL_STABLE);
        ecore_list_free_cb_set(ewl_theme_font_paths, free);

        font_path = ewl_theme_data_str_get(NULL, "/theme/font_path");
        if (!font_path)
                DRETURN(DLEVEL_STABLE);

        if (*font_path == '/')
                ecore_list_append(ewl_theme_font_paths, strdup(font_path));
        else {
                int len;
                char *tmp;

                len = strlen(ewl_theme_path);
                tmp = ewl_theme_path + len - 4;

                if (strcmp(tmp, ".edj"))
                        snprintf(key, PATH_MAX, "%s/%s",
                                        ewl_theme_path, font_path);
                else
                        snprintf(key, PATH_MAX, "%s", ewl_theme_path);

                ecore_list_append(ewl_theme_font_paths, strdup(key));
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Private function for freeing theme data in the hash.
 */
static void
ewl_theme_data_free(void *data)
{
        DENTER_FUNCTION(DLEVEL_STABLE);

        if (!data || data == (void *)EWL_THEME_KEY_NOMATCH)
                DRETURN(DLEVEL_STABLE);

        IF_RELEASE(data);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

