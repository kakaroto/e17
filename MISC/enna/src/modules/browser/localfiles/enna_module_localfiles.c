/* Interface */

#include "enna.h"

#define ENNA_MODULE_NAME "localfiles"

typedef struct _Root_Directories
{
    char *uri;
    char *label;
    char *icon;
} Root_Directories;

typedef struct _Module_Config
{
    Eina_List *root_directories;
} Module_Config;

typedef struct _Class_Private_Data
{
    const char *uri;
    const char *prev_uri;
    Module_Config *config;
} Class_Private_Data;

typedef struct _Enna_Module_LocalFiles
{
    Evas *e;
    Enna_Module *em;
    Class_Private_Data *music;
    Class_Private_Data *video;
    Class_Private_Data *photo;
} Enna_Module_LocalFiles;

static Enna_Module_LocalFiles *mod;

static unsigned char _uri_has_extension(const char *uri, int type)
{

    Eina_List *l;
    Eina_List *filters = NULL;

    if (type == ENNA_CAPS_MUSIC)
        filters = enna_config->music_filters;
    else if (type == ENNA_CAPS_VIDEO)
        filters = enna_config->video_filters;
    else if (type == ENNA_CAPS_PHOTO)
        filters = enna_config->photo_filters;

    if (!filters)
        return 0;

    for (l = filters; l; l = l->next)
    {
        const char *ext = l->data;
        if (ecore_str_has_extension(uri, ext))
            return 1;
    }

    return 0;

}

static unsigned char _uri_is_root(Class_Private_Data *data, const char *uri)
{
    Eina_List *l;

    for (l = data->config->root_directories; l; l = l->next)
    {
        Root_Directories *root = l->data;
        if (!strcmp(root->uri, uri))
            return 1;
    }

    return 0;
}

static Eina_List *_class_browse_up(const char *path, ENNA_VFS_CAPS caps,
        Class_Private_Data *data, char *icon)
{

    /* Browse Root */
    if (!path)
    {
        Eina_List *files = NULL;
        Eina_List *l;
        /* FIXME: this list should come from config ! */
        for (l = data->config->root_directories; l; l = l->next)
        {
            Enna_Vfs_File *file;
            Root_Directories *root;

            root = l->data;
            file = enna_vfs_create_directory(root->uri, root->label, "icon/hd",
                    NULL);
            files = eina_list_append(files, file);
        }
        //evas_stringshare_del(data->prev_uri);
        //evas_stringshare_del(data->uri);
        data->prev_uri = NULL;
        data->uri = NULL;
        return files;
    }
    else if (strstr(path, "file://"))
    {
        Ecore_List *files = NULL;
        char *filename = NULL;
        Eina_List *files_list = NULL;
        Eina_List *dirs_list = NULL;
        Eina_List *l;
        char dir[PATH_MAX];

        files = ecore_file_ls(path+7);
        ecore_list_sort(files, ECORE_COMPARE_CB(strcasecmp), ECORE_SORT_MIN);
        filename = ecore_list_first_goto(files);

        while ((filename = (char *)ecore_list_next(files)) != NULL)
        {
            sprintf(dir, "%s/%s", path, filename);
            if (filename[0] == '.')
                continue;
            else if (ecore_file_is_dir(dir+7))
            {
                Enna_Vfs_File *f;

                f = enna_vfs_create_directory(dir, filename, "icon/directory",
                        NULL);
                dirs_list = eina_list_append(dirs_list, f);
                if (data->prev_uri)
                {
                    if (!strcmp(dir, data->prev_uri))
                        f->is_selected = 1;
                    else
                        f->is_selected = 0;
                }
            }
            else if (_uri_has_extension(dir, caps))
            {
                Enna_Vfs_File *f;

                if (caps & ENNA_CAPS_PHOTO)
                    f = enna_vfs_create_file(dir, filename, NULL, dir + 7);
                else
                    f = enna_vfs_create_file(dir, filename, icon, NULL);

                files_list = eina_list_append(files_list, f);
            }
        }
        /* File after dir */
        for (l = files_list; l; l = l->next)
        {
            dirs_list = eina_list_append(dirs_list, l->data);
        }
        //evas_stringshare_del(data->prev_uri);
        data->prev_uri = data->uri;
        data->uri = evas_stringshare_add(path);
        return dirs_list;
    }

    return NULL;

}

static Eina_List *_class_browse_up_music(const char *path)
{
    return _class_browse_up(path, ENNA_CAPS_MUSIC, mod->music, "icon/music");
}

static Eina_List *_class_browse_up_video(const char *path)
{
    return _class_browse_up(path, ENNA_CAPS_VIDEO, mod->video, "icon/video");
}

static Eina_List *_class_browse_up_photo(const char *path)
{
    return _class_browse_up(path, ENNA_CAPS_PHOTO, mod->photo, "icon/photo");
}

static Eina_List * _class_browse_down(Class_Private_Data *data,
        ENNA_VFS_CAPS caps)
{
    /* Browse Root */
    if (data->uri && strstr(data->uri, "file://"))
    {
        char *path_tmp = NULL;
        char *p;
        Eina_List *files = NULL;

        if (_uri_is_root(data, data->uri))
        {
            Eina_List *files = NULL;
            Eina_List *l;
            for (l = data->config->root_directories; l; l = l->next)
            {
                Enna_Vfs_File *file;
                Root_Directories *root;

                root = l->data;
                file = enna_vfs_create_directory(root->uri, root->label,
                        "icon/hd", NULL);
                files = eina_list_append(files, file);
            }
            data->prev_uri = NULL;
            data->uri = NULL;
            return files;
        }

        path_tmp = strdup(data->uri);
        if (path_tmp[strlen(data->uri) - 1] == '/')
            path_tmp[strlen(data->uri) - 1] = 0;
        p = strrchr(path_tmp, '/');
        if (p && *(p - 1) == '/')
            *(p) = 0;
        else if (p)
            *(p) = 0;

        files = _class_browse_up(path_tmp, caps, data, NULL);
        data->uri = evas_stringshare_add(path_tmp);
        return files;
    }

    return NULL;
}

