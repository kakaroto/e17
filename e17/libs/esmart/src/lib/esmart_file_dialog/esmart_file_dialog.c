/**
 * @file esmart_file_dialog.c
 *
 */
#include <Evas.h>
#include <Edje.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include "../esmart_container/Esmart_Container.h"
#include "../esmart_text_entry/Esmart_Text_Entry.h"
#include "Esmart_File_Dialog.h"

#include "../../config.h"

  typedef struct _Esmart_File_Dialog Esmart_File_Dialog;

  struct _Esmart_File_Dialog
  {
    Evas_Object *clip;		/* clipped area for the file dialog */
    Evas_Object *edje;		/* the edje file we load groups from */
    Evas_Object *directories;	/* directory containers */
    char *directories_dragbar;	/* dragable part name for the
				 * directories */
    Evas_Object *files;		/* file container */
    char *files_dragbar;	/* the dragable for the file container */

    Evas_Object *entry;		/* Esmart_Text_Entry */
    char *path;			/* the cwd for the dialog */

    /* the client callback for intercepting file dialog specific stuff */
    void (*func) (void *data, Evas_Object * edje, Esmart_File_Dialog_Op type);
    /* the data that's passed to the file dialog callback */
    void *fdata;

    /* the current geometry/location of the file dialog */
    Evas_Coord x, y, w, h;

    /* the files the user wants to load/open/save(?)/ */
    Evas_List *selections;
  };

/*========================================================================*
 * Static Function Prototypes
 *========================================================================*/
static int sort_cb (Evas_Object *d1, Evas_Object *d2);
static void interp_return_key (void *data, const char *str);

#if 0
/* unused */
static void interp_tab_key (void *data, const char *str);
#endif

static void
_esmart_file_dialog_entry_focus_cb (void *data, Evas_Object * o,
				    const char *emission, const char *source);
static void
_esmart_file_dialog_scroll_cb (void *data, Evas_Object * o,
			       const char *emission, const char *source);
static void
_esmart_file_dialog_scrollbar_cb (void *data, Evas_Object * o,
				  const char *emission, const char *source);
static int
__esmart_file_dialog_directory_set_test (Evas_Object * o, const char *dir);
static void
_esmart_file_dialog_directory_cb (void *data, Evas_Object * o,
				  const char *emission, const char *source);
static void
_esmart_file_dialog_file_cb (void *data, Evas_Object * o,
			     const char *emission, const char *source);
static void
_esmart_file_dialog_global_button_cb (void *data, Evas_Object * o,
				      const char *emission,
				      const char *source);
static void
_esmart_file_dialog_file_button_cb (void *data, Evas_Object * o,
				    const char *emission, const char *source);
/*========================================================================*
 * Evas Smart Object Function Prototypes
 *========================================================================*/
static Evas_Smart *_esmart_file_dialog_object_smart_get (void);
static Evas_Object *esmart_file_dialog_object_new (Evas * evas);
static void _esmart_file_dialog_object_add (Evas_Object * o);
static void _esmart_file_dialog_object_del (Evas_Object * o);
static void _esmart_file_dialog_object_layer_set (Evas_Object * o, int l);
static void _esmart_file_dialog_object_raise (Evas_Object * o);
static void _esmart_file_dialog_object_lower (Evas_Object * o);
static void _esmart_file_dialog_object_stack_above (Evas_Object * o,
						    Evas_Object * above);
static void _esmart_file_dialog_object_stack_below (Evas_Object * o,
						    Evas_Object * below);
static void _esmart_file_dialog_object_move (Evas_Object * o, Evas_Coord x,
					     Evas_Coord y);
static void _esmart_file_dialog_object_resize (Evas_Object * o, Evas_Coord w,
					       Evas_Coord h);
static void _esmart_file_dialog_object_show (Evas_Object * o);
static void _esmart_file_dialog_object_hide (Evas_Object * o);
static void _esmart_file_dialog_object_color_set (Evas_Object * o, int r,
						  int g, int b, int a);
