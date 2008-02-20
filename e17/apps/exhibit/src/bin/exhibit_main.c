/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include "exhibit.h"

/* defines the max number of additions to the tree per timer tick */
#define MAX_INSERTS_PER_ITERATION 170

/* defines the timer tick interval for tree inserts */
#define INSERTS_INTERVAL 0.3

/* defines the initial size and increment size that file list arrays have */
#define FILELIST_SIZE 3000

/* defines the number of bytes used for evas's image cache */
#define IMAGE_CACHE_BYTES 16777216

/* defines the number of objects used in evas's image cache */
#define IMAGE_CACHE_NUM 128

typedef struct _Ex_Populate_Data Ex_Populate_Data;

struct _Ex_Populate_Data
{
   int num;
   char **entries;
   char *selected_file;
   Ex_Tree_Update update;
};

Exhibit *e;
Evas_List  *event_handlers;

static void _ex_main_monitor_dir(void *data, Ecore_File_Monitor *ecore_file_monitor, Ecore_File_Event event, const char *path);
static int _ex_main_dtree_compare_cb(Etk_Tree_Col *col, Etk_Tree_Row *row1, Etk_Tree_Row *row2, void *data);
static Etk_Bool _ex_main_entry_dir_key_down_cb(Etk_Object *object, void *event, void *data);
static void _ex_main_combobox_entry_changed_cb(Etk_Object *object, void *data);
static Etk_Bool _ex_main_window_deleted_cb(void *data);
static void _ex_main_window_key_down_cb(Etk_Object *object, void *event, void *data);
static void _ex_main_window_resize_cb(Etk_Object *object, void *data);

Evas_List *etk_tree_selected_rows_get(Etk_Tree *tree)
{
   Evas_List *row_list = NULL;
   Etk_Tree_Row *iter;
   
   for (iter = etk_tree_first_row_get(tree); 
	iter; 
	iter = etk_tree_row_walk_next(iter, ETK_TRUE))
     if (iter->selected == ETK_TRUE) 
       row_list = evas_list_append(row_list, iter);
   
   return row_list;
}

int etk_tree_num_rows_get(Etk_Tree *tree)
{
   return tree->total_rows;
}

int etk_tree_row_num_get(Etk_Tree *tree, Etk_Tree_Row *row)
{
   int num = 0;
   Etk_Tree_Row *iter;
   
   for (iter = etk_tree_first_row_get(tree);
	iter;
	iter = etk_tree_row_walk_next(iter, ETK_TRUE))
     {
	if (iter == row)
	  return num;
	num++;	
     }
   return num;
}

void
_ex_main_statusbar_zoom_update(Exhibit *e)
{
   if (e->cur_tab->fit_window)
     {
        etk_statusbar_message_pop(ETK_STATUSBAR(e->statusbar[2]), 0);
        etk_statusbar_message_push(ETK_STATUSBAR(e->statusbar[2]), _("Fit to window"), 0);
     }
   else if(e->zoom > 0)
     {
	char zoom[6];
	etk_statusbar_message_pop(ETK_STATUSBAR(e->statusbar[2]), 0);
	snprintf(zoom, sizeof(zoom), "%d:1", abs(e->zoom));
	etk_statusbar_message_push(ETK_STATUSBAR(e->statusbar[2]), zoom, 0);
     }
   else if(e->zoom < 0)
     {
	char zoom[6];
	etk_statusbar_message_pop(ETK_STATUSBAR(e->statusbar[2]), 0);
	snprintf(zoom, sizeof(zoom), "1:%d", abs(e->zoom));
	etk_statusbar_message_push(ETK_STATUSBAR(e->statusbar[2]), zoom, 0);;
     }
   else
     {
	etk_statusbar_message_pop(ETK_STATUSBAR(e->statusbar[2]), 0);
	etk_statusbar_message_push(ETK_STATUSBAR(e->statusbar[2]), "1:1", 0);
     }      
}

void
_ex_main_image_unset(void)
{
   Ex_Tab *tab = e->cur_tab;
   
   etk_window_title_set(ETK_WINDOW(e->win), 
	 WINDOW_TITLE " - Image Viewing the Kewl Way!");

   E_FREE(tab->set_img_path);
   E_FREE(tab->cur_file);
   tab->image_loaded = ETK_FALSE;
   
   etk_statusbar_message_pop(ETK_STATUSBAR(e->statusbar[0]), 0);
   etk_statusbar_message_push(ETK_STATUSBAR(e->statusbar[0]), NULL, 0);
   etk_statusbar_message_pop(ETK_STATUSBAR(e->statusbar[1]), 0);
   etk_statusbar_message_push(ETK_STATUSBAR(e->statusbar[1]), NULL, 0);

   /* Just hide the image as if we deleted it */
   etk_widget_hide(ETK_WIDGET(tab->image));
}

void
_ex_main_image_set(Exhibit *e, char *image)
{
   Etk_Range *hs, *vs;
   char *title;
   int   w, h;
   int   bytes;
   char  size[30];
   unsigned int *data2; 
   
   title = calloc(strlen(image) + strlen(WINDOW_TITLE) + 5, sizeof(char));
   snprintf(title, strlen(image) + strlen(WINDOW_TITLE) + 5, "%s - %s", image, WINDOW_TITLE);
   etk_window_title_set(ETK_WINDOW(e->win), title);

   /* Save the path of where the image really is in case
    * we browse more in the dtree 
    */
   e->cur_tab->set_img_path = malloc(PATH_MAX);
   memset(e->cur_tab->set_img_path, 0, PATH_MAX);
   e->cur_tab->cur_file = malloc(PATH_MAX);
   memset(e->cur_tab->cur_file, 0, PATH_MAX);
   
   snprintf(e->cur_tab->set_img_path, PATH_MAX, "%s", e->cur_tab->dir);
   snprintf(e->cur_tab->cur_file, PATH_MAX, "%s", ecore_file_file_get(image));

   e->cur_tab->image_loaded = ETK_TRUE;
   etk_widget_show(ETK_WIDGET(e->cur_tab->image));
   
   if (_ex_file_is_ebg(image))
     {
	/* can we do this without the size request? it doesnt look good */
	etk_widget_size_request_set(ETK_WIDGET(e->cur_tab->image), 800, 600);	
	etk_image_set_from_edje(ETK_IMAGE(e->cur_tab->image), image, "e/desktop/background");
     }
   else
     {
	etk_image_set_from_file(ETK_IMAGE(e->cur_tab->image), image, NULL);

	if (e->cur_tab->fit_window)
	  _ex_tab_current_fit_to_window(e);
	else
	  _ex_tab_current_zoom_one_to_one(e);
     }
   
   bytes = ecore_file_size(image);
   snprintf(size, sizeof(size), "%d K", (int)(bytes/1024));
   etk_statusbar_message_pop(ETK_STATUSBAR(e->statusbar[0]), 0);
   etk_statusbar_message_push(ETK_STATUSBAR(e->statusbar[0]), size, 0);
      
   etk_image_size_get(ETK_IMAGE(e->cur_tab->image), &w, &h);   
   snprintf(size, sizeof(size), "( %d x %d )", w, h);
   etk_statusbar_message_pop(ETK_STATUSBAR(e->statusbar[1]), 0);
   etk_statusbar_message_push(ETK_STATUSBAR(e->statusbar[1]), size, 0);
   
   hs = etk_scrolled_view_hscrollbar_get(
					 ETK_SCROLLED_VIEW(e->cur_tab->scrolled_view));
   vs = etk_scrolled_view_vscrollbar_get(
					 ETK_SCROLLED_VIEW(e->cur_tab->scrolled_view));
      
   etk_range_value_set(hs, (double)w/2);
   etk_range_value_set(vs, (double)h/2);

   /* Reset undo data every time we change image */
   data2 = etk_object_data_get(ETK_OBJECT(e->cur_tab->image), "undo");
   if (data2) 
     E_FREE(data2);

   etk_object_data_set(ETK_OBJECT(e->cur_tab->image), "undo", NULL);

   if (e->options->comments_visible)
     _ex_comment_show(e);
}

