/* Interface */

#include "enna.h"

#define ENNA_MODULE_NAME "metadata_localfiles"
#define ENNA_GRABBER_NAME "localfiles"

typedef struct _Enna_Module_Localfiles
{
    Evas *evas;
    Enna_Module *em;
} Enna_Module_Localfiles;

static Enna_Module_Localfiles *mod;

/*****************************************************************************/
/*                         Private Module API                                */
/*****************************************************************************/

static void
cover_get_from_saved_file (Enna_Metadata *meta)
{
    char cover[1024];

    enna_log (ENNA_MSG_EVENT, ENNA_MODULE_NAME,
              "Trying to get cover from previously saved cover file");

    if (!meta->keywords || !meta->md5)
        return;

    memset (cover, '\0', sizeof (cover));
    snprintf (cover, sizeof (cover), "%s/.enna/covers/%s.png",
              enna_util_user_home_get (), meta->md5);

    if (!ecore_file_exists (cover))
        return;

    meta->cover = strdup (cover);
}

static void
cover_get_from_picture_file (Enna_Metadata *meta)
{
    const char *known_filenames[] =
    { "cover", "front" };

    const char *known_extensions[] =
    { "jpg", "JPG", "jpeg", "JPEG", "png", "PNG" };

    char *dir = NULL;
    const char *filename = NULL;
    char cover[1024];
    int i, j;
    
    enna_log (ENNA_MSG_EVENT, ENNA_MODULE_NAME,
              "Trying to get cover from picture files");

    if (!meta || !meta->uri)
        return;

    filename = ecore_file_file_get (meta->uri);
    if (!filename)
        goto out;
    
    dir = ecore_file_dir_get (meta->uri);
    if (!ecore_file_can_read (dir))
        goto out;

    for (i = 0; i < ARRAY_NB_ELEMENTS (known_extensions); i++)
    {
        memset (cover, '\0', sizeof (cover));
        snprintf (cover, sizeof (cover), "%s/%s.%s", dir, filename,
                  known_extensions[i]);

        if (ecore_file_exists (cover))
        {
            meta->cover = strdup (cover);
            goto out;
        }

        for (j = 0; j < ARRAY_NB_ELEMENTS (known_filenames); j++)
        {
            memset (cover, '\0', sizeof (cover));
            snprintf (cover, sizeof (cover), "%s/%s.%s", dir,
                      known_filenames[j], known_extensions[i]);

            if (!ecore_file_exists (cover))
                continue;

            meta->cover = strdup (cover);
            goto out;
        }
    }

 out:
    ENNA_FREE (dir);
}

static void
localfiles_grab (Enna_Metadata *meta, int caps)
{
    if (!meta)
        return;

    /* do not grab if already known */
    if (meta->cover)
        return;
    
    cover_get_from_saved_file (meta);
    if (meta->cover)
        goto cover_found;

    /* check for known cover artwork filenames */
    cover_get_from_picture_file (meta);
    if (meta->cover)
        goto cover_found;

 cover_found:
    if (meta->cover)
        enna_log (ENNA_MSG_INFO, ENNA_MODULE_NAME,
                  "Using cover from: %s", meta->cover);
}

static Enna_Metadata_Grabber grabber = {
    ENNA_GRABBER_NAME,
    ENNA_GRABBER_PRIORITY_MAX,
    0,
    ENNA_GRABBER_CAP_COVER,
    localfiles_grab,
};

/*****************************************************************************/
/*                          Public Module API                                */
/*****************************************************************************/

Enna_Module_Api module_api =
{
    ENNA_MODULE_VERSION,
    ENNA_MODULE_METADATA,
    ENNA_MODULE_NAME
};

void module_init(Enna_Module *em)
{
    if (!em)
        return;

    mod = calloc(1, sizeof(Enna_Module_Localfiles));

    mod->em = em;
    mod->evas = em->evas;

    enna_metadata_add_grabber (&grabber);
}

void module_shutdown(Enna_Module *em)
{
    //enna_metadata_remove_grabber (ENNA_GRABBER_NAME);
    free(mod);
}