static void _esmart_file_dialog_object_clip_set (Evas_Object * o,
						 Evas_Object * clip);
static void _esmart_file_dialog_object_clip_unset (Evas_Object * o);

/*=========================================================================*
 * Public Functions
 *=========================================================================*/
EAPI Evas_Object *
esmart_file_dialog_new (Evas * e, const char *edje_file)
{
  const char *part = NULL;
  const char *str = NULL;
  char buf[PATH_MAX];
  Evas_Object *result = NULL;
  Evas_Object *entry = NULL;
  Evas_Object *container = NULL;
  Esmart_File_Dialog *data = NULL;

  if ((result = esmart_file_dialog_object_new (e)))
    {
      if ((data = evas_object_smart_data_get (result)))
	{
	  data->edje = edje_object_add (e);
	  snprintf (buf, PATH_MAX, "%s", edje_file);
	  if (!edje_object_file_set (data->edje, buf, "esmart.filedialog"))
	    {
	      evas_object_del (data->edje);
	      evas_object_del (result);
	      data->edje = NULL;
	      result = NULL;
	    }
	  else
	    {
	      if ((part = edje_object_data_get (data->edje,
						"e,fd,container,files")))
		{
		  if (edje_object_part_exists (data->edje, part))
		    {
		      container = esmart_container_new (e);
		      esmart_container_direction_set (container, 1);
		      esmart_container_fill_policy_set (container,
							CONTAINER_FILL_POLICY_FILL_X);
		      if ((str = edje_object_data_get (data->edje,
						       "e,fd,container,files,direction")))
			{
			  if (!strcmp (str, "horizontal"))
			    {
			      esmart_container_direction_set (container, 0);
			      esmart_container_fill_policy_set (container,
								CONTAINER_FILL_POLICY_FILL_Y);
			    }
			}
		      if ((str = edje_object_data_get (data->edje,
						       "e,fd,container,files,dragbar")))
			{
			  data->files_dragbar = strdup (str);
			}
		      evas_object_show (container);
		      data->files = container;
		      edje_object_part_swallow (data->edje, part, container);
		      fprintf (stderr, "Added files container\n");
		    }
		}
	      if ((part = edje_object_data_get (data->edje,
						"e,fd,container,directory")))
		{
		  if (edje_object_part_exists (data->edje, part))
		    {
		      container = esmart_container_new (e);
		      esmart_container_direction_set (container, 1);
		      esmart_container_fill_policy_set (container,
							CONTAINER_FILL_POLICY_FILL_X);
		      if ((str = edje_object_data_get (data->edje,
						       "e,fd,container,directory,direction")))
			{
			  if (!strcmp (str, "horizontal"))
			    {
			      esmart_container_direction_set (container, 0);
			      esmart_container_fill_policy_set (container,
								CONTAINER_FILL_POLICY_FILL_Y);
			    }
			}
		      if ((str = edje_object_data_get (data->edje,
						       "e,fd,container,directory,dragbar")))
			{
			  data->directories_dragbar = strdup (str);
			}
		      evas_object_show (container);
		      data->directories = container;
		      edje_object_part_swallow (data->edje, part, container);
		      fprintf (stderr, "Added direcotires container\n");
		    }
		  else
		    {
		      fprintf (stderr,
			       "Error loading directory container %s\n",
			       part);
		    }
		}
	      if ((part = edje_object_data_get (data->edje, "e,fd,entry")))
		{
		  if (edje_object_part_exists (data->edje, part))
		    {
		      entry = esmart_text_entry_new (e);
		      esmart_text_entry_max_chars_set (entry, PATH_MAX);
		      esmart_text_entry_is_password_set (entry, 0);
		      evas_object_focus_set (entry, 0);
		      esmart_text_entry_return_key_callback_set (entry,
								 interp_return_key,
								 result);

		      evas_object_show (entry);
		      data->entry = entry;
		      esmart_text_entry_edje_part_set (entry, data->edje,
						       part);
		      fprintf (stderr, "Added selection entry\n");
		    }
		}
	      evas_object_show (data->edje);
	      evas_object_clip_set (data->edje, data->clip);
	      edje_object_signal_callback_add (data->edje,
					       "e,fd,button,global,*", "*",
					       _esmart_file_dialog_global_button_cb,
					       result);
	      edje_object_signal_callback_add (data->edje,
					       "e,fd,button,file,*", "*",
					       _esmart_file_dialog_file_button_cb,
					       result);
	      edje_object_signal_callback_add (data->edje,
					       "e,fd,container,scroll,*", "*",
					       _esmart_file_dialog_scroll_cb,
					       result);
	      edje_object_signal_callback_add (data->edje,
					       "e,fd,container,scrollbar,*",
					       "*",
					       _esmart_file_dialog_scrollbar_cb,
					       result);
	      edje_object_signal_callback_add (data->edje,
					       "e,fd,entry,*", "*",
					       _esmart_file_dialog_entry_focus_cb,
					       result);
	      snprintf (buf, PATH_MAX, "%s", getenv ("PWD"));
	      data->path = strdup (buf);
	    }
	}
    }
  return (result);
}