static int
_ex_main_dtree_compare_cb(Etk_Tree_Col *col, Etk_Tree_Row *row1, Etk_Tree_Row *row2, void *data)
{
   char *dir1, *dir2;
   
   if (!row1 || !row2 || !col)
      return 0;
   
   etk_tree_row_fields_get(row1, col, NULL, NULL, &dir1, NULL);
   etk_tree_row_fields_get(row2, col, NULL, NULL, &dir2, NULL);
   return strcasecmp(dir1, dir2);
}

int
_ex_main_populate_files_timer_cb(void *fdata)
{
   Ex_Populate_Data *data;
   int i = 0;
   static int cur = 0;

   data = fdata;

   etk_tree_freeze(ETK_TREE(e->cur_tab->itree));
   etk_tree_freeze(ETK_TREE(e->cur_tab->dtree));   

   while (cur < data->num)
     {
	char image[PATH_MAX];
	char imagereal[PATH_MAX];
	struct stat st;
	char *file;

	file = data->entries[cur];

	++cur;
	++i;
        /* Do not include current dir/above dir */
	if ((!strcmp (file, ".")) || (!strcmp (file, "..")))
	  continue;

        /* Show hidden files and directories? */
	if ((!e->options->list_hidden) && (file[0] == '.'))
	  continue;
	
	snprintf(image, PATH_MAX, "%s%s", e->cur_tab->dir, file);

	if (data->update == EX_TREE_UPDATE_ALL || data->update == EX_TREE_UPDATE_DIRS)
	  {
	     if(stat(image, &st) == -1) continue;
	     if(S_ISDIR(st.st_mode))
	       {
		  etk_tree_row_append(ETK_TREE(e->cur_tab->dtree), NULL, e->cur_tab->dcol,
			etk_theme_icon_path_get(),
			"places/folder_16",
			file, NULL);
		  etk_combobox_entry_item_append(ETK_COMBOBOX_ENTRY(e->combobox_entry), file, NULL);
		  e->cur_tab->dirs = evas_list_append(e->cur_tab->dirs, file);
		  continue;
	       }
	  }

	/* If we don't want to do the rtree updating */
	if (data->update == EX_TREE_UPDATE_DIRS)
	  continue;

	if ((!e->options->show_all_filetypes) && (!_ex_file_is_viewable(file)))
	  continue;

	if(!realpath(image, imagereal))
	  snprintf(imagereal, PATH_MAX, "%s", image);
	
	_ex_main_itree_add(imagereal, data->selected_file);
	
	if (i == MAX_INSERTS_PER_ITERATION)
	  {
	     etk_tree_thaw(ETK_TREE(e->cur_tab->itree));
	     etk_tree_thaw(ETK_TREE(e->cur_tab->dtree));
	     
	     ecore_timer_add(INSERTS_INTERVAL, _ex_main_populate_files_timer_cb, data);
	     return 0;
	  }
     }

   if (data->update == EX_TREE_UPDATE_ALL || data->update == EX_TREE_UPDATE_DIRS)
     etk_tree_col_sort_full(e->cur_tab->dcol, _ex_main_dtree_compare_cb,
			    NULL, ETK_TRUE);   
   
   etk_tree_thaw(ETK_TREE(e->cur_tab->itree));
   etk_tree_thaw(ETK_TREE(e->cur_tab->dtree));   

   if (!e->cur_tab->monitor)
     {
	D(("Adding monitoring to path %s\n", e->cur_tab->dir)); 
	e->cur_tab->monitor = ecore_file_monitor_add(e->cur_tab->dir, 
	      _ex_main_monitor_dir, NULL);
     }
      
   for (cur = 0; cur < data->num; cur++)
     if (data->entries[cur])
       free(data->entries[cur]);
   if (data->entries)
     free(data->entries);
   cur = 0;
   free(data->selected_file);
   free(data);
   return 0;
}

void
_ex_main_populate_files(const char *selected_file, Ex_Tree_Update update)
{
   char back[PATH_MAX];
   Ex_Populate_Data *data;
   char **entries;
   struct dirent *dir_entry;
   DIR *dir;
   int j = 1;
   int i = 0;
   int (*cmp)(const void *, const void *) = _ex_sort_cmp_name;
	
   _ex_main_image_unset();
   //chdir(e->cur_tab->dir);
   
   if (update == EX_TREE_UPDATE_ALL || update == EX_TREE_UPDATE_DIRS)
     {
	snprintf(back, PATH_MAX, "..");
	etk_tree_row_append(ETK_TREE(e->cur_tab->dtree), NULL, e->cur_tab->dcol,
	              etk_theme_icon_path_get(), "actions/go-up_16", back, NULL);
     }

   if (e->cur_tab->monitor)
     {
	D(("Removing old monitoring\n"));
	ecore_file_monitor_del(e->cur_tab->monitor);
	e->cur_tab->monitor = NULL;
     }
   
   etk_entry_text_set(ETK_ENTRY(etk_combobox_entry_entry_get(ETK_COMBOBOX_ENTRY(e->combobox_entry))), e->cur_tab->dir);
   
   data = calloc(1, sizeof(Ex_Populate_Data));
   if (selected_file)
     data->selected_file = strdup(selected_file);
   else
     data->selected_file = NULL;
   data->update = update;
	
   dir = opendir(e->cur_tab->dir);
   
   if (!dir)
     {
	fprintf(stderr, "Could not open dir!\n");
	return;
     }
   
   entries = calloc(FILELIST_SIZE, sizeof(char *));
   
   while ((dir_entry = readdir(dir)) != NULL)
     {
	if (i == FILELIST_SIZE)
	  {      
	     i = 0;
	     ++j;
	     entries = realloc(entries, j * FILELIST_SIZE * sizeof(char *));        
	  }
	
	entries[(j - 1) * FILELIST_SIZE + i] = strdup(dir_entry->d_name);
	++i;
     }
   
   if (j == 1)
     j = 0;
   else
     j -= 1;
   
   if (data->update == EX_TREE_UPDATE_FILES || data->update == EX_TREE_UPDATE_ALL)
     {
	if (e->options->default_sort == EX_SORT_BY_DATE)
	  cmp = _ex_sort_cmp_date;
	else if (e->options->default_sort == EX_SORT_BY_SIZE)
	  cmp = _ex_sort_cmp_size;
	else if (e->options->default_sort == EX_SORT_BY_NAME)
	  cmp = _ex_sort_cmp_name;
	else if (e->options->default_sort == EX_SORT_BY_RESOLUTION)
	  cmp = _ex_sort_cmp_resol;
     }
   
   qsort(entries, FILELIST_SIZE * j + i, sizeof(char *), cmp);
   
   data->entries = entries;
   data->num = FILELIST_SIZE * j + i;
   closedir(dir);   

   ecore_timer_add(0.001, _ex_main_populate_files_timer_cb, data);  
   
}

