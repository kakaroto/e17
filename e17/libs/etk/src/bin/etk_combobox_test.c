#define _GNU_SOURCE
#include "etk_test.h"
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <Ecore.h>
#include <Ecore_File.h>

#define STARTING_STOCK_ID ETK_STOCK_DIALOG_APPLY
#define ENDING_STOCK_ID ETK_STOCK_FOLDER_NEW

static void _active_item_changed_cb(Etk_Object *object, void *data);
static void _etk_combobox_entry_populate(Etk_Combobox_Entry *combobox_entry, char *dir);
static void _etk_combobox_entry_active_item_changed_cb(Etk_Object *object, void *data);
static void _etk_combobox_entry_text_changed_cb(Etk_Object *object, void *data);
static char **str_split(char **str, char *delim);
  
static char *cur_dir = NULL;
static Etk_Widget *win = NULL;
   static char *_keywords[] = {
        "Enlightenment Project",
	"Etk HOWTO",
	"git introduction",
	"linux kernel howto",
	"install on freebsd",
	"FreeBSD handbook",
	"Handy BASH tips",
	"bash scripting faq",
	"the matrix movie",
	NULL
   };

/**************************
 *
 * Creation of the test-app window
 *
 **************************/
 
/* Creates the window for the combobox test */
void etk_test_combobox_window_create(void *data)
{
   char dir[PATH_MAX];
   Etk_Widget *vbox;
   Etk_Widget *combobox;
   Etk_Widget *image;
   Etk_Widget *checkbox;
   Etk_Widget *frame;
   Etk_Combobox_Item *item;
   int *stock_id;
   int i;

   if (win)
   {
      etk_widget_show_all(ETK_WIDGET(win));
      return;
   }
   
   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Etk Combobox Test");
   etk_container_border_width_set(ETK_CONTAINER(win), 5);
   etk_signal_connect("delete-event", ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);
   
   vbox = etk_vbox_new(ETK_FALSE, 3);
   etk_container_add(ETK_CONTAINER(win), vbox);
   

   /*******************
    * Simple combobox
    *******************/
   frame = etk_frame_new("Simple combobox");
   etk_box_append(ETK_BOX(vbox), frame, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   /* Create a simple combobox containing one column of labels and add 3 items to it */
   combobox = etk_combobox_new_default();
   etk_container_add(ETK_CONTAINER(frame), combobox);
   etk_combobox_item_append(ETK_COMBOBOX(combobox), "Item 1");
   etk_combobox_item_append(ETK_COMBOBOX(combobox), "Item 2");
   etk_combobox_item_append(ETK_COMBOBOX(combobox), "Item 3");   
   
   /*******************
    * Entry combobox with dir listing / navigation
    *******************/
   frame = etk_frame_new("Entry combobox");
   etk_box_append(ETK_BOX(vbox), frame, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   /* Create a simple combobox containing one column of labels and add 3 items to it */
   combobox = etk_combobox_entry_new();
   etk_signal_connect("active-item-changed", ETK_OBJECT(combobox), ETK_CALLBACK(_etk_combobox_entry_active_item_changed_cb), NULL);
   etk_combobox_entry_column_add(ETK_COMBOBOX_ENTRY(combobox), ETK_COMBOBOX_ENTRY_IMAGE, 24, ETK_COMBOBOX_ENTRY_NONE, 0.0);
   etk_combobox_entry_column_add(ETK_COMBOBOX_ENTRY(combobox), ETK_COMBOBOX_ENTRY_LABEL, 75, ETK_COMBOBOX_ENTRY_EXPAND, 0.0);
   etk_combobox_entry_build(ETK_COMBOBOX_ENTRY(combobox));
   etk_container_add(ETK_CONTAINER(frame), combobox);   
   getcwd(dir, sizeof(dir));   
   _etk_combobox_entry_populate(ETK_COMBOBOX_ENTRY(combobox), dir);
   
   /*******************
    * Entry combobox with active searching
    *******************/
   frame = etk_frame_new("Entry combobox (search)");
   etk_box_append(ETK_BOX(vbox), frame, ETK_BOX_START, ETK_BOX_NONE, 0);
 
   combobox = etk_combobox_entry_new_default();
      
   for (i = 0; _keywords[i]; i++)   
     etk_combobox_entry_item_append(ETK_COMBOBOX_ENTRY(combobox), _keywords[i], NULL);

   etk_signal_connect("text-changed", ETK_OBJECT(etk_combobox_entry_entry_get(ETK_COMBOBOX_ENTRY(combobox))), ETK_CALLBACK(_etk_combobox_entry_text_changed_cb), combobox);
   etk_container_add(ETK_CONTAINER(frame), combobox);   
   
   /*******************
    * Disabled combobox
    *******************/
   frame = etk_frame_new("Disabled combobox");
   etk_box_append(ETK_BOX(vbox), frame, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   /* Create a simple combobox containing one column of labels and add 3 items to it */
   combobox = etk_combobox_new_default();
   etk_container_add(ETK_CONTAINER(frame), combobox);
   etk_combobox_item_append(ETK_COMBOBOX(combobox), "Disabled text");
   etk_combobox_item_append(ETK_COMBOBOX(combobox), "Item 2");
   etk_combobox_item_append(ETK_COMBOBOX(combobox), "Item 3");
   etk_widget_disabled_set(combobox, ETK_TRUE);
   
   
   /*******************
    * Complex combobox
    *******************/
   frame = etk_frame_new("Some stock icons");
   etk_box_append(ETK_BOX(vbox), frame, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   vbox = etk_vbox_new(ETK_FALSE, 3);
   etk_container_add(ETK_CONTAINER(frame), vbox);
   
   image = etk_image_new_from_stock(STARTING_STOCK_ID, ETK_STOCK_BIG);
   etk_box_append(ETK_BOX(vbox), image, ETK_BOX_START, ETK_BOX_NONE, 0);
   
   /* Create a more complex combobox with three columns (an image, a label and a checkbox) */
   combobox = etk_combobox_new();
   etk_combobox_column_add(ETK_COMBOBOX(combobox), ETK_COMBOBOX_IMAGE, 24, ETK_COMBOBOX_NONE, 0.0);
   etk_combobox_column_add(ETK_COMBOBOX(combobox), ETK_COMBOBOX_LABEL, 75, ETK_COMBOBOX_EXPAND, 0.0);
   etk_combobox_column_add(ETK_COMBOBOX(combobox), ETK_COMBOBOX_OTHER, 24, ETK_COMBOBOX_NONE, 1.0);
   etk_combobox_build(ETK_COMBOBOX(combobox));
   etk_box_append(ETK_BOX(vbox), combobox, ETK_BOX_START, ETK_BOX_NONE, 0);
   etk_signal_connect("active-item-changed", ETK_OBJECT(combobox), ETK_CALLBACK(_active_item_changed_cb), image);
   
   /* We fill the combobox with some stock-ids */
   for (i = STARTING_STOCK_ID; i <= ENDING_STOCK_ID; i++)
   {
      image = etk_image_new_from_stock(i, ETK_STOCK_SMALL);
      checkbox = etk_check_button_new();
      item = etk_combobox_item_append(ETK_COMBOBOX(combobox), image, etk_stock_label_get(i), checkbox);
      
      stock_id = malloc(sizeof(int));
      *stock_id = i;
      etk_combobox_item_data_set_full(item, stock_id, free);
   }

   etk_widget_show_all(win);
}

/**************************
 *
 * Callbacks
 *
 **************************/

/* Called when the active item of the combobox has been changed */
static void _active_item_changed_cb(Etk_Object *object, void *data)
{
   Etk_Combobox *combobox;
   Etk_Image *image;
   Etk_Combobox_Item *active_item;
   int *stock_id;
   
   if (!(combobox = ETK_COMBOBOX(object)) || !(image = ETK_IMAGE(data)))
      return;
   if (!(active_item = etk_combobox_active_item_get(combobox)))
      return;
   if (!(stock_id = etk_combobox_item_data_get(active_item)))
      return;
   
   etk_image_set_from_stock(image, *stock_id, ETK_STOCK_BIG);
}

static void _etk_combobox_entry_populate(Etk_Combobox_Entry *combobox_entry, char *dir)
{
   Ecore_List *files;
   char *file;
   char *parent;
   char dir_stripped[PATH_MAX];
   
   if (!dir || !ecore_file_is_dir(dir))
      return;
   
   snprintf(dir_stripped, sizeof(dir_stripped), "%s", dir);
   
   /* remove any trailing slashes */
   while (dir_stripped[strlen(dir_stripped) - 1] == '/')
     dir_stripped[strlen(dir_stripped) - 1] = '\0';
   
   if (cur_dir) free(cur_dir);
   cur_dir = strdup(dir_stripped);
   
   files = ecore_file_ls(dir_stripped);
   ecore_list_sort(files, (Ecore_Compare_Cb)strcasecmp, 'a');
   ecore_list_goto_first(files);
   etk_combobox_entry_clear(combobox_entry);
   etk_entry_text_set(ETK_ENTRY(etk_combobox_entry_entry_get(combobox_entry)), 
		      dir_stripped);

   /* add navigation link to parent level dir if possible */
   parent = ecore_file_get_dir(dir_stripped);
   if (ecore_file_is_dir(parent))
     etk_combobox_entry_item_prepend(combobox_entry,
				     etk_image_new_from_stock(ETK_STOCK_PLACES_FOLDER,
							      ETK_STOCK_SMALL),
				     "..", NULL);
   free(parent);
   
   for (file = ecore_list_next(files); file; file = ecore_list_next(files))
     {
	char path[PATH_MAX];
       
	snprintf(path, sizeof(path), "%s/%s", dir_stripped, file);
	if (ecore_file_is_dir(path))
	  {
	     etk_combobox_entry_item_prepend(combobox_entry,
					    etk_image_new_from_stock(ETK_STOCK_PLACES_FOLDER, 
								     ETK_STOCK_SMALL),
					    file, NULL);
	  }
	else
	  {
	     etk_combobox_entry_item_append(combobox_entry,
					    etk_image_new_from_stock(ETK_STOCK_TEXT_X_GENERIC, 
								     ETK_STOCK_SMALL),
					    file, NULL);	     
	  }
     }
}

/* Called when the active item of the combobox_entry is changed */
static void _etk_combobox_entry_active_item_changed_cb(Etk_Object *object, void *data)
{
   Etk_Combobox_Entry *combobox;
   Etk_Combobox_Entry_Item *active_item = NULL;
   char *file;
   Etk_Widget *img;
   char path[PATH_MAX];
    
   if (!(combobox = ETK_COMBOBOX_ENTRY(object)) || !(active_item = etk_combobox_entry_active_item_get(combobox)))
      return;
   
   etk_combobox_entry_item_fields_get(active_item, &img, &file, NULL);   
 
   if (!strcmp(file, ".."))
     {
	char *parent;
	parent = ecore_file_get_dir(cur_dir);
	snprintf(path, sizeof(path), "%s", parent);
	free(parent);
     }
   else
     snprintf(path, sizeof(path), "%s/%s", cur_dir, file);
   
   if (ecore_file_is_dir(path))
     {
	_etk_combobox_entry_populate(combobox, path);
	return;
     }
   
   if (ecore_file_is_dir(file))
      _etk_combobox_entry_populate(combobox, file);
}

static void _etk_combobox_entry_text_changed_cb(Etk_Object *object, void *data)
{
   Etk_Combobox_Entry *combobox;
   Etk_Entry *entry;
   char *search_str = NULL;
   const char *entry_text = NULL;
   char **words = NULL;
   int i;

   if (!(combobox = ETK_COMBOBOX_ENTRY(data)) || !(entry = ETK_ENTRY(object)))
      return;
 
   if (!etk_combobox_entry_is_popped_up(combobox))
     {
	etk_combobox_entry_pop_up(combobox);
	etk_popup_window_focused_window_set(ETK_POPUP_WINDOW(win));
     }
   
   entry_text = etk_entry_text_get(entry);
   if (!entry_text)
     return;
   
   search_str = strdup(entry_text);   
   words = str_split(&search_str, " ");
   
   etk_combobox_entry_clear(combobox);
   etk_combobox_entry_pop_down(combobox);
   for (i = 0; _keywords[i]; i++)
     {
	int j;
	
	for (j = 0; words[j]; j++)
	  {
	     if (!strcasestr(_keywords[i], words[j]))
	       goto brk;
	  }
	etk_combobox_entry_item_append(combobox, _keywords[i], NULL);
brk:
	continue;
     }
   etk_combobox_entry_pop_up(combobox);
   etk_popup_window_focused_window_set(ETK_POPUP_WINDOW(win));
   if (words)
      free(words);
}

static char **str_split(char **str, char *delim)
{
   char **tok;
   int  i = 0;
   char *t;
   
   tok = calloc(2048, sizeof(char*));
   tok[i] = strtok(*str, delim);
   i++;
   while((t = strtok(NULL, delim)))
     {
	tok[i] = t;
	i++;
     }
   tok[i] = NULL;
   
   return tok;
}