EAPI Evas_Object *
esmart_file_dialog_edje_get (Evas_Object * efd)
{
  Esmart_File_Dialog *fddata;
  Evas_Object *result = NULL;

  if ((fddata = evas_object_smart_data_get (efd)))
    {
      result = fddata->edje;
    }
  return (result);
}

EAPI void
esmart_file_dialog_callback_add (Evas_Object * efd,
				 void (*func) (void *data, Evas_Object * edje,
					       Esmart_File_Dialog_Op type), void *data)
{
  Esmart_File_Dialog *fddata;

  if ((fddata = evas_object_smart_data_get (efd)))
    {
      fddata->func = func;
      fddata->fdata = data;
    }
}
EAPI Evas_List *
esmart_file_dialog_selections_get (Evas_Object * efd)
{
  Esmart_File_Dialog *fddata;
  Evas_List *result = NULL;

  if ((fddata = evas_object_smart_data_get (efd)))
    {
      result = fddata->selections;
    }
  return (result);
}
EAPI const char *
esmart_file_dialog_current_directory_get (Evas_Object * efd)
{
  Esmart_File_Dialog *fddata;
  const char *result = NULL;

  if ((fddata = evas_object_smart_data_get (efd)))
    {
      result = fddata->path;
    }
  return (result);
}

/*=========================================================================*
 * Private Functions
 *=========================================================================*/
static void
interp_return_key (void *data, const char *str)
{
  if (data && str)
    {
      fprintf (stderr, "%s\n", str);
    }
}

#if 0
/* unused */
static void
interp_tab_key (void *data, const char *str)
{
  if (data && str)
    {
      fprintf (stderr, "Tab %s\n", str);
    }
}
#endif

static int
sort_cb (Evas_Object *o, Evas_Object *oo)
{
  const char *txt = NULL, *txt2 = NULL;

  if (!o)
    return (1);
  if (!oo)
    return (-1);

  if ((txt = evas_object_data_get (o, "name")))
    {
      if ((txt2 = evas_object_data_get (oo, "name")))
	{
	  return (strcmp (txt, txt2));
	}
    }
  return (1);

}

static void
_esmart_file_dialog_entry_focus_cb (void *data, Evas_Object * o,
				    const char *emission, const char *source)
{
  Esmart_File_Dialog *fddata = NULL;

  if ((fddata = evas_object_smart_data_get ((Evas_Object *) data)))
    {
      if (!strcmp ("e,fd,entry,focus,in,selection", emission))
	{
	  evas_object_focus_set (fddata->entry, 1);
	}
      else if (!strcmp ("e,fd,entry,focus,out,selection", emission))
	{
	  evas_object_focus_set (fddata->entry, 0);
	}
      fprintf (stderr, "%s : %s\n", emission, source);
    }
}
static void
_esmart_file_dialog_scroll_cb (void *data, Evas_Object * o,
			       const char *emission, const char *source)
{
  fprintf (stderr, "SCROLLBAR: %s : %s\n", emission, source);
}