static Eina_List * _class_browse_down_music(void)
{
    return _class_browse_down(mod->music, ENNA_CAPS_MUSIC);
}

static Eina_List * _class_browse_down_video(void)
{
    return _class_browse_down(mod->video, ENNA_CAPS_VIDEO);
}

static Eina_List * _class_browse_down_photo(void)
{
    return _class_browse_down(mod->photo, ENNA_CAPS_PHOTO);
}

static Enna_Vfs_File * _class_vfs_get(int type)
{
    switch (type)
    {
        case ENNA_CAPS_MUSIC:
            return enna_vfs_create_directory((char *) mod->music->uri,
                    (char *) ecore_file_file_get(mod->music->uri),
                    (char *) evas_stringshare_add("icon/music"), NULL);
        case ENNA_CAPS_VIDEO:
            return enna_vfs_create_directory((char *) mod->video->uri,
                    (char *) ecore_file_file_get(mod->video->uri),
                    (char *) evas_stringshare_add("icon/video"), NULL);

        case ENNA_CAPS_PHOTO:
            return enna_vfs_create_directory((char *) mod->photo->uri,
                    (char *) ecore_file_file_get(mod->photo->uri),
                    (char *) evas_stringshare_add("icon/photo"), NULL);

    }

    return NULL;
}

static Enna_Vfs_File * _class_vfs_get_music(void)
{
    return _class_vfs_get(ENNA_CAPS_MUSIC);
}

static Enna_Vfs_File * _class_vfs_get_video(void)
{
    return _class_vfs_get(ENNA_CAPS_VIDEO);
}

static Enna_Vfs_File * _class_vfs_get_photo(void)
{
    return _class_vfs_get(ENNA_CAPS_PHOTO);
}

static void __class_init(const char *name, Class_Private_Data **priv,
        ENNA_VFS_CAPS caps, Enna_Class_Vfs *class, char *key)
{
    Class_Private_Data *data;
    Enna_Config_Data *cfgdata;
    Eina_List *l;

    data = calloc(1, sizeof(Class_Private_Data));
    *priv = data;

    enna_vfs_append(name, caps, class);
    data->prev_uri = NULL;

    data->config = calloc(1, sizeof(Module_Config));
    data->config->root_directories = NULL;

    cfgdata = enna_config_module_pair_get("localfiles");
    if (!cfgdata)
        return;
    for (l = cfgdata->pair; l; l = l->next)
    {
        Config_Pair *pair = l->data;
        if (!strcmp(pair->key, key))
        {
            Eina_List *dir_data;
            enna_config_value_store(&dir_data, key, ENNA_CONFIG_STRING_LIST,
                    pair);
            if (dir_data)
            {
                if (eina_list_count(dir_data) != 3)
                    continue;
                else
                {
                    Root_Directories *root;

                    root = calloc(1, sizeof(Root_Directories));
                    root->uri = eina_list_nth(dir_data, 0);
                    root->label = eina_list_nth(dir_data, 1);
                    enna_log(ENNA_MSG_INFO, ENNA_MODULE_NAME, "Root Data: %s",
                            root->uri);
                    root->icon = eina_list_nth(dir_data, 2);
                    data->config->root_directories = eina_list_append(
                            data->config->root_directories, root);
                }
            }
        }
    }
}

static Enna_Class_Vfs class_music =
{ "localfiles_music", 1, "Browse Local Files", NULL, "icon/hd",
{ NULL, NULL, _class_browse_up_music, _class_browse_down_music,
        _class_vfs_get_music, 
},
};

static Enna_Class_Vfs class_video =
{ "localfiles_video", 1, "Browse Local Files", NULL, "icon/hd",
{ NULL, NULL, _class_browse_up_video, _class_browse_down_video,
        _class_vfs_get_video, 
},
};

static Enna_Class_Vfs class_photo =
{ "localfiles_photo", 1, "Browse Local Files", NULL, "icon/hd",
{ NULL, NULL, _class_browse_up_photo, _class_browse_down_photo,
        _class_vfs_get_photo, 
},
};

/* Module interface */

EAPI Enna_Module_Api module_api =
{
    ENNA_MODULE_VERSION,
    "localfiles"
};

EAPI void module_init(Enna_Module *em)
{
    if (!em)
        return;

    mod = calloc(1, sizeof(Enna_Module_LocalFiles));
    mod->em = em;
    em->mod = mod;

    __class_init("localfiles_music", &mod->music, ENNA_CAPS_MUSIC,
            &class_music, "path_music");
    __class_init("localfiles_video", &mod->video, ENNA_CAPS_VIDEO,
            &class_video, "path_video");
    __class_init("localfiles_photo", &mod->photo, ENNA_CAPS_PHOTO,
            &class_photo, "path_photo");
}

EAPI void module_shutdown(Enna_Module *em)
{
    Enna_Module_LocalFiles *mod;

    mod = em->mod;;
    free(mod->music);
    free(mod->video);
    free(mod->photo);
}
