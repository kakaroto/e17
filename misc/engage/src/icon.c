#include "engage.h"
#include "limits.h"
// for stat
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#ifdef DMALLOC
#include "dmalloc.h"
#endif

Evas_List      *icon_mappings = NULL;
Evas_List      *icon_paths = NULL;

static OD_Icon *od_icon_new(const char *name, const char *icon_path);
static void     od_icon_mapping_get(const char *winclass, char **name, char **icon_name);       // DON'T free returned
static char    *od_icon_path_get(const char *icon_name);

OD_Icon        *
od_icon_new_applnk(const char *command, const char *winclass)
{
  char           *name, *icon_name, *icon_path;

  od_icon_mapping_get(winclass, &name, &icon_name);
  icon_path = od_icon_path_get(icon_name);
  OD_Icon        *ret = od_icon_new(name, icon_path);

  fprintf(stderr,
          "new applnk: name=\"%s\" winclass=\"%s\" icon_path=\"%s\" command=\"%s\"\n",
          name, winclass, icon_path, command);
  ret->type = application_link;
  ret->data.applnk.command = strdup(command);
  ret->data.applnk.winclass = strdup(winclass);
  ret->data.applnk.count = 0;
  free(icon_path);
  return ret;
}

OD_Icon        *
od_icon_new_dicon(const char *command, const char *name, const char *icon_name)
{
  char           *icon_path = od_icon_path_get(icon_name);
  OD_Icon        *ret = od_icon_new(name, icon_path);

  fprintf(stderr, "new dicon: name=\"%s\" icon_path=\"%s\" command=\"%s\"\n",
          name, icon_path, command);
  ret->type = docked_icon;
  ret->data.applnk.command = strdup(command);
  free(icon_path);
  return ret;
}

OD_Icon        *
od_icon_new_minwin(Ecore_X_Window win)
{
  char           *name, *icon_name;
  char           *winclass = od_wm_get_winclass(win);
  char           *title = od_wm_get_title(win);

  od_icon_mapping_get(winclass, &name, &icon_name);
  char           *icon_path = od_icon_path_get(icon_name);
  OD_Icon        *ret = od_icon_new(title, icon_path);

  fprintf(stderr, "new minwin: icon_path=\"%s\"\n", icon_path);
  ret->type = minimised_window;
  ret->data.minwin.window = win;
  free(winclass);
  free(title);
  free(icon_path);
  return ret;
}

OD_Icon        *
od_icon_new(const char *name, const char *icon_file)
{
  OD_Icon        *ret = (OD_Icon *) malloc(sizeof(OD_Icon));

  ret->name = strdup(name);
  ret->scale = 0.0;
  Evas_Object    *icon = ret->icon = evas_object_image_add(evas);
  Evas_Object    *tt_txt = ret->tt_txt = evas_object_text_add(evas);
  Evas_Object    *tt_shd = ret->tt_shd = evas_object_text_add(evas);

  ret->arrow = NULL;
  ret->state = 0;
  ret->appear_timer = NULL;

  evas_object_image_file_set(icon, icon_file, NULL);
  evas_object_image_alpha_set(icon, 1);
  evas_object_image_smooth_scale_set(icon, 1);
  evas_object_layer_set(icon, 100);
  evas_object_name_set(icon, "icon");
  evas_object_show(icon);

  evas_object_text_font_set(tt_txt, options.tt_fa, options.tt_fs);
  evas_object_text_text_set(tt_txt, name);
  evas_object_color_set(tt_txt,
                        (options.tt_txt_color >> 16) & 0xff,
                        (options.tt_txt_color >> 8) & 0xff,
                        (options.tt_txt_color >> 0) & 0xff, 255);
  evas_object_layer_set(tt_txt, 200);

  evas_object_text_font_set(tt_shd, options.tt_fa, options.tt_fs);
  evas_object_text_text_set(tt_shd, name);
  evas_object_color_set(tt_shd,
                        (options.tt_shd_color >> 16) & 0xff,
                        (options.tt_shd_color >> 8) & 0xff,
                        (options.tt_shd_color >> 0) & 0xff, 127);
  evas_object_layer_set(tt_shd, 199);

  return ret;
}

void
od_icon_del(OD_Icon * icon)
{
  switch (icon->type) {
  case application_link:
    free(icon->data.applnk.command);
    free(icon->data.applnk.winclass);
    break;
  case docked_icon:
    free(icon->data.dicon.command);
    break;
  case minimised_window:
    break;
  }

  evas_object_del(icon->icon);
  evas_object_del(icon->tt_txt);
  evas_object_del(icon->tt_shd);
  if (icon->arrow)
    evas_object_del(icon->arrow);
  free(icon->name);
  free(icon);
}

