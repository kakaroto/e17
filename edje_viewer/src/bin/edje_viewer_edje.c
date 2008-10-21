/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */

#include <edje_viewer_main.h>

#if HAVE___ATTRIBUTE__
#define __UNUSED__ __attribute__((unused))
#else
#define __UNUSED__
#endif

#define DAT PACKAGE_DATA_DIR"/"

#define FREE(ptr) do { if(ptr) { free(ptr); ptr = NULL; }} while (0);

static void signal_cb(void *data, Evas_Object *o, const char *sig,
	const char *src);
static void message_cb(void *data, Evas_Object *obj, Edje_Message_Type type,
	int id, void *msg);

Demo_Edje *edje_part_create(Etk_Tree *output, Etk_Mdi_Area *mdi_area, 
      const char *file, char *name)
{
   Etk_Widget *mdi_window;
   Evas_Object *o;
   Demo_Edje *de;
   Evas *evas;
   Evas_Coord w, h;

   de = calloc(1, sizeof(Demo_Edje));
   de->name = strdup(name);

   de->mdi_window = mdi_window = etk_mdi_window_new();
   etk_mdi_area_put(mdi_area, mdi_window, 20, 30);
   etk_mdi_window_title_set(ETK_MDI_WINDOW(mdi_window), name);

   evas = etk_widget_toplevel_evas_get(mdi_window);
   if (!evas) return NULL;

   o = edje_object_add(evas);
   edje_object_message_handler_set(o, message_cb, output);
   edje_object_signal_callback_add(o, "*", "*", signal_cb, output);
   edje_object_file_set(o, file, name);
   edje_object_part_drag_size_set(o, "dragable", 0.01, 0.5);
   edje_object_part_drag_step_set(o, "dragable", 0.1, 0.1);
   edje_object_part_drag_page_set(o, "dragable", 0.2, 0.2);
   edje_object_size_min_calc(o, &w, &h);

   de->edje_object = o;

   o = edje_object_add(evas);
   edje_object_file_set(o, ThemeFile, "checkers");
   edje_object_part_swallow(o, "swallow", de->edje_object);

   de->etk_evas = etk_evas_object_new_from_object(o);
   etk_bin_child_set(ETK_BIN(mdi_window), de->etk_evas);
   if (w && h)
     evas_object_resize(o, w, h);
   else
     etk_widget_size_request_set(mdi_window, 200, 200);

   return de;
}

static void signal_cb(void *data, Evas_Object *o, const char *sig, const char *src)
{
   Etk_Tree *output;
   Etk_Tree_Col *col;
   Etk_Tree_Row *row;
   int count;
   char *str;

   if (!(output = data)) return;
   
   col = etk_tree_nth_col_get(output, 0);
   count = output->total_rows;
   if (count > 5000)
     {
	row = etk_tree_first_row_get(output);
	etk_tree_row_delete(row);
     }
   
   str = calloc(1024, sizeof(char));
   snprintf(str, 1024, "CALLBACK for \"%s\" \"%s\"", sig, src);


   row = etk_tree_row_append(output, NULL, col, str, NULL);
   etk_tree_row_scroll_to(row, ETK_TRUE);
   etk_tree_row_select(row);
   if (!strcmp(sig, "drag"))
     {
	double x, y;

	count = output->total_rows;
	if (count > 5000)
	  {
	     row = etk_tree_first_row_get(output);
	     etk_tree_row_delete(row);
	  }

	edje_object_part_drag_value_get(o, src, &x, &y);
	snprintf(str, 1024, "Drag %3.3f %3.3f", x, y);
	etk_tree_row_append(output, NULL, col, str, NULL);
	etk_tree_row_scroll_to(row, ETK_TRUE);
	etk_tree_row_select(row);
     }

   FREE(str);
}


static void message_cb(void *data, Evas_Object *obj, Edje_Message_Type type,
      int id, void *msg)
{
   Etk_Tree *output;
   Etk_Tree_Col *col;
   Etk_Tree_Row *row;
   int count;
   char *str;

   if (!(output = data)) return;
   
   col = etk_tree_nth_col_get(output, 0);
   count = output->total_rows;
   if (count > 5000)
     {
	row = etk_tree_first_row_get(output);
	etk_tree_row_delete(row);
     }
   
   str = calloc(1024, sizeof(char));
   snprintf(str, 1024, "MESSAGE for %p from script type %i id %i", obj, type, id);

   row = etk_tree_row_append(output, NULL, col, str, NULL);
   etk_tree_row_scroll_to(row, ETK_TRUE);
   etk_tree_row_select(row);
   if (type == EDJE_MESSAGE_STRING)
     {
	Edje_Message_String *emsg;

	count = output->total_rows;
	if (count > 5000)
	  {
	     row = etk_tree_first_row_get(output);
	     etk_tree_row_delete(row);
	  }

	emsg = (Edje_Message_String *)msg;
	snprintf(str, 1024, "STWING: \"%s\"\n", emsg->str);
	row = etk_tree_row_append(output, NULL, col, str, NULL);
	etk_tree_row_scroll_to(row, ETK_TRUE);
	etk_tree_row_select(row);
     }
   edje_object_message_send(obj, EDJE_MESSAGE_NONE, 12345, NULL);

   FREE(str);
}
