#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "engage.h"

/* name of the config file with directory to show in the tree */
#define engage_conf "/.e/apps/engage/applinks"
#define engage_mappings "/.e/apps/engage/mappings"

int
userconfig_load()
{
  FILE           *fd = NULL;
  char           *homepath = NULL;
  char           *filename = NULL;
  char           *line = NULL;
  char           *exe = NULL;
  char           *name = NULL;
  char           *class = NULL;
  char           *icon = NULL;
  size_t          len = 0;
  size_t          read = 0;

  /* getting user environement */
  homepath = (char *) getenv("HOME");


  /* malloc for the config filename */
  filename =
    (char *) malloc(sizeof(char) *
                    (strlen(homepath) + strlen(engage_mappings)) + 1);
  memset(filename, 0, (strlen(homepath) + strlen(engage_mappings) + 1));

  /* filename is the string containing the full path for config file */
  filename = strcat(filename, homepath);
  filename = strcat(filename, engage_mappings);

  /* opening filename in read mode */
  fd = fopen(filename, "r");

  /* check that filename is open(able) */
  if (fd == NULL)
    return (1);
  /* reading filename while EOF */
  while ((read = getline(&line, &len, fd)) != -1) {
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

  if (filename)
    free(filename);

  /* close filename */
  fclose(fd);




  /* malloc for the config filename */
  filename =
    (char *) malloc(sizeof(char) * (strlen(homepath) + strlen(engage_conf)) +
                    1);
  memset(filename, 0, (strlen(homepath) + strlen(engage_conf) + 1));

  /* filename is the string containing the full path for config file */
  filename = strcat(filename, homepath);
  filename = strcat(filename, engage_conf);

  /* opening filename in read mode */
  fd = fopen(filename, "r");

  /* check that filename is open(able) */
  if (fd == NULL)
    return (1);
  /* reading filename while EOF */
  while ((read = getline(&line, &len, fd)) != -1) {
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

  if (filename)
    free(filename);

  /* close filename */
  fclose(fd);

  /* nothing special */
  return (0);
}
