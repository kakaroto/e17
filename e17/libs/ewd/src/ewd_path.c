#include <Ewd.h>

static Ewd_List *group_list = NULL;

Ewd_Path_Group *__ewd_path_group_find(char *name);
Ewd_Path_Group *__ewd_path_group_find_id(int id);

/**
 * ewd_path_group_new - create a new path group
 * @group_name: the name of the new group
 *
 * Returns 0 on error, the integer id of the new group on success.
 */
int
ewd_path_group_new(char *group_name)
{
	Ewd_Path_Group *last;
	Ewd_Path_Group *group;

	CHECK_PARAM_POINTER_RETURN("group_name", group_name, -1);

	if (!group_list) {
		  group_list = ewd_list_new();
	}
	else {
		  group = __ewd_path_group_find(group_name);
		  if (group)
			  return -1;
	}

	group = (Ewd_Path_Group *)malloc(sizeof(Ewd_Path_Group));
	memset(group, 0, sizeof(Ewd_Path_Group));

	group->name = strdup(group_name);
	ewd_list_append(group_list, group);

	last = ewd_list_goto_last(group_list);
	group->id = last->id + 1;

	return group->id;
}

/**
 * ewd_path_group_del - destroy a previously created path group
 * @group_id: the unique identifier for the group
 *
 * Returns no value.
 */
void
ewd_path_group_del(int group_id)
{
	Ewd_Path_Group *group;

	group = __ewd_path_group_find_id(group_id);

	if (!group)
		return;

	if (group->paths) {
		ewd_list_for_each(group->paths,
				  EWD_FOR_EACH(free));
		ewd_list_destroy(group->paths);
	}

	free(group->name);
	free(group);
}

/**
 * ewd_path_group_add - add a directory to be searched for files
 * @group_id: the unique identifier for the group to add the path
 * @path: the new path to be added to the group
 *
 * Returns no value.
 */
void
ewd_path_group_add(int group_id, char *path)
{
	Ewd_Path_Group *group;

	CHECK_PARAM_POINTER("path", path);

	group = __ewd_path_group_find_id(group_id);

	if (!group)
		return;

	if (!group->paths)
		group->paths = ewd_list_new();

	ewd_list_append(group->paths, strdup(path));
}

/**
 * ewd_path_group_remove - remove a directory to be searched for files
 * @group_id: the identifier for the group to remove a path
 * @path: the path to be removed from @group_id
 *
 * Returns no value. Removes @path from the list of directories to search for
 * files.
 */
void
ewd_path_group_remove(int group_id, char *path)
{
	char *found;
	Ewd_Path_Group *group;

	CHECK_PARAM_POINTER("path", path);

	group = __ewd_path_group_find_id(group_id);

	if (!group || !group->paths)
		return;

	/*
	 * Find the path in the list of available paths
	 */
	ewd_list_goto_first(group->paths);

	while ((found = ewd_list_current(group->paths))
	       && strcmp(found, path))
		ewd_list_next(group->paths);

	/*
	 * If the path is found, remove and free it
	 */
	if (found) {
		  ewd_list_remove(group->paths);
		  free(found);
	}
}

/**
 * ewd_path_group_find - find a file in a group of paths
 * @group_id - the path group id to search for @file
 * @file: the name of the file to find in the path group @group_id
 *
 * Returns a pointer to a newly allocated path location of the found file
 * on success, NULL on failure.
 */
char *
ewd_path_group_find(int group_id, char *name)
{
	char *p;
	struct stat st;
	char path[PATH_MAX];
	Ewd_Path_Group *group;

	CHECK_PARAM_POINTER_RETURN("name", name, NULL);

	group = __ewd_path_group_find_id(group_id);

	/*
	 * Search the paths of the path group for the specified file name
	 */
	ewd_list_goto_first(group->paths);
	p = ewd_list_next(group->paths);
	do {
		snprintf(path, PATH_MAX, "%s/%s", p, name);
		stat(path, &st);
	} while (!S_ISREG(st.st_mode) && (p = ewd_list_next(group->paths)));

	if (p)
		p = strdup(path);

	return p;
}

/**
 * ewd_path_group_available - get a list of all available files in the path
 * @group_id: the identifier for the path to get all available files
 *
 * Returns a pointer to a newly allocated list of all files found in the paths
 * identified by @group_id, NULL on failure.
 */
Ewd_List *
ewd_path_group_available(int group_id)
{
	Ewd_List *avail = NULL;
	Ewd_Path_Group *group;
	char *path;

	group = __ewd_path_group_find_id(group_id);

	if (!group || !group->paths || ewd_list_is_empty(group->paths))
		return NULL;

	ewd_list_goto_first(group->paths);

	while ((path = ewd_list_next(group->paths)) != NULL)
	  {
		DIR *dir;
		struct stat st;
		struct dirent *d;

		stat(path, &st);

		if (!S_ISDIR(st.st_mode))
			continue;

		dir = opendir(path);

		if (!dir)
			continue;

		while ((d = readdir(dir)) != NULL)
		  {
			char ppath[PATH_MAX];
			char *ext;
			char n[PATH_MAX];
			int l;

			if (!strncmp(d->d_name, ".", 1))
				continue;

			ext = strrchr(d->d_name, '.');

			if (!ext || strncmp(ext, ".so", 3))
				continue;

			snprintf(ppath, PATH_MAX, "%s/%s", path, d->d_name);

			stat(ppath, &st);

			if (!S_ISREG(st.st_mode))
				continue;

			l = strlen(d->d_name);

			strncpy(n, d->d_name, l - 2);

			if (!avail)
				avail = ewd_list_new();

			ewd_list_append(avail, strdup(n));
		  }
	  }

	return avail;
}

/*
 * Find the specified group name
 */
Ewd_Path_Group *
__ewd_path_group_find(char *name)
{
	Ewd_Path_Group *group;

	CHECK_PARAM_POINTER_RETURN("name", name, NULL);

	ewd_list_goto_first(group_list);

	while ((group = ewd_list_next(group_list)) != NULL)
		if (!strcmp(group->name, name))
			return group;

	return NULL;
}

/*
 * Find the specified group id
 */
Ewd_Path_Group *
__ewd_path_group_find_id(int id)
{
	Ewd_Path_Group *group;

	ewd_list_goto_first(group_list);

	while ((group = ewd_list_next(group_list)) != NULL)
		if (group->id == id)
			return group;

	return NULL;
}
