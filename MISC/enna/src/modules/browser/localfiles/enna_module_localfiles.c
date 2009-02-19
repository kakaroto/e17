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
    Ecore_Event_Handler *volume_add_handler;
    Ecore_Event_Handler *volume_remove_handler;
} Class_Private_Data;

typedef struct _Enna_Module_LocalFiles
{
    Evas *e;
    Enna_Module *em;
#ifdef BUILD_ACTIVITY_MUSIC
    Class_Private_Data *music;
#endif
#ifdef BUILD_ACTIVITY_VIDEO
    Class_Private_Data *video;
#endif
#ifdef BUILD_ACTIVITY_PHOTO
    Class_Private_Data *photo;
#endif
} Enna_Module_LocalFiles;

static Enna_Module_LocalFiles *mod;

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
            file = enna_vfs_create_directory(root->uri, root->label,
                                             root->icon ?
                                             root->icon : "icon/hd", NULL);
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

	/* If no file found return immediatly*/
	if (!files)
	    return NULL;

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
            else if (enna_util_uri_has_extension(dir, caps))
            {
                Enna_Vfs_File *f;
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

#ifdef BUILD_ACTIVITY_MUSIC
static Eina_List *_class_browse_up_music(const char *path, void *cookie)
{
    return _class_browse_up(path, ENNA_CAPS_MUSIC,
                            mod->music, "icon/file/music");
}
#endif

#ifdef BUILD_ACTIVITY_VIDEO
static Eina_List *_class_browse_up_video(const char *path, void *cookie)
{
    return _class_browse_up(path, ENNA_CAPS_VIDEO,
                            mod->video, "icon/file/video");
}
#endif

#ifdef BUILD_ACTIVITY_PHOTO
static Eina_List *_class_browse_up_photo(const char *path, void *cookie)
{
    return _class_browse_up(path, ENNA_CAPS_PHOTO,
                            mod->photo, "icon/file/photo");
}
#endif

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
                        root->icon ? root->icon : "icon/hd", NULL);
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

#ifdef BUILD_ACTIVITY_MUSIC
static Eina_List * _class_browse_down_music(void *cookie)
{
    return _class_browse_down(mod->music, ENNA_CAPS_MUSIC);
}
#endif

#ifdef BUILD_ACTIVITY_VIDEO
static Eina_List * _class_browse_down_video(void *cookie)
{
    return _class_browse_down(mod->video, ENNA_CAPS_VIDEO);
}
#endif

#ifdef BUILD_ACTIVITY_PHOTO
static Eina_List * _class_browse_down_photo(void *cookie)
{
    return _class_browse_down(mod->photo, ENNA_CAPS_PHOTO);
}
#endif

static Enna_Vfs_File * _class_vfs_get(int type)
{
    switch (type)
    {
#ifdef BUILD_ACTIVITY_MUSIC
        case ENNA_CAPS_MUSIC:
            return enna_vfs_create_directory(mod->music->uri,
                    ecore_file_file_get(mod->music->uri),
                    evas_stringshare_add("icon/music"), NULL);
#endif
#ifdef BUILD_ACTIVITY_VIDEO
        case ENNA_CAPS_VIDEO:
            return enna_vfs_create_directory(mod->video->uri,
                    ecore_file_file_get(mod->video->uri),
                    evas_stringshare_add("icon/video"), NULL);
#endif
#ifdef BUILD_ACTIVITY_PHOTO
        case ENNA_CAPS_PHOTO:
            return enna_vfs_create_directory(mod->photo->uri,
                    ecore_file_file_get(mod->photo->uri),
                    evas_stringshare_add("icon/photo"), NULL);
#endif
        default:
            break;
    }

    return NULL;
}

#ifdef BUILD_ACTIVITY_MUSIC
static Enna_Vfs_File * _class_vfs_get_music(void *cookie)
{
    return _class_vfs_get(ENNA_CAPS_MUSIC);
}
#endif

#ifdef BUILD_ACTIVITY_VIDEO
static Enna_Vfs_File * _class_vfs_get_video(void *cookie)
{
    return _class_vfs_get(ENNA_CAPS_VIDEO);
}
#endif

#ifdef BUILD_ACTIVITY_PHOTO
static Enna_Vfs_File * _class_vfs_get_photo(void *cookie)
{
    return _class_vfs_get(ENNA_CAPS_PHOTO);
}
#endif

