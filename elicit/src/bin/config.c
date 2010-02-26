#include <Ecore_File.h>
#include "elicit.h"
#include "config.h"

int
elicit_config_load(Elicit *el)
{
  FILE *f;
  char buf[PATH_MAX];
  int line;

  /* defaults for things not loaded from disk */
  el->conf.x = 0;
  el->conf.y = 0;
  el->conf.w = 200;
  el->conf.h = 200;
  el->conf.zoom_level = 4;
  el->conf.grid_visible = 1;
  el->conf.show_band = 1;

  if (!el->path.conffile) return 0;

  f = fopen(el->path.conffile, "r");
  if (!f)
    return 0;

  line = 0;
  while (fgets(buf, sizeof(buf), f))
  {
    char *c;
    int len;
    char *key, *val;

    line++;
    len = strlen(buf);

    if (buf[0] == '#' || buf[0] == '\n')
      continue;

    /* chomp */
    if(buf[len-1] == '\n')
      buf[len-1] = '\0';

    key = NULL;
    val = NULL;
    for (c = buf; *c; c++)
    {
      key = buf;
      if (*c == ':')
      {
        *c = '\0';
        val = c + 1;
      }
    }

    if (!key || !val)
    {
      fprintf(stderr, "[Elicit] Invalid line in config file (%d)\n", line);
      continue;
    }

    /* strip spaces */
    while (*val == ' ') val++;

    if (!strcmp(key, "theme"))
    {
      el->conf.theme = strdup(val);
    }
    else if (!strcmp(key, "geometry"))
    {
      sscanf(val, "%dx%d%d%d\n", &(el->conf.w), &(el->conf.h), &(el->conf.x), &(el->conf.y));
    }
    else if (!strcmp(key, "color"))
    {
      color_hex_set(el->color, val);
    }
    else if (!strcmp(key, "zoom_level"))
    {
      el->conf.zoom_level = atoi(val);
      if (!el->conf.zoom_level)
        el->conf.zoom_level = 4;
    }
    else if (!strcmp(key, "grid_visible"))
    {
      el->conf.grid_visible = atoi(val);
    }
    else if (!strcmp(key, "show_band"))
    {
      el->conf.show_band = atoi(val);
    }
  }
  fclose(f);

  return 1;
}

int
elicit_config_save(Elicit *el)
{
  FILE *f;

  if (el->palette) palette_save(el->palette);

  if (!el->path.conffile) return 0;

  f = fopen(el->path.conffile, "w");
  if (!f) return 0;

  fprintf(f, "theme: %s\n", el->conf.theme);
  fprintf(f, "geometry: %dx%d%+d%+d\n", el->conf.w, el->conf.h, el->conf.x, el->conf.y);
  fprintf(f, "color: %s\n", color_hex_get(el->color, COLOR_HEX_HASH | COLOR_HEX_CAPS));
  fprintf(f, "zoom_level: %d\n", el->conf.zoom_level);
  fprintf(f, "grid_visible: %d\n", el->conf.grid_visible ? 1 :0);
  fprintf(f, "show_band: %d\n", el->conf.show_band ? 1 : 0);
  fclose(f);

  return 1;
}
