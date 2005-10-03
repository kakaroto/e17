#include "Entangle.h"

#include <limits.h>
#include <string.h>

static Ecore_Hash *entangle_eapps = NULL;
static Ecore_List *entangle_eapp_list = NULL;
static void entangle_eapps_cb_free(void *data);

int
entangle_eapps_init()
{
    char path[PATH_MAX];
    char *home;
    Ecore_List *eapps;
    int i;

    home = getenv("HOME");
    if (!home)
    {
        fprintf(stderr, "Unable to get HOME from environment.\n");
        return 0;
    }
    snprintf(path, PATH_MAX, "%s/.e/e/applications/all", home);

    if (!ecore_file_exists(path))
    {
        fprintf(stderr, "%s dosen't exist. Where are the eapps?\n", path);
        return 0;
    }

    eapps = ecore_file_ls(path);
    if (!eapps)
    {
        fprintf(stderr, "Didn't get any Eapp files.\n");
        return 0;
    }

    entangle_eapps = ecore_hash_new(ecore_str_hash, ecore_str_compare);
    entangle_eapp_list = ecore_list_new();
    ecore_hash_set_free_value(entangle_eapps, entangle_eapps_cb_free);
    for (i = 0; i < ecore_list_nodes(eapps); i++)
    {
        char *ret, *tmp;
        int ret_size;
        Eet_File *ef;
        char e_path[PATH_MAX];
        Entangle_Eapp *eapp;

        eapp = calloc(1, sizeof(Entangle_Eapp));
        if (!eapp) 
        {
            fprintf(stderr, "Not enough memory to create eapp.\n");
            continue;
        }

        tmp = ecore_list_goto_index(eapps, i);
        eapp->eapp_name = strdup(tmp);

        snprintf(e_path, PATH_MAX, "%s/%s", path, tmp);
        eapp->path = strdup(e_path);
        ef = eet_open(e_path, EET_FILE_MODE_READ);
        if (!ef) 
        {
            fprintf(stderr, "Error reading: %s\n", e_path);
            continue;
        }

        ret = eet_read(ef, "app/info/name", &ret_size);
        if (ret_size > 0)
        {
            eapp->name = malloc(sizeof(char) * (ret_size + 1));
            snprintf(eapp->name, ret_size + 1, "%s", ret);
        }

        IF_FREE(ret);

        ret = eet_read(ef, "app/info/exe", &ret_size);
        if (ret_size > 0)
        {
            eapp->exe = malloc(sizeof(char) * (ret_size + 1));
            snprintf(eapp->exe, ret_size + 1, "%s", ret);
        }

        IF_FREE(ret);

        ret = eet_read(ef, "app/window/class", &ret_size);
        if (ret_size > 0)
        {
            eapp->class = malloc(sizeof(char) * (ret_size + 1));
            snprintf(eapp->class, ret_size + 1, "%s", ret);
        }

        IF_FREE(ret);

        ecore_hash_set(entangle_eapps, tmp, eapp);
        ecore_list_append(entangle_eapp_list, tmp);
        eet_close(ef);
    }

    ecore_list_destroy(eapps);

    return 1;
}

void
entangle_eapps_shutdown()
{
    if (entangle_eapps) ecore_hash_destroy(entangle_eapps);
}

static void
entangle_eapps_cb_free(void *data)
{
    Entangle_Eapp *eapp;

    eapp = data;
    if (!eapp) return;

    IF_FREE(eapp->path);
    IF_FREE(eapp->name);
    IF_FREE(eapp->eapp_name);
    IF_FREE(eapp->exe);
    IF_FREE(eapp->class);
    FREE(eapp);
}

Entangle_Eapp *
entangle_eapps_get(const char *key)
{
    return ecore_hash_get(entangle_eapps, (char *)key);
}

Ecore_List *
entangle_eapps_key_list_get(void)
{
    if (!entangle_eapps) return NULL;
    return entangle_eapp_list;
}

