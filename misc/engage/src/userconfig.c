#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

#include "engage.h"

E_App *od_unmatched_app;

static void
userconfig_mappings_load(char* fp)
{
  E_App          *maplist;
  Evas_List      *l;

  od_unmatched_app = e_app_new(PACKAGE_DATA_DIR "/icons/xapp.eet", 0);
  if (!od_unmatched_app) {
    printf("ERROR, you must make install\n");
    ecore_main_loop_quit();
    exit(1);
  }

  maplist = e_app_new(fp, 1);
  if (!maplist) {
    printf("You should _really_ create %s and populate it\n", fp);
    return;
  }

  /*FIXME: only 1 level deep... */
  l = maplist->subapps;
  while(l) {
    E_App *ptr = l->data;

    od_icon_mapping_add(ptr);
    l = l->next;
  }

}

static void
userconfig_applinks_load(char* fp)
{
  E_App          *applist;
  Evas_List      *l;

  applist = e_app_new(fp, 0);
  if (!applist) {
    printf("You should _really_ create %s and populate it\n", fp);
    return;
  }
  e_app_subdir_scan(applist, 0);
  
  l = applist->subapps;
  while(l) {
    E_App *ptr = l->data;

    od_dock_add_applnk(od_icon_new_applnk(ptr, NULL, NULL));
    l = l->next;
  }
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

    od_dock_add_sysicon(od_icon_new_sysicon("", file_path));

    free(file_path);
    free(file_title);
  }
  closedir(dp);

}

int
userconfig_load()
{
  char            filename[PATH_MAX];

  snprintf(filename, PATH_MAX, "%s/.e/apps/engage/mapping", getenv("HOME"));
  userconfig_mappings_load(filename);

  snprintf(filename, PATH_MAX, "%s/.e/apps/engage/launcher", getenv("HOME"));
  userconfig_applinks_load(filename);

  userconfig_sysicons_load();
  /* nothing special */
  return (0);
}