void
_ex_main_row_data_free(void *data) {
   if (data)
      free(data);
}

void
_ex_main_itree_add(const char *file, const char *selected_file)
{
   Etk_Tree_Row *row;
   Ex_Thumb *thumb;

   if (!file)
     {
	D(("ERROR: file is NULL\n"));
	return;
     }

   if (ecore_file_size(file) <= 0)
     return;

   if(!e->options->thumbs_show)
     {
	row = etk_tree_row_append(ETK_TREE(e->cur_tab->itree), NULL, 
				   e->cur_tab->icol,
				   NULL, NULL, 
				   basename((char *) file), NULL);
	etk_tree_row_data_set_full(row, strdup(file), _ex_main_row_data_free);
	return;
     }
   
   thumb = calloc(1, sizeof(Ex_Thumb));
   thumb->ep = NULL;
   thumb->e = e;
   thumb->name = strdup(basename((char *) file));
   thumb->tab = e->cur_tab;
   if(selected_file)
     {
	if(!strcmp(selected_file, file))
	  thumb->selected = ETK_TRUE;
     }
   else
     thumb->selected = ETK_FALSE;

   epsilon_request_add(file, EPSILON_THUMB_NORMAL, thumb);
   
   row = etk_tree_row_append(ETK_TREE(e->cur_tab->itree), NULL, 
	 e->cur_tab->icol,
	 PACKAGE_DATA_DIR"/gui.edj", "thumb_loading",
	 basename((char *) file), NULL);
   etk_tree_row_data_set_full(row, strdup(file), _ex_main_row_data_free);
}

static void
_ex_main_monitor_dir(void *data, Ecore_File_Monitor *ecore_file_monitor, Ecore_File_Event event, const char *path)
{
   struct stat st;
   Etk_Tree_Row *row;
   
   /* TODO: update non-visible tabs too */
   
   /* Only do changes if trees are visible */
   if (ecore_file_monitor != e->cur_tab->monitor)
     return;

   stat(path, &st);

   if(_ex_file_is_viewable(basename((char *) path)) || 
	 S_ISDIR(st.st_mode))
     {
	switch (event)
	  {
	   case ECORE_FILE_EVENT_CREATED_DIRECTORY:
	      etk_tree_clear(ETK_TREE(e->cur_tab->dtree));
	      etk_combobox_entry_clear(ETK_COMBOBOX_ENTRY(e->combobox_entry));
	      _ex_main_populate_files(NULL, EX_TREE_UPDATE_DIRS);
	      break;
	   case ECORE_FILE_EVENT_DELETED_DIRECTORY:
	      etk_tree_clear(ETK_TREE(e->cur_tab->dtree));
	      etk_combobox_entry_clear(ETK_COMBOBOX_ENTRY(e->combobox_entry));
	      _ex_main_populate_files(NULL, EX_TREE_UPDATE_DIRS);
	      break;
	   case ECORE_FILE_EVENT_DELETED_SELF:
	      etk_tree_clear(ETK_TREE(e->cur_tab->dtree));
	      etk_tree_clear(ETK_TREE(e->cur_tab->itree));
	      etk_combobox_entry_clear(ETK_COMBOBOX_ENTRY(e->combobox_entry));
	      _ex_main_populate_files(NULL, EX_TREE_UPDATE_ALL);
	      break;
	   case ECORE_FILE_EVENT_MODIFIED:
	      /* Careful with what to do here.. */
	      if ((row =  _ex_image_find_row_from_file(e->cur_tab, path)))
	       {
		  _ex_thumb_update_at_row(row);
	       }
	      break;
	   case ECORE_FILE_EVENT_DELETED_FILE:
	      /* TODO: remove the correct item from tree, don't refresh all */
	      etk_tree_clear(ETK_TREE(e->cur_tab->itree));
	      _ex_main_populate_files(NULL, EX_TREE_UPDATE_ALL);
	      break;
	   case ECORE_FILE_EVENT_CREATED_FILE:
	      if (e->options->monitor_focus)
		 _ex_main_itree_add(path, path);
	      else
		 _ex_main_itree_add(path, NULL);
	      break;
	   default:
	      D(("Unknown ecore file event occured\n"));
	      break;
	  }
	
	D(("Monitor event %d happened in %s\n", event, path));
     }
}

