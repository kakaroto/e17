#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

#include "engage.h"

/* name of the config file with directory to show in the tree */
#define engage_conf "/.e/apps/engage/applinks"
#define engage_mappings "/.e/apps/engage/mappings"

static void
userconfig_mappings_load(FILE * fp)
{
  char           *line = NULL;
  size_t          len = 0;
  size_t          read = 0;
  char           *name = NULL;
  char           *class = NULL;
  char           *icon = NULL;

  while ((read = getline(&line, &len, fp)) != -1) {
    if (*line == '#' || *line == '\n')
      continue;
    /* stripping line return */
    line[strlen(line) - 1] = 0;
    /* split of the line */
    class = (char *) strtok(line, "|");
    name = (char *) strtok(NULL, "|");
    icon = (char *) strtok(NULL, "|");

#if 0
    printf("line [%s]\n", line);
#endif

    if (class != NULL && name != NULL && icon != NULL) {
      od_icon_mapping_add(class, name, icon);
    }
  }
  /* cleaning memory */
  if (line)
    free(line);
  line = NULL;
}

static void
userconfig_applinks_load(FILE * fp)
{
  char           *line = NULL;
  size_t          len = 0;
  size_t          read = 0;
  char           *exe = NULL;
  char           *name = NULL;
  char           *icon = NULL;

  while ((read = getline(&line, &len, fp)) != -1) {
    if (*line == '#' || *line == '\n')
      continue;
    /* stripping line return */
    line[strlen(line) - 1] = 0;
    /* split of the line */
    exe = (char *) strtok(line, "|");
    name = (char *) strtok(NULL, "|");
    icon = (char *) strtok(NULL, "|");

#if 0
    printf("line [%s]\n", line);
#endif

    if (exe != NULL && name != NULL && exe[strlen(exe) - 1] != '#') {
      od_dock_add_applnk(od_icon_new_applnk(exe, name));
    } else if (exe != NULL && name != NULL && icon != NULL &&
               exe[strlen(exe) - 1] == '#') {
      exe[strlen(exe) - 1] = 0;
      od_dock_add_dicon(od_icon_new_dicon(exe, name, icon));
    }
  }
  /* cleaning memory */
  if (line)
    free(line);
}

static void
userconfig_sysicons_load(void)
{
  char            path[PATH_MAX];
  struct dirent  *next;
  DIR            *dp;
  char           *file_title, *file_path;
  int             title_len;

  snprintf(path, PATH_MAX, "%s/.e/apps/engage/sysicons", getenv("HOME"));
  dp = opendir((const char *) path);

  if (!dp)
    return;
  while (next = readdir(dp)) {
    if (!strstr(next->d_name + strlen(next->d_name) - 4, ".eet"))
      continue; 
    
    file_path = malloc(strlen(path) + strlen(next->d_name) + 2); // 2 = / + \0
    strcpy(file_path, path);
    strcat(file_path, "/");
    strcat(file_path, next->d_name);

    title_len = strlen(next->d_name) - 4;
    file_title = malloc(title_len + 1);
    strncpy(file_title, next->d_name, title_len);
    *(file_title + title_len) = '\0';

    od_dock_add_sysicon(od_icon_new_sysicon(""/*file_title*/, file_path));

    free(file_path);
    free(file_title);
  }
  closedir(dp);

}

int
userconfig_load()
{
  FILE           *fd = NULL;
  char            filename[PATH_MAX];

  snprintf(filename, PATH_MAX, "%s/.e/apps/engage/mappings", getenv("HOME"));
  if ((fd = fopen(filename, "r"))) {
    userconfig_mappings_load(fd);
    fclose(fd);
  }
  snprintf(filename, PATH_MAX, "%s/.e/apps/engage/applinks", getenv("HOME"));
  if ((fd = fopen(filename, "r"))) {
    userconfig_applinks_load(fd);
    fclose(fd);
  }

  userconfig_sysicons_load();
  /* nothing special */
  return (0);
}
