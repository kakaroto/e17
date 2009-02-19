/* Interface */

#include "enna.h"

#define ENNA_MODULE_NAME "dvd"



typedef struct _Class_Private_Data
{
    const char *uri;
    unsigned char state;
    Ecore_Event_Handler *volume_add_handler;
    Ecore_Event_Handler *volume_remove_handler;
} Class_Private_Data;

typedef struct _Enna_Module_Dvd
{
    Evas *e;
    Enna_Module *em;
    Class_Private_Data *dvd;

} Enna_Module_Dvd;

static Enna_Module_Dvd *mod;

static Eina_List *_class_browse_up(const char *path, void *cookie)
{
    Enna_Vfs_File *f;
    Eina_List *l = NULL;
    f = enna_vfs_create_file("dvd://", "Play", "icon/video", NULL);
    l = eina_list_append(l, f);
    f = enna_vfs_create_file("dvdnav://", "Play (With Menus)", "icon/video", NULL);
    l = eina_list_append(l, f);
    return l;
}


static Eina_List * _class_browse_down(void *cookie)
{
    return NULL;
}

static Enna_Vfs_File * _class_vfs_get(void *cookie)
{

    return enna_vfs_create_directory(mod->dvd->uri,
	ecore_file_file_get(mod->dvd->uri),
	evas_stringshare_add("icon/dvd"), NULL);
}



static Enna_Class_Vfs class_dvd = {
    "dvd_dvd",
    1,
    "Watch DVD Video",
    NULL,
    "icon/dev/dvd",
    {
	NULL,
	NULL,
	_class_browse_up,
	_class_browse_down,
	_class_vfs_get,
    },
    NULL
};

static int _add_volumes_cb(void *data, int type, void *event)
{
    Enna_Volume *v =  event;

    if (!strcmp(v->type, "dvd://"))
    {
	printf("Dvd added\n");
	enna_vfs_append("dvd", ENNA_CAPS_VIDEO, &class_dvd);
    }
    return 1;
}

static int _remove_volumes_cb(void *data, int type, void *event)
{
    Enna_Volume *v = event;

    if (!strcmp(v->type, "dvd://"))
    {
	printf("DVD Removes\n");
	enna_vfs_class_remove("dvd", ENNA_CAPS_VIDEO);
    }
    return 1;
}



/* Module interface */

Enna_Module_Api module_api =
{
    ENNA_MODULE_VERSION,
    ENNA_MODULE_BROWSER,
    "browser_dvd"
};

void module_init(Enna_Module *em)
{
    if (!em)
        return;

    mod = calloc(1, sizeof(Enna_Module_Dvd));
    mod->em = em;
    em->mod = mod;

    mod->dvd = calloc(1, sizeof(Class_Private_Data));



    mod->dvd->volume_add_handler =
        ecore_event_handler_add(ENNA_EVENT_VOLUME_ADDED,
	    _add_volumes_cb, mod->dvd);
    mod->dvd->volume_remove_handler =
        ecore_event_handler_add(ENNA_EVENT_VOLUME_REMOVED,
	    _remove_volumes_cb, mod->dvd);


}

void module_shutdown(Enna_Module *em)
{
    Enna_Module_Dvd *mod;

    mod = em->mod;
    free(mod->dvd);

}