static void
_esmart_file_dialog_scrollbar_cb (void *data, Evas_Object * o,
				  const char *emission, const char *source)
{
  double dx, dy;
  int scroll = 0;
  int length = 0;
  Evas_Object *oo = NULL;
  Evas_Object *obj = NULL;
  Evas_Coord cx, cy, cw, ch;
  Esmart_File_Dialog *fddata;

  if (!(obj = (Evas_Object *) data))
    return;

  if ((fddata = evas_object_smart_data_get (obj)))
    {
      if ((fddata->files_dragbar) && !strcmp (fddata->files_dragbar, source))
	{
	  oo = fddata->files;
	  fprintf (stderr, "Files SCROLL: %s : %s\n", emission, source);
	}
      else if ((fddata->directories_dragbar) &&
	       !strcmp (fddata->directories_dragbar, source))
	{
	  oo = fddata->directories;
	  fprintf (stderr, "Directory SCROLL: %s : %s\n", emission, source);
	}
      else
	{
	  fprintf (stderr, "Unknown SCROLL: %s : %s\n", emission, source);
	}
      if (oo)
	{
	  length = esmart_container_elements_length_get (oo);
	  edje_object_part_drag_value_get (o, source, &dx, &dy);
	  evas_object_geometry_get (oo, &cx, &cy, &cw, &ch);
	  if (esmart_container_direction_get (oo) > 0)
	    {
	      if (length < ch)
		return;
	      scroll = (int) (dy * (length - ch));
	    }
	  else
	    {
	      if (length < cw)
		return;
	      scroll = (int) (dx * (length - cw));
	    }
	  esmart_container_scroll_offset_set (oo, -scroll);
	}
    }
}
static int
__esmart_file_dialog_directory_set_test (Evas_Object * o, const char *dir)
{
  DIR *dirp = NULL;
  struct stat status;

  if (!stat (dir, &status))
    {
      if ((dirp = opendir (dir)))
	{
	  closedir (dirp);
	  return (0);
	}
    }
  else
    {
      fprintf (stderr, "errno is %d\n", errno);
    }
  return (1);
}
static void
_esmart_file_dialog_directory_cb (void *data, Evas_Object * o,
				  const char *emission, const char *source)
{
  const char *part = NULL;
  char buf[PATH_MAX], path[PATH_MAX];
  const char *txt = NULL;
  Esmart_File_Dialog *fddata;
  Evas_Object *obj = NULL;
  if (!(obj = (Evas_Object *) data))
    return;

  if ((fddata = evas_object_smart_data_get (obj)))
    {
      if ((txt = evas_object_data_get (o, "name")))
	{
	  if (fddata->path)
	    {
	      if ((!strcmp (emission, "e,fd,directory,load")))
		{
		  snprintf (buf, PATH_MAX, "%s/%s", fddata->path, txt);
		  if ((realpath (buf, &path[0])))
		    {
		      if (__esmart_file_dialog_directory_set_test (obj, buf))
			return;

		      free (fddata->path);
		      fddata->path = strdup (path);
		      fddata->selections =
			evas_list_free (fddata->selections);
		      if (edje_object_part_exists
			  (fddata->edje, fddata->directories_dragbar))
			{
			  edje_object_part_drag_value_set (fddata->edje,
							   fddata->
							   directories_dragbar,
							   0.0, 0.0);
			}
		      if (edje_object_part_exists (fddata->edje,
						   fddata->files_dragbar))
			{
			  edje_object_part_drag_value_set (fddata->edje,
							   fddata->
							   files_dragbar, 0.0,
							   0.0);
			}
		      if ((part = edje_object_data_get (fddata->edje,
							"e,fd,entry")))
			{
			  edje_object_part_text_set (fddata->edje, part, "");
			}
		      if (fddata->directories)
			esmart_container_empty (fddata->directories);
		      if (fddata->files)
			esmart_container_empty (fddata->files);

		      evas_object_show (obj);
#if 0
		      fprintf (stderr, "New Path is %s\n", fddata->path);
#endif
		    }
		}
	      else if ((!strcmp (emission, "e,fd,directory,load,recursive")))
		{
		  snprintf (buf, PATH_MAX, "%s/%s", fddata->path, txt);
		  if ((realpath (buf, &path[0])))
		    {
		      if (__esmart_file_dialog_directory_set_test (obj, buf))
			return;

		      if ((part = edje_object_data_get (fddata->edje,
							"e,fd,entry")))
			{
			  edje_object_part_text_set (fddata->edje, part, txt);
			}
		      fddata->selections =
			evas_list_append (fddata->selections, txt);
		      fddata->func (fddata->fdata, obj, ESMART_FILE_DIALOG_OK);
		      fddata->selections =
			evas_list_free (fddata->selections);
		    }
		}
	      else if ((!strcmp (emission, "e,fd,directory,selected")))
		{
		  if ((part = edje_object_data_get (fddata->edje,
						    "e,fd,entry")))
		    {
		      edje_object_part_text_set (fddata->edje, part, txt);
		    }
		  fddata->selections =
		    evas_list_append (fddata->selections, txt);
		}
	      else if (!strcmp ("e,fd,directory,unselected", emission))
		{
		  fddata->selections =
		    evas_list_remove (fddata->selections, txt);
		}
	      fprintf (stderr, "%s\n", emission);
	    }
	}
    }
}
static void
_esmart_file_dialog_file_cb (void *data, Evas_Object * o,
			     const char *emission, const char *source)
{
  const char *txt = NULL;
  const char *part = NULL;
  Esmart_File_Dialog *fddata;
  Evas_Object *obj = NULL;
  if (!(obj = (Evas_Object *) data))
    return;

  if ((fddata = evas_object_smart_data_get (obj)))
    {
      if ((txt = evas_object_data_get (o, "name")))
	{
	  if (!strcmp ("e,fd,file,selected", emission))
	    {
	      if ((part = edje_object_data_get (fddata->edje,
						"e,fd,entry,focus,in,selection")))
		{
		  edje_object_part_text_set (fddata->edje, part, txt);
		}
	      fddata->selections = evas_list_append (fddata->selections, txt);
	    }
	  else if (!strcmp ("e,fd,file,unselected", emission))
	    {
	      fddata->selections = evas_list_remove (fddata->selections, txt);
	    }
	  else if (!strcmp ("e,fd,file,load", emission))
	    {
	      fprintf (stderr, "Load Request: %s\n", txt);
	      fddata->selections = evas_list_append (fddata->selections, txt);
	      fddata->func (fddata->fdata, obj, ESMART_FILE_DIALOG_OK);
	      fddata->selections = evas_list_free (fddata->selections);
	    }
	}
#if 0
      fprintf (stderr, "%s\n", emission);
#endif
    }
}
static void
_esmart_file_dialog_global_button_cb (void *data, Evas_Object * o,
				      const char *emission,
				      const char *source)
{
  Esmart_File_Dialog *fddata;
  Evas_Object *obj = NULL;
  if (!(obj = (Evas_Object *) data))
    return;

  if ((fddata = evas_object_smart_data_get (obj)))
    {
      if (!strcmp (emission, "e,fd,button,global,cancel"))
	{
	  if (fddata->func)
	    {
	      fddata->func (fddata->fdata, obj, ESMART_FILE_DIALOG_CANCEL);
	    }
	}
      else if (!strcmp (emission, "e,fd,button,global,ok"))
	{
	  if (fddata->func)
	    {
	      fddata->func (fddata->fdata, obj, ESMART_FILE_DIALOG_OK);
	    }
	}
      else
	{
	  fprintf (stderr, "%s\n", emission);
	}
    }
}
static void
_esmart_file_dialog_file_button_cb (void *data, Evas_Object * o,
				    const char *emission, const char *source)
{
  Esmart_File_Dialog *fddata;
  Evas_Object *obj = NULL;

  if (!(obj = (Evas_Object *) data))
    return;

  if ((fddata = evas_object_smart_data_get (obj)))
    {
      if (!strcmp (emission, "e,fd,button,file,new"))
	{
	  if (fddata->func)
	    {
	      fddata->func (fddata->fdata, obj, ESMART_FILE_DIALOG_NEW);
	    }
	}
      else if (!strcmp (emission, "e,fd,button,file,rename"))
	{
	  if (fddata->func)
	    {
	      fddata->func (fddata->fdata, obj, ESMART_FILE_DIALOG_RENAME);
	    }
	}
      else if (!strcmp (emission, "e,fd,button,file,delete"))
	{
	  if (fddata->func)
	    {
	      fddata->func (fddata->fdata, obj, ESMART_FILE_DIALOG_DELETE);
	    }
	}
      else
	{
	  fprintf (stderr, "%s\n", emission);
	}
    }
}

