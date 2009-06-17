#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

#include "engage.h"

E_App *od_unmatched_app, *applist, *maplist;

static void _userconfig_applinks_change(void *data, E_App *a, E_App_Change ch);

static void
userconfig_mappings_load(char* fp)
{
  Eina_List      *l;

  od_unmatched_app = e_app_new(PACKAGE_DATA_DIR "/icons/xapp.eap", 0);
  if (!od_unmatched_app) {
    printf("ERROR: Engage could not locate default icon xapp.eap - please make install\n");
    ecore_main_loop_quit();
    exit(1);
  }

  maplist = e_app_new(fp, 1);
  if (!maplist) {
    printf("You must create the %s/ directory and populate it with .eap files or a .order file\n", fp);
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
_userconfig_applinks_change(void *data, E_App *a, E_App_Change ch)
{
  OD_Icon *tmp = NULL;
  Eina_List *l;
  if (a->parent == applist) {
    switch (ch) {
      case E_APP_ADD:
        tmp = od_icon_new_applnk(a, NULL, NULL);
        tmp->launcher = 1;
        od_dock_add_applnk(tmp);
        break;
      case E_APP_DEL:
        l = dock.applnks;
        while (l) {
          if (strcmp(((OD_Icon *) l->data)->winclass, a->win_class) == 0) {
            tmp = l->data;
            break;
          }
          l = l->next;
        }
        if (tmp) {
          dock.icons = eina_list_remove(dock.icons, tmp);
          dock.applnks = eina_list_remove(dock.applnks, tmp);
          od_dock_reposition();
          od_icon_del(tmp);
        }
        break;
      case E_APP_CHANGE:
        /* this is a hack for now, if icon is changed see if it really is new.
         */
        l = dock.applnks;
        tmp = NULL;
        while (l) {
          if (strcmp(((OD_Icon *) l->data)->winclass, a->win_class) == 0) {
             tmp = l->data;
             break;
          }
          l = l->next;
        }
        if (!tmp) {
          tmp = od_icon_new_applnk(a, NULL, NULL);
          tmp->launcher = 1;
          od_dock_add_applnk(tmp);
        }
        break;
      default:
        printf("Unhandled callback on applinks %d\n", ch);
    }
  } else {
    // FIXME: handle mapping changes
  }
}
  
static void
userconfig_applinks_load(char* fp)
{
  Eina_List      *l;

  applist = e_app_new(fp, 0);
  if (!applist) {
    printf("You should _really_ create %s and populate it\n", fp);
    return;
  }
  e_app_subdir_scan(applist, 0);
  e_app_change_callback_add(_userconfig_applinks_change, NULL);
  
  l = applist->subapps;
  while(l) {
    E_App *ptr;
    OD_Icon *ico;
    ptr = l->data;
    ico = od_icon_new_applnk(ptr, NULL, NULL);
    ico->launcher = 1;

    od_dock_add_applnk(ico);
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
  while ((next = readdir(dp))) {
    if (!strstr(next->d_name + strlen(next->d_name) - 4, ".edj"))
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

  snprintf(filename, PATH_MAX, "%s/.e/e/applications/all", getenv("HOME"));
  userconfig_mappings_load(filename);

  snprintf(filename, PATH_MAX, "%s/.e/e/applications/bar/engage", getenv("HOME"));
  userconfig_applinks_load(filename);

  userconfig_sysicons_load();
  /* nothing special */
  return (0);
}
