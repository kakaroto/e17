#ifndef __EVFS_VFOLDER_H_
#define __EVFS_VFOLDER_H_

/* We have to be careful here. There are several implementations
 * of Virtual Folders on several platforms.  The most recently 
 * hyped, of course, is the Windows Vista version.
 * In all their forms, they amount to queries that return a 
 * set of files on one or more filesystems, based on searches
 * of file metadata and simple properties (name, creation date,
 * owner etc).
 * There are several things we must take into account:
 * 
 * 1. Where do we store them? Do we delegate ownership to the evfs
 *    server, and 'associate' their location with a folder on a 
 *    filesystem?
 * 2. What update interval do we use? Do we allow this to be tuned
 *    on a server or vfolder level - e.g. 'live' vfolders where the 
 *    search takes place on-request, or do it in the background on 
 *    a pre-set time period.  If we had a filesystem hook here it 
 *    would be easier - but we don't.  At least not one that will
 *    work on anything but linux.
 * 3. What storage method do we use? I'm leaning towards eet, but
 *    xml would seem to be the better option - we need to represent
 *    'or', 'and', 'not' clauses, etc.
 */

typedef struct evfs_vfolder evfs_vfolder;
struct evfs_vfolder
{
   Evas_List *bases;            /* A list of base URIs where we start our search */
};

void evfs_vfolder_initialise();
evfs_vfolder *evfs_vfolder_new();

#endif