/*==========================================================================
 * Smart Object Code, Go Away
 *========================================================================*/


/*** external API ***/

static Evas_Object *
esmart_file_dialog_object_new (Evas * evas)
{
  Evas_Object *esmart_file_dialog_object;

  esmart_file_dialog_object =
    evas_object_smart_add (evas, _esmart_file_dialog_object_smart_get ());

  return esmart_file_dialog_object;
}

/*** smart object handler functions ***/

static Evas_Smart *
_esmart_file_dialog_object_smart_get (void)
{
  static Evas_Smart *smart = NULL;
  if (smart)
    return (smart);

  smart = evas_smart_new ("esmart_file_dialog_object",
			  _esmart_file_dialog_object_add,
			  _esmart_file_dialog_object_del,
			  _esmart_file_dialog_object_layer_set,
			  _esmart_file_dialog_object_raise,
			  _esmart_file_dialog_object_lower,
			  _esmart_file_dialog_object_stack_above,
			  _esmart_file_dialog_object_stack_below,
			  _esmart_file_dialog_object_move,
			  _esmart_file_dialog_object_resize,
			  _esmart_file_dialog_object_show,
			  _esmart_file_dialog_object_hide,
			  _esmart_file_dialog_object_color_set,
			  _esmart_file_dialog_object_clip_set,
			  _esmart_file_dialog_object_clip_unset, NULL);

  return smart;
}