static Etk_Bool
_ex_main_entry_dir_key_down_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Event_Key_Down *ev = event;
   Etk_Bool stop_signal = ETK_FALSE;

   if (!strcmp(ev->key, "Tab"))
     {
	const char *path;
	const char *dir;
	const char *file;
	Evas_List *l;
        
        /* Stop the propagation of the signal so the focus won't be passed to the next widget */
        stop_signal = ETK_TRUE;
	
	path = etk_entry_text_get(ETK_ENTRY(etk_combobox_entry_entry_get(ETK_COMBOBOX_ENTRY(e->combobox_entry))));
	dir = ecore_file_dir_get((char*)path);
	file = ecore_file_file_get(path);
	
	if(!dir || !strcmp(dir, ""))
	  dir = strdup("/");
		
	if(!file || !strcmp(file, path))
	  return stop_signal;
	
	D(("dir='%s' file='%s'\n", dir, file));
	
	//if(strcmp(dir, e->cur_path))
	  {
	     DIR *dirfd;
	     struct dirent *dir_entry;
	     struct stat st;
	     	     
	     while(e->cur_tab->dirs)
	       {
		  e->cur_tab->dirs = evas_list_remove_list(e->cur_tab->dirs, e->cur_tab->dirs);
	       }
	     
	     if ((dirfd = opendir(dir)) == NULL)
	       return stop_signal;
	     
	     while ((dir_entry = readdir(dirfd)) != NULL)
	       {
		  char fullpath[PATH_MAX];
		  
		  if (!strcmp(dir_entry->d_name, ".")||!strcmp(dir_entry->d_name, ".."))
		    continue;
		  
		  snprintf(fullpath, PATH_MAX, "%s/%s", dir, dir_entry->d_name);
		  if(stat(fullpath, &st) == -1) continue;

		  if(S_ISDIR(st.st_mode))
		    e->cur_tab->dirs = evas_list_append(e->cur_tab->dirs, strdup(dir_entry->d_name));
	       }
	     
	     closedir(dirfd);
	  }			
	
	for(l = e->cur_tab->dirs; l; l = l->next)
	  {
	     if(!strncmp(file, l->data, strlen(file)))
	       {
		  char fullpath[PATH_MAX];
		  snprintf(fullpath, PATH_MAX, "%s/%s/", dir, (char*)l->data);
		  etk_entry_text_set(ETK_ENTRY(etk_combobox_entry_entry_get(ETK_COMBOBOX_ENTRY(e->combobox_entry))), fullpath);
		  break;
	       }	
	  }
	
     }
   
   if (!strcmp(ev->key, "Return") || !strcmp(ev->key, "KP_Enter"))
     {
	Etk_Widget *entry;
	
	entry = etk_combobox_entry_entry_get(ETK_COMBOBOX_ENTRY(e->combobox_entry));
	
	_ex_slideshow_stop();
	_ex_thumb_abort();
        e->cur_tab->dir = _ex_path_normalize((char*)etk_entry_text_get(ETK_ENTRY(entry)));
        etk_tree_clear(ETK_TREE(e->cur_tab->itree));
        etk_tree_clear(ETK_TREE(e->cur_tab->dtree));
	etk_combobox_entry_clear(ETK_COMBOBOX_ENTRY(e->combobox_entry));
        _ex_main_populate_files(NULL, EX_TREE_UPDATE_ALL);
     }

   return stop_signal;
}

static void _ex_main_combobox_entry_changed_cb(Etk_Object *object, void *data)
{
   char *dir;
   
   etk_combobox_entry_item_fields_get(etk_combobox_entry_active_item_get(ETK_COMBOBOX_ENTRY(e->combobox_entry)),
				      &dir, NULL);   
   _ex_slideshow_stop();
   _ex_thumb_abort();
   e->cur_tab->dir = strdup(dir);
   etk_tree_clear(ETK_TREE(e->cur_tab->itree));
   etk_tree_clear(ETK_TREE(e->cur_tab->dtree));
   etk_combobox_entry_clear(ETK_COMBOBOX_ENTRY(e->combobox_entry));
   _ex_main_populate_files(NULL, EX_TREE_UPDATE_ALL);   
}

static Etk_Bool
_ex_main_window_deleted_cb(void *data)
{  
   etk_main_quit();
   return 1;
}

static void
_ex_main_window_key_down_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Event_Key_Down *ev = event;   
   
   if (ev->modifiers & ETK_MODIFIER_CTRL)
     {
	if (!strcmp(ev->key, "t"))
	  {
	     Ex_Tab *tab;
	     
	     tab = _ex_tab_new(e, e->cur_tab->dir);

	     _ex_main_window_tab_append(tab);
	     _ex_main_populate_files(NULL, EX_TREE_UPDATE_ALL);
	  }
	else if(!strcmp(ev->key, "w"))
	  {
	     _ex_tab_delete();
	  }
	else if(!strcmp(ev->key, "q"))
	  {	
	     etk_main_quit();		
	  }
	else if(!strcmp(ev->key, "d"))
	  {
	     Etk_Tree_Row *r;
	     char         *icol_string;
	     
	     r = etk_tree_selected_row_get(ETK_TREE(e->cur_tab->itree));
	     if(!r) return;
	     
	     etk_tree_row_fields_get(r, etk_tree_nth_col_get(ETK_TREE(e->cur_tab->itree), 0),
		   &icol_string, NULL, etk_tree_nth_col_get(ETK_TREE(e->cur_tab->itree), 1), NULL);
	     _ex_favorites_add(e, icol_string);
	  }
	else if(!strcmp(ev->key, "x"))
	  {
	     if(!e->cur_tab->fit_window)
	       _ex_tab_current_fit_to_window(e);
	     else
	       _ex_tab_current_zoom_one_to_one(e);	
	  }
	else if(!strcmp(ev->key, "s"))
	  {
	     _ex_main_window_slideshow_toggle();
	  }		   
	else if(!strcmp(ev->key, "f"))
	  {
	     _ex_main_window_fullscreen_toggle();
	  }		   
     }
   
   /* extra keyboard shortcuts, regardless of modifiders */
   if (!strcmp(ev->key, "KP_Add") || !strcmp(ev->key, "plus"))
     _ex_tab_current_zoom_in(e);
   else if (!strcmp(ev->key, "KP_Subtract") || !strcmp(ev->key, "minus"))
     _ex_tab_current_zoom_out(e);

   /* only active when in full screen mode */
   if(etk_window_fullscreen_get(ETK_WINDOW(e->win)))
     {
	if(!strcmp(ev->key, "Escape") || !strcmp(ev->key, "f"))
	  _ex_main_window_fullscreen_toggle();	
	if(!strcmp(ev->key, "space")) 
	  _ex_slideshow_next(e);	
	if(!strcmp(ev->key, "b")) 	  
	  _ex_slideshow_prev(e);	  
     }
}

static void
_ex_main_window_resize_cb(Etk_Object *object, void *data)
{
   etk_window_geometry_get(ETK_WINDOW(object), NULL, NULL,
			   &e->options->last_w, &e->options->last_h);
}

void
_ex_main_window_slideshow_toggle(void)
{
   if (e->slideshow.active)     
     _ex_slideshow_stop();     
   else     
     _ex_slideshow_start();
}

