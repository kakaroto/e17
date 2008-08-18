#include "evfs.h"

EvfsFilereference* evfs_filereference_new(char* plugin, char* path, int filetype)
{
	EvfsFilereference* ref = NEW(EvfsFilereference);
	ref->path = strdup(path);
	ref->plugin_uri = strdup(plugin);
	ref->file_type = filetype;
	
	if (!evfs_object_client_is_get()) 
		ref->plugin = evfs_get_plugin_for_uri(evfs_server_get(), plugin);

	return ref;
}

/*Helper function for local files*/
EvfsFilereference* evfs_filereference_local_new(char* path)
{
	EvfsFilereference* ref = evfs_filereference_new("file", path, 0);
	return ref;
}

void evfs_filereference_stat_to_system(EvfsFilereference* file, struct stat* st) 
{
	/*Yes - ugly.  But the sizes are different, to accomodate all
	 * systems*/
	if (file->stat) {
		st->st_mode = file->stat->st_mode;
		st->st_uid = file->stat->st_uid;
		st->st_gid = file->stat->st_gid;
		st->st_size = file->stat->st_size;
		st->st_atime = file->stat->ist_atime;
		st->st_ctime = file->stat->ist_ctime;
		st->st_mtime = file->stat->ist_mtime;

	}
}

void evfs_stat_system_to_evfs_filereference(EvfsFilereference* file, struct stat* st) 
{
	/*Yes - ugly.  But the sizes are different, to accomodate all
	 * systems*/
	if (file->stat) {
		free(file->stat);
	}
	file->stat = NEW(EvfsStat);

	file->stat->st_mode = st->st_mode;
	file->stat->st_uid = st->st_uid;
	file->stat->st_gid = st->st_gid;
	file->stat->st_size = st->st_size;
	file->stat->ist_atime = st->st_atime;
	file->stat->ist_ctime = st->st_ctime;
	file->stat->ist_mtime = st->st_mtime;

}