void
_esmart_file_dialog_object_add (Evas_Object * o)
{
  Esmart_File_Dialog *data = NULL;

  data = malloc (sizeof (Esmart_File_Dialog));
  memset (data, 0, sizeof (Esmart_File_Dialog));

  data->clip = evas_object_rectangle_add (evas_object_evas_get (o));
  evas_object_color_set (data->clip, 255, 255, 255, 255);

  evas_object_smart_data_set (o, data);
}


void
_esmart_file_dialog_object_del (Evas_Object * o)
{
  Esmart_File_Dialog *data;

  if ((data = evas_object_smart_data_get (o)))
    {
      free (data);
    }
}

void
_esmart_file_dialog_object_layer_set (Evas_Object * o, int l)
{
  Esmart_File_Dialog *data;

  if ((data = evas_object_smart_data_get (o)))
    {
      evas_object_layer_set (data->clip, l);
    }
}

void
_esmart_file_dialog_object_raise (Evas_Object * o)
{
  Esmart_File_Dialog *data;

  data = evas_object_smart_data_get (o);

  if ((data = evas_object_smart_data_get (o)))
    {
      evas_object_raise (data->clip);
    }
}

void
_esmart_file_dialog_object_lower (Evas_Object * o)
{
  Esmart_File_Dialog *data;

  if ((data = evas_object_smart_data_get (o)))
    {
      evas_object_lower (data->clip);
    }
}