void
_ex_main_window_fullscreen_toggle()
{
   if (etk_window_fullscreen_get(ETK_WINDOW(e->win)))
     {
	etk_signal_connect("resized", ETK_OBJECT(e->win), ETK_CALLBACK(_ex_main_window_resize_cb), e);
	etk_container_remove(e->vboxf);
	etk_container_add(ETK_CONTAINER(e->win), e->vbox);
	etk_widget_show_all(e->win);
	etk_container_add(ETK_CONTAINER(e->hpaned_shadow), e->notebook);
	if (evas_list_count(e->tabs) > 1)
	  etk_notebook_tabs_visible_set(ETK_NOTEBOOK(e->notebook), ETK_TRUE);
	else
	  etk_notebook_tabs_visible_set(ETK_NOTEBOOK(e->notebook), ETK_FALSE);
	etk_window_fullscreen_set(ETK_WINDOW(e->win), ETK_FALSE);	
     }
   else
     {
	etk_signal_disconnect("resized", ETK_OBJECT(e->win), ETK_CALLBACK(_ex_main_window_resize_cb), e);
	etk_window_fullscreen_set(ETK_WINDOW(e->win), ETK_TRUE);
	etk_widget_hide(e->statusbar[0]);
	etk_widget_hide(e->statusbar[1]);
	etk_widget_hide(e->statusbar[2]);
	etk_widget_hide(e->statusbar[3]);
	etk_widget_hide(e->sort_bar);
	etk_container_remove(e->vbox);
	e->vboxf = etk_vbox_new(ETK_FALSE, 0);
	etk_container_add(ETK_CONTAINER(e->win), e->vboxf);
	etk_box_append(ETK_BOX(e->vboxf), e->notebook, 
		       ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
	etk_widget_show(e->vboxf);
	etk_widget_show(e->notebook);
	etk_notebook_tabs_visible_set(ETK_NOTEBOOK(e->notebook), ETK_FALSE);
     }
}

static void 
_ex_main_window_tab_toggled_cb(Etk_Object *object, void *data)
{     
   Ex_Tab  *tab;
   int num;
   
   _ex_slideshow_stop();

   num = etk_notebook_current_page_get(ETK_NOTEBOOK(object));
   if (num < 0)
     return;
   
   tab = evas_list_nth(e->tabs, num);

   e->cur_tab = tab;
   D(("Toggeled tab %p number %d\n", tab, e->cur_tab->num));

   D(("Selecting tab %d\n", e->cur_tab->num));
   _ex_tab_select(tab);
   etk_entry_text_set(ETK_ENTRY(etk_combobox_entry_entry_get(ETK_COMBOBOX_ENTRY(e->combobox_entry))), e->cur_tab->dir);
}

void
_ex_main_window_tab_append(Ex_Tab *tab)
{
   if (evas_list_count(e->tabs) >= 1)
     etk_notebook_tabs_visible_set(ETK_NOTEBOOK(e->notebook), ETK_TRUE);
   else
     etk_notebook_tabs_visible_set(ETK_NOTEBOOK(e->notebook), ETK_FALSE);     

   e->tabs = evas_list_append(e->tabs, tab);
   e->cur_tab = tab;
   etk_notebook_page_append(ETK_NOTEBOOK(e->notebook), _ex_file_get(e->cur_tab->dir), e->cur_tab->scrolled_view);
   etk_notebook_current_page_set(ETK_NOTEBOOK(e->notebook), evas_list_count(e->tabs) - 1);

   tab->num = etk_notebook_current_page_get(ETK_NOTEBOOK(e->notebook));
}

static void 
_etk_main_drag_drop_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Event_Selection_Request *ev = event;
   Etk_Selection_Data_Files *files;
   int i;
   
   if (ev->content != ETK_SELECTION_CONTENT_FILES)
     return;
   
   files = ev->data;
   
   for (i = 0; i < files->num_files; i++)
     {
	char *dir;
	char *file;
	
	if ((file = strstr(files->files[i], "file://")) == NULL)
	  continue;
			
	file += strlen("file://");	
	dir = ecore_file_dir_get(file);
	
	if(!ecore_file_is_dir(dir))
	  continue;
	
	if (!ecore_file_exists(file))
	  file = NULL;
	
	E_FREE(e->cur_tab->dir);
	e->cur_tab->dir = strdup(dir);
	etk_tree_clear(ETK_TREE(e->cur_tab->itree));
	etk_tree_clear(ETK_TREE(e->cur_tab->dtree));
	etk_combobox_entry_clear(ETK_COMBOBOX_ENTRY(e->combobox_entry));
	_ex_main_populate_files(ecore_file_file_get(file), EX_TREE_UPDATE_ALL);
	if (ecore_file_exists(file) && !ecore_file_is_dir(file))
	  _ex_main_image_set(e, file);
	etk_notebook_page_tab_label_set(ETK_NOTEBOOK(e->notebook), e->cur_tab->num, 
	      _ex_file_get(e->cur_tab->dir));
	break;
     }
}

void 
_ex_main_dialog_show(char *text, Etk_Message_Dialog_Type type) 
{
     Etk_Widget *dialog;

     dialog = etk_message_dialog_new(type, ETK_MESSAGE_DIALOG_OK, text);
     etk_signal_connect_swapped("response", ETK_OBJECT(dialog),
				ETK_CALLBACK(etk_object_destroy), dialog);

     etk_window_title_set(ETK_WINDOW(dialog), _("Exhibit - Notice"));
     etk_widget_show_all(dialog);
}