void
od_icon_arrow_show(OD_Icon * icon)
{
  if (icon->arrow)
    evas_object_show(icon->arrow);
  else {
    icon->arrow = evas_object_image_add(evas);
    int             height = (int) options.arrow_size;
    int             width = 1 + 2 * (int) options.arrow_size;
    int            *pattern = (int *) malloc(sizeof(int) * width * height);

    int             x, y;

    for (y = 0; y < height; y++) {
      for (x = 0; x < width; x++)
        pattern[y * width + x] = (x + y < 3 * height / 2 ||
                                  y - x <
                                  -height / 2 ? 0x00000000 : 0xff000000);
    }

    evas_object_image_alpha_set(icon->arrow, 1);
    evas_object_image_size_set(icon->arrow, width, height);
    evas_object_image_smooth_scale_set(icon->arrow, 0);
    evas_object_image_data_copy_set(icon->arrow, pattern);
    evas_object_image_data_update_add(icon->arrow, 0, 0, width, height);
    evas_object_image_fill_set(icon->arrow, 0.0, 0.0, width, height);
    evas_object_resize(icon->arrow, width, height);
    evas_object_layer_set(icon->arrow, 100);
    evas_object_show(icon->arrow);
    free(pattern);
  }
}

void
od_icon_arrow_hide(OD_Icon * icon)
{
  if (icon->arrow)
    evas_object_hide(icon->arrow);
}

void
od_icon_tt_show(OD_Icon * icon)
{
  evas_object_show(icon->tt_txt);
  evas_object_show(icon->tt_shd);
}

void
od_icon_tt_hide(OD_Icon * icon)
{
  evas_object_hide(icon->tt_txt);
  evas_object_hide(icon->tt_shd);
}

void
od_icon_name_change(OD_Icon * icon, const char *name)
{
  free(icon->name);
  icon->name = strdup(name);
  evas_object_text_text_set(icon->tt_txt, name);
  evas_object_text_text_set(icon->tt_shd, name);
  need_redraw = true;
}

void
od_icon_mapping_add(const char *winclass, const char *name,
                    const char *icon_name)
{
  icon_mappings = evas_list_append(icon_mappings, strdup(winclass));
  icon_mappings = evas_list_append(icon_mappings, strdup(name));
  icon_mappings = evas_list_append(icon_mappings, strdup(icon_name));
}

void
od_icon_mapping_get(const char *winclass, char **name, char **icon_name)
{
  printf("getting mapping for %s\n", winclass);
  Evas_List      *item = icon_mappings;

  while (item) {
    if (strcmp(winclass, (char *) item->data) == 0) {
      *name = (char *) item->next->data;
      *icon_name = (char *) item->next->next->data;
      return;
    }

    if (!(item = item->next)) {
      fprintf(stderr, "corrupt icon mappings, pos 1\n");
      exit(EXIT_FAILURE);
    }
    if (!(item = item->next)) {
      fprintf(stderr, "corrupt icon mappings, pos 2\n");
      exit(EXIT_FAILURE);
    }
    item = item->next;
  }
  *name = strdup(winclass);
  *icon_name = strdup(winclass);
}

char           *
od_icon_path_get(const char *icon_name)
{
  Evas_List      *item = icon_paths;

  while (item) {
    char           *path = (char *) item->data;
    char            buffer[strlen(path) + strlen(icon_name) + strlen(".png") + 2];      // one extra for '/', another for '\0'
    struct stat     dummy;

    strcpy(buffer, path);
    strcat(buffer, "/");
    strcat(buffer, icon_name);
    strcat(buffer, ".png\0");
    if (stat(buffer, &dummy) == 0)
      return strdup(buffer);
    item = item->next;
  }

  if (strcmp(icon_name, "xapp") != 0)
    return od_icon_path_get("xapp");
  else
    return NULL;
}

void
od_icon_add_path(const char *path)
{
  icon_paths = evas_list_append(icon_paths, strdup(path));
}

void
od_icon_add_kde_set(const char *path)
{
  static char    *sizes[] =
    { "128x128", "96x96", "64x64", "48x48", "32x32", "24x24", "16x16", "" };
  static char    *types[] = { "apps", "devices", "filesystems", "actions", "" };

  char          **size = sizes;

  while ((*size)[0] != '\0') {
    char          **type = types;

    while ((*type)[0] != '\0') {
      char            buffer[PATH_MAX];
      struct stat     dummy;

      strcpy(buffer, path);
      strcat(buffer, "/");
      strcat(buffer, *size);
      strcat(buffer, "/");
      strcat(buffer, *type);
      if (stat(buffer, &dummy) == 0)
        od_icon_add_path(buffer);
      type++;
    }
    size++;
  }
}