void
_esmart_file_dialog_object_stack_above (Evas_Object * o, Evas_Object * above)
{
  Esmart_File_Dialog *data;

  data = evas_object_smart_data_get (o);

  if ((data = evas_object_smart_data_get (o)))
    {
      evas_object_stack_above (data->clip, above);
    }
}

void
_esmart_file_dialog_object_stack_below (Evas_Object * o, Evas_Object * below)
{
  Esmart_File_Dialog *data;

  if ((data = evas_object_smart_data_get (o)))
    {
      evas_object_stack_below (data->clip, below);
    }
}

void
_esmart_file_dialog_object_move (Evas_Object * o, Evas_Coord x, Evas_Coord y)
{
  Esmart_File_Dialog *data;

  data = evas_object_smart_data_get (o);

  if ((data = evas_object_smart_data_get (o)))
    {
      evas_object_move (data->clip, x, y);
      evas_object_move (data->edje, x, y);
    }
}

void
_esmart_file_dialog_object_resize (Evas_Object * o, Evas_Coord w,
				   Evas_Coord h)
{
  Esmart_File_Dialog *data;

  data = evas_object_smart_data_get (o);

  if ((data = evas_object_smart_data_get (o)))
    {
      evas_object_resize (data->clip, w, h);
      evas_object_resize (data->edje, w, h);
    }
}

static void
__file_dialog_entity_free_cb (void *data, Evas * e, Evas_Object * o, void *ev)
{
  char *str = NULL;
  if (o)
    {
      if ((str = evas_object_data_del (o, "name")))
	{
#if 0
	  fprintf (stderr, "Freeing: %s\n", str);
	  free (str);
#endif
	}
#if 0
      else
	{
	  fprintf (stderr, "Danger: You've got leakage %p, or do you?\n", o);
	}
#endif
    }
}

static Evas_Object *
__esmart_file_dialog_file_object_get (Evas_Object * o, char *file)
{
  char *key = NULL;
  char buf[PATH_MAX];
  Evas_Coord minw, minh;
  const char *edjefile = NULL;
  Evas_Object *result = NULL;
  Esmart_File_Dialog *data;

  if (!file)
    return (NULL);
  if (strcmp (file, "."))
    {
      if (strcmp (file, ".."))
	{
	  if (file[0] == '.')
	    return (NULL);
	}
    }
  if ((data = evas_object_smart_data_get (o)))
    {
      result = edje_object_add (evas_object_evas_get (o));
      edje_object_file_get (data->edje, &edjefile, NULL);
      if (edje_object_file_set (result, edjefile, "esmart.filedialog.file"))
	{
	  if (edje_object_part_exists (result, "file.name"))
	    {
	      edje_object_size_min_get (result, &minw, &minh);
	      evas_object_resize (result, minw, minh);
	      edje_object_part_text_set (result, "file.name", file);
	      snprintf (buf, PATH_MAX, "%s", file);
	      key = strdup (buf);
	      evas_object_data_set (result, "name", key);
	      evas_object_event_callback_add (result, EVAS_CALLBACK_FREE,
					      __file_dialog_entity_free_cb,
					      NULL);
	      edje_object_signal_callback_add (result, "e,fd,file,*", "*",
					       _esmart_file_dialog_file_cb,
					       o);
	      evas_object_show (result);
	    }
	  else
	    {
	      fprintf (stderr, "Part existance check failed");
	      evas_object_del (result);
	      result = NULL;
	    }
	}
      else
	{
	  fprintf (stderr, "Edje File set failed");
	  evas_object_del (result);
	  result = NULL;
	}
    }
  return (result);
}
static Evas_Object *
__esmart_file_dialog_directory_object_get (Evas_Object * o, char *file)
{
  char *key = NULL;
  char buf[PATH_MAX];
  Evas_Coord minw, minh;
  const char *edjefile = NULL;
  Evas_Object *result = NULL;
  Esmart_File_Dialog *data;

  if (!file)
    return (NULL);
  if (!strcmp (file, "."))
    {
      return (NULL);
    }
  else if (strcmp (file, ".."))
    {
      if (file[0] == '.')
	return (NULL);
    }
  if ((data = evas_object_smart_data_get (o)))
    {
      result = edje_object_add (evas_object_evas_get (o));
      edje_object_file_get (data->edje, &edjefile, NULL);
      if (edje_object_file_set
	  (result, edjefile, "esmart.filedialog.directory"))
	{
	  if (edje_object_part_exists (result, "directory.name"))
	    {
	      edje_object_size_min_get (result, &minw, &minh);
	      evas_object_resize (result, minw, minh);
	      edje_object_part_text_set (result, "directory.name", file);
	      snprintf (buf, PATH_MAX, "%s", file);
	      key = strdup (buf);
	      evas_object_data_set (result, "name", key);
	      evas_object_event_callback_add (result, EVAS_CALLBACK_FREE,
					      __file_dialog_entity_free_cb,
					      NULL);
	      edje_object_signal_callback_add (result, "e,fd,directory,*",
					       "*",
					       _esmart_file_dialog_directory_cb,
					       o);
	      evas_object_show (result);
	    }
	  else
	    {
	      fprintf (stderr, "Part existance check failed\n");
	      evas_object_del (result);
	      result = NULL;
	    }
	}
      else
	{
	  evas_object_del (result);
	  result = NULL;
	}
    }
  return (result);
}