void
_ex_main_window_show(char *dir, int fullscreen, int slideshow)
{
   Ex_Tab *tab;
   Etk_Widget *entry_hbox, *toolbar;
   char file[PATH_MAX];
   char *homedir;
   const char **dnd_types;
   int dnd_types_num;
  
   memset(file, 0, PATH_MAX);

   e = calloc(1, sizeof(Exhibit));
   e->mouse.down = 0;
   e->menu = NULL;
   e->tabs = NULL;
   e->slideshow.active = ETK_FALSE;
   e->slideshow.interval = 5.0;
   
   e->options = _ex_options_new();
   /* Now load saved options */
   _ex_options_load(e);  
   
   if(!e->options->fav_path)
     {
	int size = PATH_MAX * sizeof(char);
	e->options->fav_path = malloc(size);
	homedir = getenv("HOME");
	if (!homedir) 
	  snprintf(e->options->fav_path, size, "%s", "/tmp/exhibit_favorites");
	else
	  {
	     snprintf(e->options->fav_path, size, "%s/.e", homedir);
	     if(!ecore_file_is_dir(e->options->fav_path))
	       ecore_file_mkdir(e->options->fav_path);
	     snprintf(e->options->fav_path, size, "%s/.e/exhibit", homedir);
	     if(!ecore_file_is_dir(e->options->fav_path))
	       ecore_file_mkdir(e->options->fav_path);
	     snprintf(e->options->fav_path, size, "%s/.e/exhibit/favorites", homedir);
	  }
     }
   
   if(!ecore_file_is_dir(e->options->fav_path))
     ecore_file_mkdir(e->options->fav_path);
   
   tab = NULL;
   
   e->win = etk_window_new();   
   etk_widget_dnd_dest_set(e->win, ETK_TRUE);
   dnd_types_num = 1;
   dnd_types = calloc(dnd_types_num, sizeof(char*));
   dnd_types[0] = strdup("text/uri-list");
   // DND disabled for now
   //etk_signal_connect("drag-drop", ETK_OBJECT(e->win), ETK_CALLBACK(_etk_main_drag_drop_cb), e);
   etk_widget_dnd_types_set(e->win, dnd_types, dnd_types_num);   
   etk_window_title_set(ETK_WINDOW(e->win), WINDOW_TITLE " - Image Viewing the Kewl Way!");
   etk_window_wmclass_set(ETK_WINDOW(e->win), "exhibit", "Exhibit");
   etk_window_resize(ETK_WINDOW(e->win), e->options->last_w, e->options->last_h);
   etk_signal_connect("delete-event", ETK_OBJECT(e->win), ETK_CALLBACK(_ex_main_window_deleted_cb), e);
   etk_signal_connect("key-down", ETK_OBJECT(e->win), ETK_CALLBACK(_ex_main_window_key_down_cb), e);
   etk_signal_connect("resized", ETK_OBJECT(e->win), ETK_CALLBACK(_ex_main_window_resize_cb), e);

   e->vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(e->win), e->vbox);
               
   e->hpaned = etk_hpaned_new();
   etk_box_append(ETK_BOX(e->vbox), e->hpaned, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);   

   e->vpaned = etk_vpaned_new();
   etk_paned_child1_set(ETK_PANED(e->hpaned), e->vpaned, ETK_FALSE);
   
   e->table = etk_table_new(4, 4, ETK_TABLE_NOT_HOMOGENEOUS);
   etk_paned_child1_set(ETK_PANED(e->vpaned), e->table, ETK_TRUE);

   e->menu_bar = etk_menu_bar_new();
   etk_table_attach(ETK_TABLE(e->table), e->menu_bar, 0, 4, 0, 0, ETK_TABLE_FILL | ETK_TABLE_HEXPAND, 0, 0);
   
     {
	Etk_Widget *menu;
	Etk_Widget *submenu;
	Etk_Widget *menu_item;
	
	/* Create "File" menu item */
	menu_item = _ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("File"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(e->menu_bar), NULL, NULL);
	/* Create submenu for "File" item */
	menu = etk_menu_new();
	
	/* Set the File submenu to the File menu item and populate it */
	etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));
	
	menu_item = _ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("New"), ETK_STOCK_WINDOW_NEW, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_run_in_cb), e);
	submenu = etk_menu_new();
	etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(submenu));
	
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Tab"), ETK_STOCK_TAB_NEW, ETK_MENU_SHELL(submenu), ETK_CALLBACK(_ex_menu_new_tab_cb), NULL);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Save image"), ETK_STOCK_DOCUMENT_SAVE, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_save_image_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Save image as"), ETK_STOCK_DOCUMENT_SAVE, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_save_image_as_cb), NULL);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Set as wallpaper"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_set_wallpaper_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_SEPARATOR, NULL, ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), NULL, NULL);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Rename"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_rename_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Delete"), ETK_STOCK_X_DIRECTORY_TRASH, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_delete_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Move"), ETK_STOCK_FORMAT_INDENT_MORE, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_move_cb), NULL);
	_ex_menu_item_new(EX_MENU_ITEM_SEPARATOR, NULL, ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), NULL, NULL);
	
	menu_item = _ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Close"), ETK_STOCK_LIST_REMOVE, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_run_in_cb), e);
	submenu = etk_menu_new();
	etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(submenu));

	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Tab"), ETK_STOCK_TAB_NEW, ETK_MENU_SHELL(submenu), ETK_CALLBACK(_ex_menu_delete_tab_cb), NULL);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Quit"), ETK_STOCK_DIALOG_CLOSE, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_quit_cb), e);
	
	/* Create "Edit" menu item */
	menu_item = _ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Edit"), -99, ETK_MENU_SHELL(e->menu_bar), NULL, NULL);
	/* Create submenu for "Edit" menu */
	menu = etk_menu_new();
	
	/* Set the File submenu to the File menu item and populate it */	
	etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));       	
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Undo"), ETK_STOCK_EDIT_UNDO, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_undo_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_SEPARATOR, NULL, ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), NULL, NULL);
	
	/* Open With submenu */
	menu_item = _ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Open With..."), ETK_STOCK_APPLICATION_X_EXECUTABLE, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_run_in_cb), e);
	e->submenu = etk_menu_new();
	etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(e->submenu));

	_ex_menu_build_run_menu(NULL);

	/* Continue "Edit" menu */
	_ex_menu_item_new(EX_MENU_ITEM_SEPARATOR, NULL, ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), NULL, NULL);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Rotate clockwise"), ETK_STOCK_GO_NEXT, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_rot_clockwise_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Rotate counterclockwise"), ETK_STOCK_GO_PREVIOUS, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_rot_counter_clockwise_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Flip horizontally"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_flip_horizontal_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Flip vertically"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_flip_vertical_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_SEPARATOR, NULL, ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), NULL, NULL);
	
	menu_item = _ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Effects"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_run_in_cb), e);
	submenu = etk_menu_new();
	etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(submenu));
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Blur"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(submenu), ETK_CALLBACK(_ex_menu_blur_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Sharpen"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(submenu), ETK_CALLBACK(_ex_menu_sharpen_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Brighten"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(submenu), ETK_CALLBACK(_ex_menu_brighten_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_SEPARATOR, NULL, ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), NULL, NULL);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Options"), ETK_STOCK_PREFERENCES_SYSTEM, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_options_cb), e);

	/* Create the "View" menu item */
	menu_item = _ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("View"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(e->menu_bar), NULL, NULL);
	menu = etk_menu_new();
	etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));
	
	menu_item = _ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Sort"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_run_in_cb), e);
	submenu = etk_menu_new();
	etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(submenu));
	menu_item = _ex_menu_item_new(EX_MENU_ITEM_RADIO, _("Date"), ETK_STOCK_OFFICE_CALENDAR, ETK_MENU_SHELL(submenu), ETK_CALLBACK(_ex_sort_date_cb), NULL);
	if (e->options->default_sort == EX_SORT_BY_DATE)
	  etk_menu_item_check_active_set(ETK_MENU_ITEM_CHECK(menu_item), ETK_TRUE);
	menu_item = _ex_menu_item_new(EX_MENU_ITEM_RADIO, _("Size"), ETK_STOCK_DRIVE_HARDDISK, ETK_MENU_SHELL(submenu), ETK_CALLBACK(_ex_sort_size_cb), menu_item);
	if (e->options->default_sort == EX_SORT_BY_SIZE)
	  etk_menu_item_check_active_set(ETK_MENU_ITEM_CHECK(menu_item), ETK_TRUE);	
	menu_item = _ex_menu_item_new(EX_MENU_ITEM_RADIO, _("Name"), ETK_STOCK_TEXT_X_GENERIC, ETK_MENU_SHELL(submenu), ETK_CALLBACK(_ex_sort_name_cb), menu_item);
	if (e->options->default_sort == EX_SORT_BY_NAME)
	  etk_menu_item_check_active_set(ETK_MENU_ITEM_CHECK(menu_item), ETK_TRUE);	
	menu_item = _ex_menu_item_new(EX_MENU_ITEM_RADIO, _("Resolution"), ETK_STOCK_UTILITIES_SYSTEM_MONITOR, ETK_MENU_SHELL(submenu), ETK_CALLBACK(_ex_sort_resol_cb), menu_item);
	if (e->options->default_sort == EX_SORT_BY_RESOLUTION)
	  etk_menu_item_check_active_set(ETK_MENU_ITEM_CHECK(menu_item), ETK_TRUE);
	
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Zoom in"), EX_IMAGE_ZOOM_IN, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_zoom_in_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Zoom out"), EX_IMAGE_ZOOM_OUT, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_zoom_out_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Zoom 1:1"), EX_IMAGE_ONE_TO_ONE, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_zoom_one_to_one_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Fit to window"), EX_IMAGE_FIT_TO_WINDOW, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_fit_to_window_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Fullscreen"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_window_fullscreen_toggle_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_SEPARATOR, NULL, ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), NULL, NULL);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Toggle slideshow"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_toggle_slideshow_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Refresh"), ETK_STOCK_VIEW_REFRESH, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_refresh_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Toggle Comments"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_comments_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_SEPARATOR, NULL, ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), NULL, NULL);		
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Add to favorites"), ETK_STOCK_EMBLEM_PHOTOS, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_add_to_fav_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("View favorites"), ETK_STOCK_EMBLEM_FAVORITE, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_go_to_fav_cb), e);
	
	/* Create the "Help" menu */
	menu_item = _ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Help"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(e->menu_bar), NULL, NULL);
	menu = etk_menu_new();
	etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("About"), ETK_STOCK_HELP_BROWSER, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_about_cb), e);
     }   
   
   toolbar = etk_toolbar_new();
   etk_table_attach(ETK_TABLE(e->table), toolbar,
		    0, 3, 1, 1,
		    ETK_TABLE_HEXPAND | ETK_TABLE_HFILL, 0, 0);
     
   e->zoom_in[0] = etk_tool_button_new();
   e->zoom_in[1] = etk_image_new_from_edje(PACKAGE_DATA_DIR"/gui.edj", "zoom_in");
   etk_button_image_set(ETK_BUTTON(e->zoom_in[0]), ETK_IMAGE(e->zoom_in[1]));
   etk_signal_connect_swapped("clicked", ETK_OBJECT(e->zoom_in[0]), ETK_CALLBACK(_ex_tab_current_zoom_in), e);
   etk_toolbar_append(ETK_TOOLBAR(toolbar), e->zoom_in[0], ETK_BOX_START);

   e->zoom_out[0] = etk_tool_button_new();
   e->zoom_out[1] = etk_image_new_from_edje(PACKAGE_DATA_DIR"/gui.edj", "zoom_out");
   etk_button_image_set(ETK_BUTTON(e->zoom_out[0]), ETK_IMAGE(e->zoom_out[1]));
   etk_signal_connect_swapped("clicked", ETK_OBJECT(e->zoom_out[0]), ETK_CALLBACK(_ex_tab_current_zoom_out), e);
   etk_toolbar_append(ETK_TOOLBAR(toolbar), e->zoom_out[0], ETK_BOX_START);

   e->fit[0] = etk_tool_button_new();
   e->fit[1] = etk_image_new_from_edje(PACKAGE_DATA_DIR"/gui.edj", "fit_to_window");
   etk_button_image_set(ETK_BUTTON(e->fit[0]), ETK_IMAGE(e->fit[1]));
   etk_signal_connect_swapped("clicked", ETK_OBJECT(e->fit[0]), ETK_CALLBACK(_ex_tab_current_fit_to_window), e);
   etk_toolbar_append(ETK_TOOLBAR(toolbar), e->fit[0], ETK_BOX_START);

   e->original[0] = etk_tool_button_new();
   e->original[1] = etk_image_new_from_edje(PACKAGE_DATA_DIR"/gui.edj", "one_to_one");
   etk_button_image_set(ETK_BUTTON(e->original[0]), ETK_IMAGE(e->original[1]));
   etk_signal_connect_swapped("clicked", ETK_OBJECT(e->original[0]), ETK_CALLBACK(_ex_tab_current_zoom_one_to_one), e);
   etk_toolbar_append(ETK_TOOLBAR(toolbar), e->original[0], ETK_BOX_START);

   entry_hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_table_attach(ETK_TABLE(e->table), entry_hbox, 0, 3, 2, 2, ETK_TABLE_HEXPAND | ETK_TABLE_HFILL, 0, 0);

   e->combobox_entry = etk_combobox_entry_new_default();   
   etk_box_append(ETK_BOX(entry_hbox), e->combobox_entry, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   etk_signal_connect("key-down", ETK_OBJECT(e->combobox_entry), ETK_CALLBACK(_ex_main_entry_dir_key_down_cb), e);
   etk_signal_connect("active-item-changed", ETK_OBJECT(e->combobox_entry), ETK_CALLBACK(_ex_main_combobox_entry_changed_cb), e);

   /* create first tab but dont place it in notebook */
   if (dir)
     {
	if (ecore_file_is_dir(dir))	  	     
	  tab = _ex_tab_new(e, dir);   
	else if (ecore_file_exists(dir))
	  {	     
	     char *dir2;
	     
	     dir2 = ecore_file_dir_get(ecore_file_realpath(dir));
	     tab = _ex_tab_new(e, dir2);
	     E_FREE(dir2);
	     realpath(dir, file);
	  }
	else if (ecore_file_download_protocol_available("http://"))
	  {
	     char *ptr;
	     char *tmp_file;
	     int i;
	     
	     if ((ptr = strrchr(dir, '/'))) ptr++;

	     if (!ptr)
	       ptr = dir;

	     if (!_ex_file_is_viewable(ptr))
	       {
		  tab = _ex_tab_new(e, ".");
	       }
	     else
	       {
		  tmp_file = malloc(PATH_MAX);
		  for (i = 0;;i++)
		    {
		       snprintf(tmp_file, PATH_MAX, "%s/ex_%d_%s", 
			     e->options->dl_path, i, ptr);
		       
		       if (!ecore_file_exists(tmp_file))
			 break;
		    }
		  
		  tab = _ex_tab_new(e, e->options->dl_path);
		  
		  if (ecore_file_download(dir, tmp_file,
			   _ex_file_download_complete_cb,
			   _ex_file_download_progress_cb, tmp_file))
		    {
		       D(("Starting download to %s\n", tmp_file));
		       _ex_file_download_dialog(dir, tmp_file);
		    }
	       }

	  }
	else     
	  tab = _ex_tab_new(e, ".");
     }
   else
     tab = _ex_tab_new(e, ".");
   
   e->hpaned_shadow = etk_shadow_new();
   etk_shadow_shadow_set(ETK_SHADOW(e->hpaned_shadow), ETK_SHADOW_INSIDE,
     ETK_SHADOW_LEFT, 5, 2, 0, 115);
   etk_paned_child2_set(ETK_PANED(e->hpaned), e->hpaned_shadow, ETK_TRUE);
   
   e->notebook = etk_notebook_new();
   etk_notebook_tabs_homogeneous_set(ETK_NOTEBOOK(e->notebook), ETK_TRUE);
   etk_notebook_tabs_visible_set(ETK_NOTEBOOK(e->notebook), ETK_FALSE);
   etk_container_add(ETK_CONTAINER(e->hpaned_shadow), e->notebook);
   etk_signal_connect("current-page-changed", ETK_OBJECT(e->notebook), 
	 ETK_CALLBACK(_ex_main_window_tab_toggled_cb), NULL);
      
   e->hbox = etk_hbox_new(ETK_TRUE, 0);
   e->sort_bar = etk_statusbar_new();
   etk_statusbar_message_push(ETK_STATUSBAR(e->sort_bar), "Sort by date", 0);
   etk_statusbar_has_resize_grip_set(ETK_STATUSBAR(e->sort_bar), ETK_FALSE);
   etk_box_append(ETK_BOX(e->vbox), e->hbox, ETK_BOX_END, ETK_BOX_NONE, 0);
   etk_box_append(ETK_BOX(e->hbox), e->sort_bar, ETK_BOX_START, ETK_BOX_NONE, 0);
     {
	Etk_Widget *menu;

	menu = etk_menu_new();
	etk_signal_connect("mouse-down", ETK_OBJECT(e->sort_bar), ETK_CALLBACK(_ex_sort_label_mouse_down_cb), menu);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Sort by name"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), NULL, NULL);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Sort by date"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), NULL, NULL);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Sort by size"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), NULL, NULL);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Sort by resolution"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), NULL, NULL);
	_ex_menu_item_new(EX_MENU_ITEM_SEPARATOR, NULL, ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), NULL, NULL);	
	_ex_menu_item_new(EX_MENU_ITEM_CHECK, _("Ascending"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), NULL, NULL);
     }
   
   e->statusbar[0] = etk_statusbar_new();
   etk_statusbar_has_resize_grip_set(ETK_STATUSBAR(e->statusbar[0]), ETK_FALSE);
   etk_box_append(ETK_BOX(e->hbox), e->statusbar[0], ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   
   e->statusbar[1] = etk_statusbar_new();
   etk_statusbar_has_resize_grip_set(ETK_STATUSBAR(e->statusbar[1]), ETK_FALSE);
   etk_box_append(ETK_BOX(e->hbox), e->statusbar[1], ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   e->statusbar[2] = etk_statusbar_new();
   etk_statusbar_has_resize_grip_set(ETK_STATUSBAR(e->statusbar[2]), ETK_FALSE);   
   etk_box_append(ETK_BOX(e->hbox), e->statusbar[2], ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   
   if (e->options->default_view == EX_IMAGE_ONE_TO_ONE) 
      etk_statusbar_message_push(ETK_STATUSBAR(e->statusbar[2]), "1:1", 0);
   else if (e->options->default_view == EX_IMAGE_FIT_TO_WINDOW)
      etk_statusbar_message_push(ETK_STATUSBAR(e->statusbar[2]), "Fit to window", 0);
   
   e->statusbar[3] = etk_statusbar_new();
   etk_box_append(ETK_BOX(e->hbox), e->statusbar[3], ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

   _ex_main_window_tab_append(tab);   
   _ex_main_populate_files(file, EX_TREE_UPDATE_ALL);
   _ex_tab_select(tab);
   
   etk_widget_show_all(e->win);
   
   if(fullscreen)
     _ex_main_window_fullscreen_toggle();
   if (slideshow)
     _ex_main_window_slideshow_toggle();
}

int
main(int argc, char *argv[])
{
   int i;
   int fullscreen = 0, slideshow = 0;
   
   for (i = 1; i < argc; i++)
     {
	if (((!strcmp(argv[i], "-h")) ||
		 (!strcmp(argv[i], "--help"))))
	  {
	     printf("Usage:\n");
	     printf("  %s <image> <options>\n", PACKAGE);
	     printf("  %s <path> <options>\n", PACKAGE);
	     printf("  %s <url> <options>\n\n", PACKAGE);
	     printf("  -f, --fullscreen\t start Exhibit in fullscreen mode\n");
	     printf("  -s, --slideshow\t start Exhibit in slideshow mode\n");
	     printf("  -h, --help\t\t display this help and exit\n");
	     printf("  -v, --version\t\t output version information and exit\n\n");
	     exit(1);
	  }
	else if (((!strcmp(argv[i], "-v")) ||
		 (!strcmp(argv[i], "--version"))))
	  {
	     printf("%s version %s\n", PACKAGE, VERSION);
	     exit(1);
	  }
	else if (((!strcmp(argv[i], "-f")) ||
		 (!strcmp(argv[i], "--fullscreen"))))
	  fullscreen = 1;
	else if (((!strcmp(argv[i], "-s")) ||
		 (!strcmp(argv[i], "--slideshow"))))
	  slideshow = 1;
     }
   
   if (!etk_init(argc, argv))
     {
	fprintf(stderr, "Could not init etk. Exiting...\n");
	return 0;
     };

   if (!efreet_mime_init())
     {
	fprintf(stderr, "Could not init Efreet. Exiting...\n");
	return 0;
     }

   ecore_file_init();
   if(!_ex_options_init())
     fprintf(stderr, "WARNING: Exhibit could not set up its options files!\n"
	   "         You will not be able to save your preferences.\n");

   epsilon_request_init();

   event_handlers = evas_list_append(event_handlers,
	 ecore_event_handler_add(EPSILON_EVENT_DONE, _ex_thumb_complete, NULL));
         
   if(argc > 1 + fullscreen + slideshow)
     _ex_main_window_show(argv[1 + fullscreen + slideshow], fullscreen, slideshow);
   else
     _ex_main_window_show(NULL, fullscreen, slideshow);   
   
   evas_imaging_image_cache_set(IMAGE_CACHE_BYTES);
   /*evas_image_cache_set(etk_toplevel_evas_get(ETK_TOPLEVEL(e->win)), IMAGE_CACHE_NUM);*/
   
   etk_main();         
   if(e)
     _ex_options_save(e);
   _ex_options_shutdown();
   ecore_file_shutdown();
   efreet_mime_shutdown();
   etk_shutdown();

   return 0;
}
