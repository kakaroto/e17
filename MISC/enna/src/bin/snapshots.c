#include "enna.h"

#define SNAPSHOTS_PATH "snapshots"

EAPI char *
enna_snapshot_get(const char *uri)
{
    char dst[1024];
    char *md5;
    
    if (!uri)
        return NULL;

    if (!enna->use_snapshots)
        return NULL;
    
    /* try to create snapshot directory storage first */
    memset(dst, '\0', sizeof (dst));
    snprintf(dst, sizeof (dst),
             "%s/.enna/%s", enna_util_user_home_get(), SNAPSHOTS_PATH);
    if (!ecore_file_is_dir(dst))
        mkdir(dst, 0755);

    /* calculate uri's MD5 sum to determine unique filename */
    md5 = md5sum((char *)uri);
    memset(dst, '\0', sizeof (dst));
    snprintf(dst, sizeof (dst), "%s/.enna/%s/%s.png",
            enna_util_user_home_get(), SNAPSHOTS_PATH, md5);
    free(md5);

    /* ask mediaplayer to take a snapshot if file do not already exists */
    if (!ecore_file_exists (dst))
        enna_mediaplayer_snapshot(uri, dst);
    
    return strdup (dst);
}
