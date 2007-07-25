#include "order.h"
#include "e.h"

#include <stdio.h>

char *_od_dotorder_locate(void) {
  char *homedir;
  char buf[4096];

  homedir = e_user_homedir_get();
  if (homedir) {
    snprintf(buf, sizeof(buf), "%s/.e/e/applications/bar/engage/.order", homedir);
    free(homedir);
    return strdup(buf);
  }
  return NULL;
}

void _od_dotorder_app_add(const char *name) {
  FILE *f;
  char *dotorder;

  dotorder = _od_dotorder_locate();
  if (!dotorder)
    return;
  f = fopen(dotorder, "ab");
  if (f) {
    fputs("\n", f);
    fputs(name, f);
    fclose(f);
  }
  free(dotorder);
}

void _od_dotorder_app_del(const char *name) {
  FILE *f;
  char buf[4096];
  char *buf_ptr;
  char *dotorder;
  Ecore_List *list = ecore_list_new();

  dotorder = _od_dotorder_locate();
  if (!dotorder)
    return;

  if ((f = fopen(dotorder, "r+")) == NULL)
    return;

  while (fgets(buf, 4096, f)) {
    if (strncmp(name, buf, strlen(name)) != 0)
      ecore_list_append(list, strdup(buf));
  }
  fclose(f);
  ecore_list_first_goto(list);

  if ((f = fopen(dotorder, "w")) == NULL)
    return;

  while((buf_ptr = ecore_list_next(list))) {
    snprintf(buf, 4096, "%s", buf_ptr);
    fputs(buf_ptr, f);
    free(buf_ptr);
  }
  free(dotorder);
  fclose(f);
  ecore_list_destroy(list);
}

