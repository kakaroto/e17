#include "enna.h"

#include <stdio.h>
#include <mntent.h>

#define ENNA_MODULE_NAME   "mtab"

#define MTAB_FILE          "/etc/mtab"

typedef struct _Enna_Module_mtab
{
    Evas *e;
    Enna_Module *em;
} Enna_Module_mtab;

typedef enum {
    MTAB_TYPE_NONE,
    MTAB_TYPE_NFS,
    MTAB_TYPE_SMB,
} mtab_type_t;

static Enna_Module_mtab *mod;

/***************************************/
/*           mtab handling             */
/***************************************/

static void
mtab_add_mnt (mtab_type_t t, char *fsname, char *dir)
{
    Enna_Volume *evol;
    char name[512], tmp[1024], srv[128], share[128];
    const char *icon = NULL;
    const char *type = NULL;
    const char *uri = NULL;
    char *p;

    if (t == MTAB_TYPE_NONE)
        return;

    if (!fsname || !dir)
        return;

    memset (name,  '\0', sizeof (name));
    memset (tmp,   '\0', sizeof (tmp));
    memset (srv,   '\0', sizeof (srv));
    memset (share, '\0', sizeof (share));

    switch (t)
    {
    case MTAB_TYPE_NFS:
        p = strchr (fsname, ':');
        strncpy (srv, fsname, p - fsname);
        strcpy (share, p + 1);
        snprintf (name, sizeof (name), "[NFS] %s on %s", share, srv);
        icon = eina_stringshare_add ("icon/dev/nfs");
        break;

    case MTAB_TYPE_SMB:
        p = strchr (fsname + 2, '/');
        strncpy (srv, fsname + 2, p - (fsname + 2));
        strcpy (share, p + 1);
        snprintf (name, sizeof (name), "[SAMBA] %s on %s", share, srv);
        icon = eina_stringshare_add ("icon/dev/samba");
        break;

    default:
        break;
    }

    type = eina_stringshare_add ("file://");
    snprintf (tmp, sizeof (tmp), "file://%s", dir);
    uri = eina_stringshare_add (tmp);

    evol        = calloc (1, sizeof (Enna_Volume));
    evol->name  = strdup (name);
    evol->label = strdup (name);
    evol->icon  = icon;
    evol->type  = type;
    evol->uri   = uri;

    enna_log (ENNA_MSG_EVENT, ENNA_MODULE_NAME,
              "New mount point discovered at %s", fsname);

    enna_volumes_append (type, evol);
}

static void
mtab_parse (void)
{
    struct mntent *mnt;
    FILE *fp;

    fp = fopen (MTAB_FILE, "r");
    if (!fp)
        return;

    while ((mnt = getmntent (fp)))
    {
        mtab_type_t type = MTAB_TYPE_NONE;

        if (!strcmp (mnt->mnt_type, "nfs") ||
            !strcmp (mnt->mnt_type, "nfs4"))
            type = MTAB_TYPE_NFS;
        else if (!strcmp (mnt->mnt_type, "smbfs") ||
                 !strcmp (mnt->mnt_type, "cifs"))
            type = MTAB_TYPE_SMB;
        else
            continue;

        mtab_add_mnt (type, mnt->mnt_fsname, mnt->mnt_dir);
    }

    endmntent (fp);
}

/* Module interface */

Enna_Module_Api module_api = {
    ENNA_MODULE_VERSION,
    ENNA_MODULE_VOLUME,
    "volume_mtab"
};

void
module_init (Enna_Module *em)
{
    if (!em)
        return;

    mod = calloc (1, sizeof (Enna_Module_mtab));
    mod->em = em;
    em->mod = mod;

    mtab_parse ();
}

void
module_shutdown (Enna_Module *em)
{
    /* nothing to do here */
}
