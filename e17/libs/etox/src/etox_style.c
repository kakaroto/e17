#include "Etox_private.h"
#include "Etox.h"

static Ewd_List *path_list = NULL;

void
etox_style_init ()
{
  char global_path[PATH_MAX];
  char user_path[PATH_MAX];

  path_list = ewd_list_new ();
  ewd_list_set_free_cb (path_list, EWD_FREE_CB (free));

  snprintf (global_path, PATH_MAX, PACKAGE_DATA_DIR "/style");
  etox_style_add_path (global_path);

  snprintf (user_path, PATH_MAX, "%s/.e/etox/style", getenv ("HOME"));
  etox_style_add_path (user_path);
}

void
etox_style_add_path (char *path)
{
  if (!path)
    return;

  if (!path_list)
    etox_style_init ();

  ewd_list_append (path_list, strdup (path));
}

void
etox_style_del_path (char *path)
{
  char *tmp;

  if (!path || !path_list)
    return;

  ewd_list_goto_first (path_list);
  while ((tmp = (char *) ewd_list_next (path_list)))
    if (!strcmp (tmp, path))
      {
	if (ewd_list_goto (path_list, tmp))
	  ewd_list_remove (path_list);
	FREE (tmp);
      }
}

Ewd_List *
etox_style_get_paths (void)
{
  return path_list;
}


Etox_Style
etox_style_new (char *name)
{
  Etox_Style style;
  Etox_Style_Bit bit;
  FILE *font_file;
  char s[4096];
  int i1, i2, i3, fields;
  char s2[4096];
  char nbuf[4096];
  char *tmp;

  if (!path_list)
    etox_style_init ();

  style = malloc (sizeof (struct _Etox_Style));

  if (name)
    style->name = strdup (name);
  else
    style->name = NULL;

  style->bits = ewd_list_new ();
  ewd_list_set_free_cb (style->bits, EWD_FREE_CB (free));

  /* Look for the style file */

  if (!_etox_loadfile_is_good (name))
    {
      int ok = 0;

      if (!strstr (name, ".style"))
	{
	  strncpy (nbuf, name, 4000);
	  strcat (nbuf, ".style");

	  ok = _etox_loadfile_is_good (nbuf);
	}

      ewd_list_goto_first (path_list);
      while ((tmp = (char *) ewd_list_next (path_list)))
	{
	  if (ok)
	    break;
	  strncpy (nbuf, tmp, 4000);
	  if (!(nbuf[strlen (nbuf) - 1] == '/'))
	    strcat (nbuf, "/");
	  strncat (nbuf, name, (4000 - strlen (nbuf)));
	  ok = _etox_loadfile_is_good (nbuf);
	  if (!ok)
	    if (!strstr (name, ".style"))
	      {
		strcat (nbuf, ".style");

		ok = _etox_loadfile_is_good (nbuf);
	      }
	}
    }
  else
    strncpy (nbuf, name, 4000);

  font_file = fopen (nbuf, "r");
  while (_etox_loadfile_get_line (s, 4096, font_file))
    {
      i1 = i2 = i3 = 0;
      memset (s2, 0, 4096);
      fields = sscanf (s, "%4000[^=]= %i %i %i", s2, &i1, &i2, &i3);

      if (fields < 3)
	{
	  fclose (font_file);
	  return style;
	}

      bit = malloc (sizeof (struct _Etox_Style_Bit));

      if (!strcmp (s2, "sh"))
	bit->type = ETOX_STYLE_TYPE_SHADOW;
      if (!strcmp (s2, "fg"))
	bit->type = ETOX_STYLE_TYPE_FOREGROUND;
      if (!strcmp (s2, "ol"))
	bit->type = ETOX_STYLE_TYPE_OUTLINE;
      bit->x = i1;
      bit->y = i2;
      bit->a = i3;

      ewd_list_append (style->bits, bit);
    }
  fclose (font_file);

  _etox_get_style_offsets (style, &(style->offset_w), &(style->offset_h));

  return style;
}

void
etox_style_free (Etox_Style style)
{
  if (!style)
    return;

  ewd_list_destroy (style->bits);
  FREE (style);
}
