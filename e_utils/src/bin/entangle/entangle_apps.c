#include "Entangle.h"

#include <string.h>
#include <limits.h>

static Ecore_List *entangle_apps_bar = NULL;
static Ecore_List *entangle_apps_engage = NULL;
static Ecore_List *entangle_apps_favorite = NULL;
static Ecore_List *entangle_apps_restart = NULL;
static Ecore_List *entangle_apps_startup = NULL;
static Ecore_List *entangle_apps_deleted_dirs = NULL;

static void entangle_apps_free_cb(void *data);
static void entangle_apps_dir_init(Ecore_List *dir, const char *name);
static void entangle_apps_dir_save(Ecore_List *dir, const char *name);
static void entangle_apps_dir_dump(Ecore_List *dir, const char *indent);
static void entangle_apps_dir_delete(const char *dir);

int
entangle_apps_init(void)
{
    char *home;

    home = getenv("HOME");
    if (!home)
    {
        fprintf(stderr, "Unable to get HOME env variable.\n");
        return 0;
    }

    entangle_apps_bar = ecore_list_new();
    entangle_apps_engage = ecore_list_new();
    entangle_apps_favorite = ecore_list_new();
    entangle_apps_restart = ecore_list_new();
    entangle_apps_startup = ecore_list_new();
    entangle_apps_deleted_dirs = ecore_list_new();

    ecore_list_free_cb_set(entangle_apps_bar, entangle_apps_free_cb);
    ecore_list_free_cb_set(entangle_apps_engage, entangle_apps_free_cb);
    ecore_list_free_cb_set(entangle_apps_favorite, entangle_apps_free_cb);
    ecore_list_free_cb_set(entangle_apps_restart, entangle_apps_free_cb);
    ecore_list_free_cb_set(entangle_apps_startup, entangle_apps_free_cb);
    ecore_list_free_cb_set(entangle_apps_deleted_dirs, entangle_apps_free_cb);

    entangle_apps_dir_init(entangle_apps_bar, "bar/default");
    entangle_apps_dir_init(entangle_apps_engage, "bar/engage");
    entangle_apps_dir_init(entangle_apps_favorite, "favorite");
    entangle_apps_dir_init(entangle_apps_restart, "restart");
    entangle_apps_dir_init(entangle_apps_startup, "startup");

    return 1;
}

void
entangle_apps_shutdown(void)
{
    if (entangle_apps_deleted_dirs) ecore_list_destroy(entangle_apps_deleted_dirs);
    if (entangle_apps_bar) ecore_list_destroy(entangle_apps_bar);
    if (entangle_apps_engage) ecore_list_destroy(entangle_apps_engage);
    if (entangle_apps_favorite) ecore_list_destroy(entangle_apps_favorite);
    if (entangle_apps_restart) ecore_list_destroy(entangle_apps_restart);
    if (entangle_apps_startup) ecore_list_destroy(entangle_apps_startup);
}

void
entangle_apps_save(void)
{
    int i;

    for (i = 0; i < ecore_list_count(entangle_apps_deleted_dirs); i++)
    {
        Entangle_App *app;
        app = ecore_list_index_goto(entangle_apps_deleted_dirs, i);
        entangle_apps_dir_delete(app->eapp->path);
    }

    entangle_apps_dir_save(entangle_apps_bar, "bar/default");
    entangle_apps_dir_save(entangle_apps_engage, "engage");
    entangle_apps_dir_save(entangle_apps_favorite, "favorite");
    entangle_apps_dir_save(entangle_apps_restart, "restart");
    entangle_apps_dir_save(entangle_apps_startup, "startup");
}

static void
entangle_apps_dir_delete(const char *dir)
{
    char path[PATH_MAX];

    if (ecore_file_is_dir(dir))
    {
        Ecore_List *contents;
	char *name;

        contents = ecore_file_ls(dir);
	if (contents)
	{
	    while ((name = ecore_list_next(contents)))
	    {
		snprintf(path, PATH_MAX, "%s/%s", dir, name);

		if (ecore_file_is_dir(path))
		    entangle_apps_dir_delete(path);

		if (!ecore_file_unlink(path))
		   fprintf(stderr, "Error unlinking (%s).\n", path);
	    }
	    ecore_list_destroy(contents);
	}
        if (!ecore_file_rmdir(dir))
            fprintf(stderr, "Error deleting (%s).\n", dir);
    }
    else
        fprintf(stderr, "Set something (%s) not a directory on the directory "
                        "delete list.\n", dir);
}