static int _add_volumes_cb(void *data, int type, void *event)
{
    Root_Directories *root ;
    Enna_Volume *v =  event;
    Class_Private_Data *priv = data;

    if (!strcmp(v->type, "file://"))
    {
	root = calloc(1, sizeof(Root_Directories));

	root->uri = strdup(v->uri);
	root->label = strdup(v->label);
	root->icon = strdup(v->icon);

	enna_log(ENNA_MSG_EVENT, ENNA_MODULE_NAME, "add : %s", root->label);
	priv->config->root_directories = eina_list_append(
	    priv->config->root_directories, root);
    }
    return 1;
}

static int _remove_volumes_cb(void *data, int type, void *event)
{
    Enna_Volume *v = event;
    Class_Private_Data *priv = data;

    if (!strcmp(v->type, "file://"))
    {
	Root_Directories *root;
	Eina_List *l;
	EINA_LIST_FOREACH(priv->config->root_directories, l, root)
            {
                if (!strcmp(root->label, v->label))
                {
                    enna_log(ENNA_MSG_EVENT, ENNA_MODULE_NAME,
                             "remove : %s", root->label);
                    priv->config->root_directories =
                        eina_list_remove(priv->config->root_directories, root);
                }
            }
    }
    return 1;
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
                    enna_log(ENNA_MSG_INFO, ENNA_MODULE_NAME,
                             "Root Data: %s", root->uri);
                    root->icon = eina_list_nth(dir_data, 2);
                    data->config->root_directories = eina_list_append(
                        data->config->root_directories, root);
                }
            }
        }
    }

    data->volume_add_handler =
        ecore_event_handler_add(ENNA_EVENT_VOLUME_ADDED,
                                _add_volumes_cb, data);
    data->volume_remove_handler =
        ecore_event_handler_add(ENNA_EVENT_VOLUME_REMOVED,
                                _remove_volumes_cb, data);

}

#ifdef BUILD_ACTIVITY_MUSIC
static Enna_Class_Vfs class_music = {
    "localfiles_music",
    1,
    "Browse Local Devices",
    NULL,
    "icon/hd",
    { NULL,
      NULL,
      _class_browse_up_music,
      _class_browse_down_music,
      _class_vfs_get_music,
    },
    NULL
};
#endif

#ifdef BUILD_ACTIVITY_VIDEO
static Enna_Class_Vfs class_video = {
    "localfiles_video",
    1,
    "Browse Local Devices",
    NULL,
    "icon/hd",
    { NULL,
      NULL,
      _class_browse_up_video,
      _class_browse_down_video,
      _class_vfs_get_video,
    },
    NULL
};
#endif

#ifdef BUILD_ACTIVITY_PHOTO
static Enna_Class_Vfs class_photo = {
    "localfiles_photo",
    1,
    "Browse Local Devices",
    NULL,
    "icon/hd",
    { NULL,
      NULL,
      _class_browse_up_photo,
      _class_browse_down_photo,
      _class_vfs_get_photo,
    },
    NULL
};
#endif

/* Module interface */

Enna_Module_Api module_api =
{
    ENNA_MODULE_VERSION,
    ENNA_MODULE_BROWSER,
    "browser_localfiles"
};

void module_init(Enna_Module *em)
{
    if (!em)
        return;

    mod = calloc(1, sizeof(Enna_Module_LocalFiles));
    mod->em = em;
    em->mod = mod;

#ifdef BUILD_ACTIVITY_MUSIC
    __class_init("localfiles_music", &mod->music, ENNA_CAPS_MUSIC,
                 &class_music, "path_music");
#endif
#ifdef BUILD_ACTIVITY_VIDEO
    __class_init("localfiles_video", &mod->video, ENNA_CAPS_VIDEO,
                 &class_video, "path_video");
#endif
#ifdef BUILD_ACTIVITY_PHOTO
    __class_init("localfiles_photo", &mod->photo, ENNA_CAPS_PHOTO,
                 &class_photo, "path_photo");
#endif
}

void module_shutdown(Enna_Module *em)
{
    Enna_Module_LocalFiles *mod;

    mod = em->mod;
#ifdef BUILD_ACTIVITY_MUSIC
    free(mod->music);
#endif
#ifdef BUILD_ACTIVITY_VIDEO
    free(mod->video);
#endif
#ifdef BUILD_ACTIVITY_PHOTO
    free(mod->photo);
#endif
}
