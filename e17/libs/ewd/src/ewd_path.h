#ifndef _EWD_PATH_H
#define _EWD_PATH_H

typedef struct _ewd_path_group Ewd_Path_Group;
struct _ewd_path_group
{
	int id;
	char *name;
	Ewd_List *paths;
};

/*
 * Create a new path group
 */
int ewd_path_group_new(char *group_name);

/*
 * Destroy a previous path group
 */
void ewd_path_group_del(int group_id);

/*
 * Add a directory to be searched for files
 */
void ewd_path_group_add(int group_id, char *path);

/*
 * Remove a directory to be searched for files
 */
void ewd_path_group_remove(int group_id, char *path);

/*
 * Find the absolute path if it exists in the group of paths
 */
char * ewd_path_group_find(int group_id, char *name);

/*
 * Get a list of all the available files in a path set
 */
Ewd_List * ewd_path_group_available(int group_id);

#endif
