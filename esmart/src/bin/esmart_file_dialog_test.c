/**
 * esmart_file_dialog_main.c
 * A file dialog available to evas programmers in the form of a smart
 * object.  
 *
 * $ esmart_file_dialog_test 
 * to see it in action w/ default theme
 * $ esmart_file_dialog_test /path/to/my_custom/theme.eet 
 * to test a custom theme you've written for it
 */
#include <Ecore.h>
#include <Edje.h>
#include <Ecore_Evas.h>
#include <Esmart_File_Dialog.h>

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

/**
 * the callback signal the file dialog calls when events happen
 * @param data a pointer to the ecore_evas window
 * @param efd the esmart_file_dialog smart object
 * @param type the event type that the file dialog dispatched
 */
void
file_dialog_cb (void *data, Evas_Object * efd, Esmart_File_Dialog_Op type)
{
  Eina_List *l = NULL;
  Ecore_Evas *ee = (Ecore_Evas *) data;

  switch (type)
    {
    case ESMART_FILE_DIALOG_CANCEL:
      fprintf (stderr, "Cancel Clicked\n");
      ecore_main_loop_quit ();
      break;
    case ESMART_FILE_DIALOG_OK:
      for (l = esmart_file_dialog_selections_get (efd); l; l = l->next)
	{
	  fprintf (stderr, "%s\n", (char *) l->data);
	}
      fprintf (stderr, "OK Clicked\n");
      break;
    case ESMART_FILE_DIALOG_DELETE:
      /* FIXME: selectionslist should be one or more directories to delete */
      fprintf (stderr, "Delete Clicked\n");
      break;
	case ESMART_FILE_DIALOG_NEW:
      /* FIXME I guess this should be DIR_NEW, selections list should be of
       * length 1, the name of the new directory to make
       */
      fprintf (stderr, "New Clicked\n");
      break;
    case ESMART_FILE_DIALOG_RENAME:
      /* FIXME selection list should be of length two, the first is the
       * current file we'd like to rename to the second
       */
      fprintf (stderr, "Rename Clicked\n");
      break;
    case ESMART_FILE_DIALOG_DIR_CHANGED:
      fprintf (stderr, "Directory Changed %s\n",
	       esmart_file_dialog_current_directory_get (efd));
      ecore_evas_title_set (ee,
			    esmart_file_dialog_current_directory_get (efd));
      break;
    default:
      fprintf (stderr, "Unknown file dialog type, %d\n", type);
      break;
    }
}

/**
 * exit_cb - called when the app exits(window is killed)
 * @ev_type -
 * @ev - 
 * @data -
 */
static int
exit_cb (void *data, int ev_type, void *ev)
{
  ecore_main_loop_quit ();
  return (0);
}

/**
 * window_resize_cb - when the ecore_evas is resized by the user
 * @ee - the Ecore_Evas that was resized 
 */
static void
window_resize_cb (Ecore_Evas * ee)
{
  int w, h;
  Evas_Object *o = NULL;

  ecore_evas_geometry_get (ee, NULL, NULL, &w, &h);
  if ((o = evas_object_name_find (ecore_evas_get (ee), "bg")))
    {
      evas_object_resize (o, (double) w, (double) h);
    }
  if ((o = evas_object_name_find (ecore_evas_get (ee), "fd")))
    {
      evas_object_resize (o, (double) w, (double) h);
    }
}

/**
 * window_del_cb - callback for when the ecore_evas is deleted
 * @ee - the Ecore_Evas that was deleted
 */
static void
window_del_cb (Ecore_Evas * ee)
{
  ecore_main_loop_quit ();
}

/**
 * main - your C apps start here, duh.
 * @argc - unused
 * @argv - unused
 */
int
main (int argc, const char *argv[])
{
  Evas *evas = NULL;
  char buf[PATH_MAX];
  Ecore_Evas *e = NULL;
  Evas_Object *o = NULL;
  Evas_Object *bg = NULL;
  Evas_Object *efd = NULL;
  Evas_Coord w, h;

  ecore_init ();
  ecore_app_args_set (argc, argv);

  ecore_event_handler_add (ECORE_EVENT_SIGNAL_EXIT, exit_cb, NULL);

  if (ecore_evas_init ())
    {
      edje_init ();
      edje_frametime_set (1.0 / 25.0);

      e = ecore_evas_software_x11_new (NULL, 0, 0, 0, 300, 300);
      ecore_evas_title_set (e, "Esmart File Dialog Test");
      ecore_evas_callback_delete_request_set (e, window_del_cb);
      ecore_evas_callback_resize_set (e, window_resize_cb);

      evas = ecore_evas_get (e);
      bg = evas_object_rectangle_add (evas);
      evas_object_move (bg, 0, 0);
      evas_object_resize (bg, 300, 300);
      evas_object_color_set (bg, 89, 94, 97, 255);
      evas_object_layer_set (bg, 0);
      evas_object_name_set (bg, "bg");
      evas_object_show (bg);

      if (argv[1])
	snprintf (buf, PATH_MAX, "%s", argv[1]);
      else
	snprintf (buf, PATH_MAX, "%s", PACKAGE_DATA_DIR "/fd.eet");
      if ((efd = esmart_file_dialog_new (evas, buf)))
	{
	  evas_object_move (efd, 0, 0);
	  esmart_file_dialog_callback_add (efd, file_dialog_cb, e);
	  evas_object_resize (efd, 300, 300);
	  evas_object_color_set (efd, 255, 255, 255, 128);
	  evas_object_layer_set (efd, 1);
	  evas_object_name_set (efd, "fd");

	  o = esmart_file_dialog_edje_get (efd);
	  edje_object_size_min_get (o, &w, &h);
	  if ((w > 0) && (h > 0))
	    {
	      ecore_evas_size_min_set (e, (int) w, (int) h);
	      evas_object_resize (efd, w, h);
	    }

	  edje_object_size_max_get (o, &w, &h);

	  if ((w > 0) && (h > 0))
	    {
	      if (w > INT_MAX)
		w = INT_MAX;
	      if (h > INT_MAX)
		h = INT_MAX;
	      ecore_evas_size_max_set (e, (int) w, (int) h);
	    }
	  evas_object_show (efd);
	  ecore_evas_show (e);
	  ecore_main_loop_begin ();
	}
    }
  edje_shutdown ();
  ecore_evas_shutdown ();
  ecore_shutdown ();

  return (0);
}