void
_esmart_file_dialog_object_show (Evas_Object * o)
{
  DIR *dir = NULL;
  char buf[PATH_MAX];
  struct stat status;
  struct dirent *di = NULL;
  Evas_Object *obj = NULL;
  Esmart_File_Dialog *data;
  if ((data = evas_object_smart_data_get (o)))
    {
      if ((dir = opendir (data->path)))
	{
	  while ((di = readdir (dir)))
	    {
	      snprintf (buf, PATH_MAX, "%s/%s", data->path, di->d_name);
	      if (!stat (buf, &status))
		{
		  if (S_ISDIR (status.st_mode))
		    {
		      if (data->directories)
			{
			  if ((obj =
			       __esmart_file_dialog_directory_object_get (o,
									  di->
									  d_name)))
			    {
			      esmart_container_element_append (data->
							       directories,
							       obj);
			    }
			}
		    }
		  else
		    {
		      if (data->files)
			{
			  if ((obj = __esmart_file_dialog_file_object_get (o,
									   di->
									   d_name)))
			    {
			      esmart_container_element_append (data->files,
							       obj);
			    }
			}
		    }
		}
	    }
	  closedir (dir);
	  esmart_container_sort (data->files, sort_cb);
	  esmart_container_sort (data->directories, sort_cb);
	  data->func (data->fdata, o, ESMART_FILE_DIALOG_DIR_CHANGED);
	}
      else
	{
	  fprintf (stderr, "Unable to open %s\n", data->path);
	}
      evas_object_show (data->clip);
    }
}

void
_esmart_file_dialog_object_hide (Evas_Object * o)
{
  Esmart_File_Dialog *data;

  if ((data = evas_object_smart_data_get (o)))
    {
      evas_object_hide (data->clip);

      esmart_container_empty (data->directories);
      esmart_container_empty (data->files);
    }
}

void
_esmart_file_dialog_object_color_set (Evas_Object * o, int r, int g, int b,
				      int a)
{
  Esmart_File_Dialog *data;

  if ((data = evas_object_smart_data_get (o)))
    {
      evas_object_color_set (data->clip, r, g, b, a);
    }
}

void
_esmart_file_dialog_object_clip_set (Evas_Object * o, Evas_Object * clip)
{
  Esmart_File_Dialog *data;

  if ((data = evas_object_smart_data_get (o)))
    {
      evas_object_clip_set (data->clip, clip);
    }
}

void
_esmart_file_dialog_object_clip_unset (Evas_Object * o)
{
  Esmart_File_Dialog *data;

  if ((data = evas_object_smart_data_get (o)))
    {
      evas_object_clip_unset (data->clip);
    }
}