static void
entangle_apps_free_cb(void *data)
{
    Entangle_App *app;
    app = data;

    if (!app) return;

    /* if we have children we are a directory, and not in the eapp list */
    if (app->children) 
    {
        IF_FREE(app->eapp->path);
        IF_FREE(app->eapp->name);
        FREE(app->eapp);
        ecore_list_destroy(app->children);
    }
    app->eapp = NULL;
    FREE(app);
}

static void
entangle_apps_dir_init(Ecore_List *dir, const char *name)
{
    FILE *file;
    char path[PATH_MAX];
    char buf[1024];

    snprintf(path, PATH_MAX, "%s/.e/e/applications/%s/.order", 
                                            getenv("HOME"), name);

    file = fopen(path, "r");
    if (!file) return;

    while ((fgets(buf, 1024, file)) != NULL)
    {
        Entangle_App *app;
        Entangle_Eapp *eapp;

        buf[strlen(buf) - 1] = '\0';    /* strip the \n */
        if (strlen(buf) == 0) continue;

        app = calloc(1, sizeof(Entangle_App));
        eapp = entangle_eapps_get(buf);
        if (!eapp)
        {
            char dir_path[PATH_MAX];
            char rel_path[PATH_MAX];

            snprintf(rel_path, PATH_MAX, "%s/%s", name, buf);
            snprintf(dir_path, PATH_MAX, "%s/.e/e/applications/%s", 
                                                getenv("HOME"), rel_path);

            if (!ecore_file_is_dir(dir_path))
            {
                fprintf(stderr, "ERROR found %s in the %s order file. There is no .eap "
                                "for it, and it isn't a directory. Skipping...\n", buf, name);
                FREE(app);
                continue;
            }

            app->eapp = calloc(1, sizeof(Entangle_Eapp));
            app->eapp->eapp_name = strdup(buf);
            app->eapp->name = strdup(buf);
            app->eapp->path = strdup(dir_path);
            app->eapp->exe = strdup(rel_path);

            app->children = ecore_list_new();
            entangle_apps_dir_init(app->children, rel_path);
        }
        else app->eapp = eapp;

        ecore_list_append(dir, app);
    }

    fclose(file);
}

static void
entangle_apps_dir_save(Ecore_List *dir, const char *name)
{
    int i;
    FILE *fp;
    char path[PATH_MAX];
    char dir_path[PATH_MAX];

    if (!dir) return;

    /* create the directory if it dosen't exist */
    snprintf(dir_path, PATH_MAX, "%s/.e/e/applications/%s", getenv("HOME"), name);
    if (!ecore_file_exists(dir_path)) ecore_file_mkdir(dir_path);

    snprintf(path, PATH_MAX, "%s/.order", dir_path);
    fp = fopen(path, "w");
    for (i = 0; i < ecore_list_count(dir); i++)
    {
        Entangle_App *app;

        app = ecore_list_index_goto(dir, i);
        snprintf(path, PATH_MAX, "%s\n", app->eapp->eapp_name);
        fwrite(path, sizeof(char), strlen(path), fp);

        if (app->children != NULL)
        {
            snprintf(path, PATH_MAX, "%s/%s", name, app->eapp->name);
            entangle_apps_dir_save(app->children, path);
        }
    }
    fclose(fp);
}

void
entangle_apps_dump(void)
{
    printf("Favorites\n");
    entangle_apps_dir_dump(entangle_apps_favorite, "");

    printf("\niBar\n");
    entangle_apps_dir_dump(entangle_apps_bar, "");

    printf("\nEngage\n");
    entangle_apps_dir_dump(entangle_apps_engage, "");

    printf("\nStartup\n");
    entangle_apps_dir_dump(entangle_apps_startup, "");

    printf("\nRestart\n");
    entangle_apps_dir_dump(entangle_apps_restart, "");
}

static void
entangle_apps_dir_dump(Ecore_List *dir, const char *indent)
{
    int i;
    char buf[1024];

    for (i = 0; i < ecore_list_count(dir); i++)
    {
        Entangle_App *app;
        
        app = ecore_list_index_goto(dir, i);
        printf("%s%s [%s]\n", indent, app->eapp->name, app->eapp->eapp_name);
        if (app->children != NULL)
        {
            snprintf(buf, 1024, "%s    ", indent);
            entangle_apps_dir_dump(app->children, buf);
        }
    }
}

Ecore_List *
entangle_apps_list_get(const char *list)
{
    if (!strcmp(list, "favorite")) return entangle_apps_favorite;
    if (!strcmp(list, "bar")) return entangle_apps_bar;
    if (!strcmp(list, "engage")) return entangle_apps_engage;
    if (!strcmp(list, "startup")) return entangle_apps_startup;
    if (!strcmp(list, "restart")) return entangle_apps_restart;
    if (!strcmp(list, "deleted_dirs")) return entangle_apps_deleted_dirs;
    return NULL;
}


