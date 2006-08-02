/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include "exhibit.h"
#include <Ecore_File.h>

extern pid_t pid;
extern Evas_List *thumb_list;

Exhibit *e;
Evas_List  *event_handlers;

static void _ex_main_button_zoom_in_cb(Etk_Object *obj, void *data);
static void _ex_main_button_zoom_out_cb(Etk_Object *obj, void *data);
static void _ex_main_button_zoom_one_to_one_cb(Etk_Object *obj, void *data);
static void _ex_main_button_fit_to_window_cb(Etk_Object *obj, void *data);
static int _ex_main_dtree_compare_cb(Etk_Tree *tree, Etk_Tree_Row *row1, Etk_Tree_Row *row2, Etk_Tree_Col *col, void *data);
static void _ex_main_goto_dir_clicked_cb(Etk_Object *object, void *data);
static void _ex_main_entry_dir_key_down_cb(Etk_Object *object, void *event, void *data);
static Etk_Bool _ex_main_window_deleted_cb(void *data);
static void _ex_main_window_key_down_cb(Etk_Object *object, void *event, void *data);
static void _ex_main_window_resize_cb(Etk_Object *object, void *data);

void
_ex_main_statusbar_zoom_update(Exhibit *e)
{
   if (e->cur_tab->fit_window)
     {
        etk_statusbar_pop(ETK_STATUSBAR(e->statusbar[2]), 0);
        etk_statusbar_push(ETK_STATUSBAR(e->statusbar[2]), _("Fit to window"), 0);
     }
   else if(e->zoom > 0)
     {
	char zoom[6];
	etk_statusbar_pop(ETK_STATUSBAR(e->statusbar[2]), 0);
	snprintf(zoom, sizeof(zoom), "%d:1", abs(e->zoom));
	etk_statusbar_push(ETK_STATUSBAR(e->statusbar[2]), zoom, 0);
     }
   else if(e->zoom < 0)
     {
	char zoom[6];
	etk_statusbar_pop(ETK_STATUSBAR(e->statusbar[2]), 0);
	snprintf(zoom, sizeof(zoom), "1:%d", abs(e->zoom));
	etk_statusbar_push(ETK_STATUSBAR(e->statusbar[2]), zoom, 0);;
     }
   else
     {
	etk_statusbar_pop(ETK_STATUSBAR(e->statusbar[2]), 0);
	etk_statusbar_push(ETK_STATUSBAR(e->statusbar[2]), "1:1", 0);
     }      
}

static void
_ex_main_button_zoom_in_cb(Etk_Object *obj, void *data)
{
   Exhibit *e;
   
   e = data;

   _ex_tab_current_zoom_in(e);
}

static void
_ex_main_button_zoom_out_cb(Etk_Object *obj, void *data)
{
   Exhibit *e;
   
   e = data;

   _ex_tab_current_zoom_out(e);
}

static void
_ex_main_button_zoom_one_to_one_cb(Etk_Object *obj, void *data)
{
   Exhibit *e;
   
   e = data;

   _ex_tab_current_zoom_one_to_one(e);
}

static void
_ex_main_button_fit_to_window_cb(Etk_Object *obj, void *data)
{
   Exhibit *e;
   
   e = data;
   
   _ex_tab_current_fit_to_window(e);
}

void
_ex_main_image_unset(Exhibit *e)
{
   Ex_Tab *tab = e->cur_tab;
   
   etk_window_title_set(ETK_WINDOW(e->win), 
	 WINDOW_TITLE " - Image Viewing the Kewl Way!");

   E_FREE(tab->set_img_path);
   E_FREE(tab->cur_file);
   tab->image_loaded = ETK_FALSE;
   
   etk_statusbar_pop(ETK_STATUSBAR(e->statusbar[0]), 0);
   etk_statusbar_push(ETK_STATUSBAR(e->statusbar[0]), NULL, 0);
   etk_statusbar_pop(ETK_STATUSBAR(e->statusbar[1]), 0);
   etk_statusbar_push(ETK_STATUSBAR(e->statusbar[1]), NULL, 0);

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
      we browse more in the dtree */
   e->cur_tab->set_img_path = malloc(PATH_MAX);
   memset(e->cur_tab->set_img_path, 0, PATH_MAX);
   e->cur_tab->cur_file = malloc(PATH_MAX);
   memset(e->cur_tab->cur_file, 0, PATH_MAX);
   
   sprintf(e->cur_tab->set_img_path, "%s", e->cur_tab->cur_path);
   sprintf(e->cur_tab->cur_file, "%s", image);

   e->cur_tab->image_loaded = ETK_TRUE;
   etk_widget_show(ETK_WIDGET(e->cur_tab->image));
   
   if(_ex_file_is_ebg(image))
     {
	/* can we do this without the size request? it doesnt look good */
	etk_widget_size_request_set(ETK_WIDGET(e->cur_tab->image), 800, 600);	
	etk_image_set_from_edje(ETK_IMAGE(e->cur_tab->image), image, "desktop/background");
     }
   else
     {
	etk_image_set_from_file(ETK_IMAGE(e->cur_tab->image), image);

	if (e->cur_tab->fit_window)
          {
	    _ex_main_button_fit_to_window_cb(NULL, e);
            e->options->default_view = EX_IMAGE_FIT_TO_WINDOW;
          }
	else
          {
	    _ex_main_button_zoom_one_to_one_cb(NULL, e);
            e->options->default_view = EX_IMAGE_ONE_TO_ONE;
          }					
     }
   
   bytes = ecore_file_size(image);
   snprintf(size, sizeof(size), "%d K", (int)(bytes/1024));
   etk_statusbar_pop(ETK_STATUSBAR(e->statusbar[0]), 0);
   etk_statusbar_push(ETK_STATUSBAR(e->statusbar[0]), size, 0);
      
   etk_image_size_get(ETK_IMAGE(e->cur_tab->image), &w, &h);   
   snprintf(size, sizeof(size), "( %d x %d )", w, h);
   etk_statusbar_pop(ETK_STATUSBAR(e->statusbar[1]), 0);
   etk_statusbar_push(ETK_STATUSBAR(e->statusbar[1]), size, 0);
   
   hs = etk_scrolled_view_hscrollbar_get(ETK_SCROLLED_VIEW(e->cur_tab->scrolled_view));
   vs = etk_scrolled_view_vscrollbar_get(ETK_SCROLLED_VIEW(e->cur_tab->scrolled_view));
      
   etk_range_value_set(hs, (double)w/2);
   etk_range_value_set(vs, (double)h/2);

   /* Reset undo data every time we change image */
   D(("Resetting undo data on image %p\n", e->cur_tab->image));
   data2 = etk_object_data_get(ETK_OBJECT(e->cur_tab->image), "undo");
   if (data2) 
     E_FREE(data2);

   etk_object_data_set(ETK_OBJECT(e->cur_tab->image), "undo", NULL);
   
   if(e->cur_tab->comment.visible)
     _ex_comment_load(e);
}

static int
_ex_main_dtree_compare_cb(Etk_Tree *tree, Etk_Tree_Row *row1, Etk_Tree_Row *row2, Etk_Tree_Col *col, void *data)
{
   char *dir1, *dir2;
   
   if (!row1 || !row2 || !col)
      return 0;
   
   etk_tree_row_fields_get(row1, col, NULL, NULL, &dir1, NULL);
   etk_tree_row_fields_get(row2, col, NULL, NULL, &dir2, NULL);
   return strcasecmp(dir1, dir2);
}

void
_ex_main_populate_files(Exhibit *e, const char *selected_file)
{
   char back[PATH_MAX];
   DIR *dir;
   struct dirent *dir_entry;
   Etk_Tree_Row *row, *selected_row;

   selected_row = NULL;
   snprintf(back, PATH_MAX, "..");
   etk_tree_append(ETK_TREE(e->cur_tab->dtree), e->cur_tab->dcol, 
		   etk_theme_icon_theme_get(), "actions/go-up_16", back, NULL);

   chdir(e->cur_tab->dir);

   D(("Changing to dir: %s\n", e->cur_tab->dir));

   if ((dir = opendir(".")) == NULL)
     return ;

   etk_tree_freeze(ETK_TREE(e->cur_tab->itree));
   etk_tree_freeze(ETK_TREE(e->cur_tab->dtree));
   
   getcwd(e->cur_tab->cur_path, PATH_MAX);
   if(strlen(e->cur_tab->cur_path) < PATH_MAX - 2)
     {
	int len = strlen(e->cur_tab->cur_path);
	e->cur_tab->cur_path[len] = '/';
	e->cur_tab->cur_path[len + 1] = '\0';
     }
   etk_entry_text_set(ETK_ENTRY(e->entry[0]), e->cur_tab->cur_path);
   
   while ((dir_entry = readdir(dir)) != NULL)
     {
	char image[PATH_MAX];
	char imagereal[PATH_MAX];
	struct stat st;
	Epsilon *ep;

        /* Do not include hidden files */
	if (dir_entry->d_name[0] == '.')
	  continue;

	snprintf(image, PATH_MAX, "%s", dir_entry->d_name);
	if(stat(image, &st) == -1) continue;
	if(S_ISDIR(st.st_mode))
	  {
	     etk_tree_append(ETK_TREE(e->cur_tab->dtree), e->cur_tab->dcol,
			     etk_theme_icon_theme_get(),
			     "places/folder_16",
			     dir_entry->d_name, NULL);
	     e->cur_tab->dirs = evas_list_append(e->cur_tab->dirs, dir_entry->d_name);
	     continue;
	  }

	if(!_ex_file_is_viewable(dir_entry->d_name))
	  continue;

	if(!realpath(image, imagereal))
	  snprintf(imagereal, PATH_MAX, "%s", image);

	ep = epsilon_new(imagereal);
	epsilon_thumb_size(ep, EPSILON_THUMB_NORMAL);

	if(epsilon_exists(ep) == EPSILON_OK)
	  {
	     char *thumb;

	     thumb = (char*)epsilon_thumb_file_get(ep);
	     row = etk_tree_append(ETK_TREE(e->cur_tab->itree), e->cur_tab->icol, thumb, dir_entry->d_name, NULL);
	     if(selected_file)	       		  		  
	       if(!strcmp(selected_file, dir_entry->d_name))
		 selected_row = row;		    	     
	     E_FREE(thumb);
	  }
	else {
	   Ex_Thumb *thumb;

	   thumb = calloc(1, sizeof(Ex_Thumb));
	   thumb->ep = ep;
	   thumb->e = e;
	   thumb->name = strdup(dir_entry->d_name);
	   thumb_list = evas_list_append(thumb_list, thumb);
	   if(selected_file)
	     {
		if(!strcmp(selected_file, dir_entry->d_name))
		  thumb->selected = ETK_TRUE;
	     }
	   else
	     thumb->selected = ETK_FALSE;
	   if(pid == -1) _ex_thumb_generate();
	}
     }

   etk_tree_thaw(ETK_TREE(e->cur_tab->itree));
   etk_tree_thaw(ETK_TREE(e->cur_tab->dtree));
   etk_tree_sort(ETK_TREE(e->cur_tab->dtree), _ex_main_dtree_compare_cb, ETK_TRUE, e->cur_tab->dcol, NULL);

   if(selected_row)
     {
	etk_tree_row_select(selected_row);
	etk_tree_row_scroll_to(selected_row, ETK_TRUE);
     }

   /* Set the dir to the current dir at the end so we avoid
      stepdown like ".." if we just call the refresh on
      the listing like after a delete */
   e->cur_tab->dir = strdup(".");
   
   closedir(dir);
}

static void
_ex_main_entry_dir_key_down_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Event_Key_Up_Down *ev;
   Exhibit *e;

   e = data;
   ev = event;

   if(!strcmp(ev->key, "Tab"))
     {
	const char *path;
	const char *dir;
	const char *file;
	Evas_List *l;
        
        /* Stop the propagation of the signal so the focus won't be passed to the next widget */
        etk_signal_stop();
	
	path = etk_entry_text_get(ETK_ENTRY(e->entry[0]));
	dir = ecore_file_get_dir((char*)path);
	file = ecore_file_get_file(path);
	
	if(!dir || !strcmp(dir, ""))
	  dir = strdup("/");
		
	if(!file || !strcmp(file, path))
	  return;	
	
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
	       return;
	     
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
		  etk_entry_text_set(ETK_ENTRY(e->entry[0]), fullpath);
		  break;
	       }	
	  }
	
     }
   
   if(!strcmp(ev->key, "Return") || !strcmp(ev->key, "KP_Enter"))
     {
	_ex_slideshow_stop(e);
        e->cur_tab->dir = strdup((char*)etk_entry_text_get(ETK_ENTRY(e->entry[0])));
        etk_tree_clear(ETK_TREE(e->cur_tab->itree));
        etk_tree_clear(ETK_TREE(e->cur_tab->dtree));
        _ex_main_populate_files(e, NULL);
     }
}

static void
_ex_main_goto_dir_clicked_cb(Etk_Object *object, void *data)
{
   Exhibit *e;

   e = data;

   e->cur_tab->dir = strdup((char*)etk_entry_text_get(ETK_ENTRY(e->entry[0])));
   etk_tree_clear(ETK_TREE(e->cur_tab->itree));
   etk_tree_clear(ETK_TREE(e->cur_tab->dtree));
   _ex_main_populate_files(e, NULL);
}

static Etk_Bool
_ex_main_window_deleted_cb(void *data)
{
   Exhibit *e;

   e = data;
   etk_main_quit();
   return 1;
}

static void
_ex_main_window_key_down_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Event_Key_Up_Down *ev;
   Exhibit *e;

   e = data;
   ev = event;

   if(evas_key_modifier_is_set(ev->modifiers, "Control"))
     {
	if(!strcmp(ev->key, "t"))
	  {
	     Ex_Tab *tab;
	     
	     tab = _ex_tab_new(e, e->cur_tab->cur_path);

	     _ex_main_window_tab_append(e, tab);
	     _ex_main_populate_files(e, NULL);
	  }
	else if(!strcmp(ev->key, "w"))
	  {
	     if(evas_list_count(e->tabs) > 1)
	       {
		  //_ex_main_window_tab_remove(e->cur_tab);
		  //_ex_tab_del(e->cur_tab);
	       }
	  }
	else if(!strcmp(ev->key, "q"))
	  {
	     etk_main_quit();
	  }
	else if(!strcmp(ev->key, "s"))
	  {
	     _ex_main_window_slideshow_toggle(e);
	  }		
	else if(!strcmp(ev->key, "d"))
	  {
	     Etk_Tree_Row *r;
	     char         *icol_string;
	     
	     r = etk_tree_selected_row_get(ETK_TREE(e->cur_tab->itree));
	     if(!r) return;
	     
	     etk_tree_row_fields_get(r, etk_tree_nth_col_get(ETK_TREE(e->cur_tab->itree), 0), NULL, &icol_string, etk_tree_nth_col_get(ETK_TREE(e->cur_tab->itree), 1),NULL);
	     _ex_favorites_add(e, icol_string);
	  }
     }
}

static void
_ex_main_window_resize_cb(Etk_Object *object, void *data)
{

   etk_window_geometry_get(ETK_WINDOW(object), NULL, NULL, 
			   &e->options->last_w, &e->options->last_h);
}

void
_ex_main_window_slideshow_toggle(Exhibit *e)
{
   if(e->slideshow.active)
     {
	_ex_slideshow_stop(e);
     }
   else
     {
	_ex_slideshow_start(e);
     }
}

static void 
_ex_main_window_tab_toggled_cb(Etk_Object *object, void *data)
{     
   Exhibit *e;
   Ex_Tab  *tab;
   
   e = data;
   _ex_slideshow_stop(e);
   tab = evas_list_nth(e->tabs, etk_notebook_current_page_get(ETK_NOTEBOOK(object)));

   e->cur_tab = tab;
   _ex_tab_select(tab);
   etk_entry_text_set(ETK_ENTRY(e->entry[0]), e->cur_tab->cur_path);   
}

void
_ex_main_window_tab_remove(Ex_Tab *tab)
{

}

void
_ex_main_window_tab_append(Exhibit *e, Ex_Tab *tab)
{
   if(evas_list_count(e->tabs) == 1)
     {
	/* adding first "real" tab, copy existing tab, and create new one */
	e->notebook = etk_notebook_new();        
	etk_paned_child2_set(ETK_PANED(e->hpaned), e->notebook, ETK_TRUE);
	
	if(e->cur_tab->fit_window)
	  {
	     if(e->cur_tab->comment.visible)
	       etk_notebook_page_append(ETK_NOTEBOOK(e->notebook), 
					_ex_file_get(e->cur_tab->dir), 
					e->cur_tab->comment.vbox);
	     else
	       etk_notebook_page_append(ETK_NOTEBOOK(e->notebook), 
					_ex_file_get(e->cur_tab->dir), 
					e->cur_tab->alignment);
	  }
	else
	  {
	     if(e->cur_tab->comment.visible)
	       etk_notebook_page_append(ETK_NOTEBOOK(e->notebook),
					_ex_file_get(e->cur_tab->dir),
					e->cur_tab->comment.vbox);
	     else	       
	       etk_notebook_page_append(ETK_NOTEBOOK(e->notebook), 
					_ex_file_get(e->cur_tab->dir), 
					e->cur_tab->scrolled_view);
	  }
	
	etk_signal_connect("current_page_changed", ETK_OBJECT(e->notebook), ETK_CALLBACK(_ex_main_window_tab_toggled_cb), e);
	etk_widget_show(ETK_WIDGET(e->notebook));
     }

   e->tabs = evas_list_append(e->tabs, tab);
   e->cur_tab = tab;
   etk_notebook_page_append(ETK_NOTEBOOK(e->notebook), _ex_file_get(e->cur_tab->dir), e->cur_tab->scrolled_view);
   etk_notebook_current_page_set(ETK_NOTEBOOK(e->notebook), evas_list_count(e->tabs) - 1);
}

static void 
_etk_main_drag_drop_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Event_Selection_Request *ev;
   Etk_Selection_Data_Files *files;
   Exhibit *e;
   
   int i;
   
   ev = event;
   e = data;
   
   if(ev->content != ETK_SELECTION_CONTENT_FILES)
     return;
   
   files = ev->data;
   
   for (i = 0; i < files->num_files; i++)
     {
	char *dir;
	char *file;
	

	if ((file = strstr(files->files[i], "file://")) == NULL)
	  continue;
		
	
	file += strlen("file://");	
	dir = ecore_file_get_dir(file);
	
	if(!ecore_file_is_dir(dir))
	  continue;
	
	if(!ecore_file_exists(file))
	  file = NULL;
	
	E_FREE(e->cur_tab->dir);
	e->cur_tab->dir = strdup(dir);
	etk_tree_clear(ETK_TREE(e->cur_tab->itree));
	etk_tree_clear(ETK_TREE(e->cur_tab->dtree));
	_ex_main_populate_files(e, ecore_file_get_file(file));
	if(ecore_file_exists(file) && !ecore_file_is_dir(file))
	  _ex_main_image_set(e, file);
	etk_notebook_page_tab_label_set(ETK_NOTEBOOK(e->notebook), e->cur_tab->num, _ex_file_get(e->cur_tab->cur_path));
	break;
     }
}


void
_ex_main_window_show(char *dir)
{
   Ex_Tab *tab;
   const char *file;
   char *homedir;
   const char **dnd_types;
   int dnd_types_num;
   
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
   
   file = NULL;
   tab = NULL;
   
   e->win = etk_window_new();
   etk_widget_dnd_dest_set(e->win, ETK_TRUE);
   dnd_types_num = 1;
   dnd_types = calloc(dnd_types_num, sizeof(char*));
   dnd_types[0] = strdup("text/uri-list");
   etk_signal_connect("drag_drop", ETK_OBJECT(e->win), ETK_CALLBACK(_etk_main_drag_drop_cb), e);
   etk_widget_dnd_types_set(e->win, dnd_types, dnd_types_num);   
   etk_window_title_set(ETK_WINDOW(e->win), WINDOW_TITLE " - Image Viewing the Kewl Way!");
   etk_window_wmclass_set(ETK_WINDOW(e->win), "exhibit", "Exhibit");
   etk_window_resize(ETK_WINDOW(e->win), e->options->last_w, e->options->last_h);
   etk_signal_connect("delete_event", ETK_OBJECT(e->win), ETK_CALLBACK(_ex_main_window_deleted_cb), e);
   etk_signal_connect("key_down", ETK_OBJECT(e->win), ETK_CALLBACK(_ex_main_window_key_down_cb), e);
   etk_signal_connect("resize", ETK_OBJECT(e->win), ETK_CALLBACK(_ex_main_window_resize_cb), e);

   e->vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(e->win), e->vbox);
               
   e->hpaned = etk_hpaned_new();
   //etk_container_add(ETK_CONTAINER(e->win), e->hpaned);
   etk_box_pack_start(ETK_BOX(e->vbox), e->hpaned, ETK_TRUE, ETK_TRUE, 0);   

   e->vpaned = etk_vpaned_new();
   etk_paned_child1_set(ETK_PANED(e->hpaned), e->vpaned, ETK_FALSE);
   
   e->table = etk_table_new(4, 4, ETK_FALSE);
   etk_paned_child1_set(ETK_PANED(e->vpaned), e->table, ETK_TRUE);

   e->menu_bar = etk_menu_bar_new();
   etk_table_attach(ETK_TABLE(e->table), e->menu_bar, 0, 4, 0, 0, 0, 0, ETK_FILL_POLICY_HFILL | ETK_FILL_POLICY_VFILL | ETK_FILL_POLICY_HEXPAND);
   
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
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("New window"), ETK_STOCK_WINDOW_NEW, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_new_window_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Save image"), ETK_STOCK_DOCUMENT_SAVE, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_save_image_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Save image as"), ETK_STOCK_DOCUMENT_SAVE, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_save_image_as_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_SEPERATOR, NULL, ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), NULL, NULL);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Search"), ETK_STOCK_SYSTEM_SEARCH, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_search_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_SEPERATOR, NULL, ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), NULL, NULL);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Rename"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_rename_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Delete"), ETK_STOCK_X_DIRECTORY_TRASH, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_delete_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_SEPERATOR, NULL, ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), NULL, NULL);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Options"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_options_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_SEPERATOR, NULL, ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), NULL, NULL);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Close window"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_close_window_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Quit"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_quit_cb), e);
	
	/* Create "Edit" menu item */
	menu_item = _ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Edit"), -99, ETK_MENU_SHELL(e->menu_bar), NULL, NULL);
	/* Create submenu for "Edit" menu */
	menu = etk_menu_new();
	
	/* Set the File submenu to the File menu item and populate it */	
	etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));       	
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Undo"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_undo_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_SEPERATOR, NULL, ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), NULL, NULL);
	
	/* Run in submenu */
	menu_item = _ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Run in ..."), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_run_in_cb), e);
	submenu = etk_menu_new();
	etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(submenu));
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("The Gimp"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(submenu), ETK_CALLBACK(_ex_menu_run_in_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("XV"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(submenu), ETK_CALLBACK(_ex_menu_run_in_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Xpaint"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(submenu), ETK_CALLBACK(_ex_menu_run_in_cb), e);
	
	/* Continue "Edit" menu */
	_ex_menu_item_new(EX_MENU_ITEM_SEPERATOR, NULL, ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), NULL, NULL);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Rotate clockwise"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_rot_clockwise_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Rotate counterclockwise"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_rot_counter_clockwise_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Flip horizontally"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_flip_horizontal_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Flip vertically"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_flip_vertical_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_SEPERATOR, NULL, ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), NULL, NULL);
	
	menu_item = _ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Effects"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_run_in_cb), e);
	submenu = etk_menu_new();
	etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(submenu));
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Blur"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(submenu), ETK_CALLBACK(_ex_menu_blur_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Sharpen"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(submenu), ETK_CALLBACK(_ex_menu_sharpen_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Brighten"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(submenu), ETK_CALLBACK(_ex_menu_brighten_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_SEPERATOR, NULL, ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), NULL, NULL);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Set as wallpaper"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_set_wallpaper_cb), e);
	
	/* Create the "View" menu item */
	menu_item = _ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("View"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(e->menu_bar), NULL, NULL);
	menu = etk_menu_new();
	etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));
	
	menu_item = _ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Sort"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_run_in_cb), e);
	submenu = etk_menu_new();
	etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(submenu));
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Date"), ETK_STOCK_OFFICE_CALENDAR, ETK_MENU_SHELL(submenu), ETK_CALLBACK(_ex_sort_date_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Size"), ETK_STOCK_DRIVE_HARDDISK, ETK_MENU_SHELL(submenu), ETK_CALLBACK(_ex_sort_size_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Name"), ETK_STOCK_TEXT_X_GENERIC, ETK_MENU_SHELL(submenu), ETK_CALLBACK(_ex_sort_name_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Resolution"), ETK_STOCK_UTILITIES_SYSTEM_MONITOR, ETK_MENU_SHELL(submenu), ETK_CALLBACK(_ex_sort_resol_cb), e);
		
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Zoom in"), EX_IMAGE_ZOOM_IN, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_zoom_in_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Zoom out"), EX_IMAGE_ZOOM_OUT, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_zoom_out_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Zoom 1:1"), EX_IMAGE_ONE_TO_ONE, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_zoom_one_to_one_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Fit to window"), EX_IMAGE_FIT_TO_WINDOW, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_fit_to_window_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_SEPERATOR, NULL, ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), NULL, NULL);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Toggle slideshow"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_toggle_slideshow_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Refresh"), ETK_STOCK_VIEW_REFRESH, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_refresh_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Toggle Comments"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_comments_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_SEPERATOR, NULL, ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), NULL, NULL);		
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Add to favorites"), ETK_STOCK_EMBLEM_PHOTOS, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_add_to_fav_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("View favorites"), ETK_STOCK_EMBLEM_FAVORITE, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_go_to_fav_cb), e);
	
	/* Create the "Help" submenu */
	menu_item = _ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Help"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(e->menu_bar), NULL, NULL);
	menu = etk_menu_new();
	etk_menu_item_submenu_set(ETK_MENU_ITEM(menu_item), ETK_MENU(menu));
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Release notes"), ETK_STOCK_TEXT_X_GENERIC, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_release_notes_cb), e);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("About"), ETK_STOCK_HELP_BROWSER, ETK_MENU_SHELL(menu), ETK_CALLBACK(_ex_menu_about_cb), e);
     }   
   
   e->zoom_in[0] = etk_button_new();
   e->zoom_in[1] = etk_image_new_from_edje(PACKAGE_DATA_DIR"/images/images.edj", "zoom_in");
   etk_button_image_set(ETK_BUTTON(e->zoom_in[0]), ETK_IMAGE(e->zoom_in[1]));
   etk_signal_connect("clicked", ETK_OBJECT(e->zoom_in[0]), ETK_CALLBACK(_ex_main_button_zoom_in_cb), e);
   etk_table_attach(ETK_TABLE(e->table), e->zoom_in[0],
		    0, 0, 1, 1,
		    0, 0, ETK_FILL_POLICY_NONE);

   e->zoom_out[0] = etk_button_new();
   e->zoom_out[1] = etk_image_new_from_edje(PACKAGE_DATA_DIR"/images/images.edj", "zoom_out");
   etk_button_image_set(ETK_BUTTON(e->zoom_out[0]), ETK_IMAGE(e->zoom_out[1]));
   etk_signal_connect("clicked", ETK_OBJECT(e->zoom_out[0]), ETK_CALLBACK(_ex_main_button_zoom_out_cb), e);   
   etk_table_attach(ETK_TABLE(e->table), e->zoom_out[0],
		    1, 1, 1, 1,
		    0, 0, ETK_FILL_POLICY_NONE);

   e->fit[0] = etk_button_new();
   e->fit[1] = etk_image_new_from_edje(PACKAGE_DATA_DIR"/images/images.edj", "fit_to_window");
   etk_button_image_set(ETK_BUTTON(e->fit[0]), ETK_IMAGE(e->fit[1]));
   etk_signal_connect("clicked", ETK_OBJECT(e->fit[0]), ETK_CALLBACK(_ex_main_button_fit_to_window_cb), e);   
   etk_table_attach(ETK_TABLE(e->table), e->fit[0],
		    2, 2, 1, 1,
		    0, 0, ETK_FILL_POLICY_NONE);

   e->original[0] = etk_button_new();
   e->original[1] = etk_image_new_from_edje(PACKAGE_DATA_DIR"/images/images.edj", "one_to_one");
   etk_button_image_set(ETK_BUTTON(e->original[0]), ETK_IMAGE(e->original[1]));
   etk_signal_connect("clicked", ETK_OBJECT(e->original[0]), ETK_CALLBACK(_ex_main_button_zoom_one_to_one_cb), e);
   etk_table_attach(ETK_TABLE(e->table), e->original[0],
		    3, 3, 1, 1,
		    0, 0, ETK_FILL_POLICY_NONE);

   e->entry[0] = etk_entry_new();
   etk_table_attach(ETK_TABLE(e->table), e->entry[0], 0, 2, 2, 2, 0, 0, ETK_FILL_POLICY_HEXPAND | ETK_FILL_POLICY_HFILL);
   etk_signal_connect("key_down", ETK_OBJECT(e->entry[0]), ETK_CALLBACK(_ex_main_entry_dir_key_down_cb), e);

   e->entry[1] = etk_button_new_with_label("Go");
   etk_table_attach(ETK_TABLE(e->table), e->entry[1],
		    3, 3, 2, 2,
		    0, 0, ETK_FILL_POLICY_NONE);
   etk_signal_connect("clicked", ETK_OBJECT(e->entry[1]), ETK_CALLBACK(_ex_main_goto_dir_clicked_cb), e);
   
   /* create first tab but dont place it in notebook */
   if(dir)
     {
	if(ecore_file_is_dir(dir))	  	     
	  tab = _ex_tab_new(e, dir);   
	else if(ecore_file_exists(dir))
	  {	     
	     char *dir2;
	     
	     dir2 = ecore_file_get_dir(dir);
	     tab = _ex_tab_new(e, dir2);
	     E_FREE(dir2);
	     file = ecore_file_get_file(dir);
	  }
	else     
	  tab = _ex_tab_new(e, ".");
     }
   else
     tab = _ex_tab_new(e, ".");
   
	
   e->cur_tab = tab;   
   e->tabs = evas_list_append(e->tabs, tab);   
   _ex_tab_select(tab);
   etk_paned_child2_set(ETK_PANED(tab->e->hpaned), tab->scrolled_view, ETK_TRUE);
   
   _ex_main_populate_files(e, file);
      
   e->hbox = etk_hbox_new(ETK_TRUE, 0);   
   etk_box_pack_end(ETK_BOX(e->vbox), e->hbox, ETK_FALSE, ETK_FALSE, 0);

   e->sort = etk_label_new("Sort by date");
   /* size is different than statusbar, how do we make it look good? */
   //etk_box_pack_start(ETK_BOX(e->hbox), e->sort, ETK_TRUE, ETK_TRUE, 0);
     {
	Etk_Widget *menu;
	Etk_Widget *menu_item;

	menu = etk_menu_new();
	etk_signal_connect("mouse_down", ETK_OBJECT(e->sort), ETK_CALLBACK(_ex_sort_label_mouse_down_cb), menu);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Sort by name"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), NULL, NULL);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Sort by date"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), NULL, NULL);
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Sort by size"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), NULL, NULL);
	_ex_menu_item_new(EX_MENU_ITEM_SEPERATOR, NULL, ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), NULL, NULL);	
	_ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Ascending"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), NULL, NULL);
	menu_item = _ex_menu_item_new(EX_MENU_ITEM_NORMAL, _("Sort"), ETK_STOCK_NO_STOCK, ETK_MENU_SHELL(menu), NULL, NULL);
     }
   
   e->statusbar[0] = etk_statusbar_new();
   etk_statusbar_has_resize_grip_set(ETK_STATUSBAR(e->statusbar[0]), ETK_FALSE);
   etk_box_pack_start(ETK_BOX(e->hbox), e->statusbar[0], ETK_TRUE, ETK_TRUE, 0);
   
   e->statusbar[1] = etk_statusbar_new();
   etk_statusbar_has_resize_grip_set(ETK_STATUSBAR(e->statusbar[1]), ETK_FALSE);
   etk_box_pack_start(ETK_BOX(e->hbox), e->statusbar[1], ETK_TRUE, ETK_TRUE, 0);

   e->statusbar[2] = etk_statusbar_new();
   etk_statusbar_has_resize_grip_set(ETK_STATUSBAR(e->statusbar[2]), ETK_FALSE);   
   etk_box_pack_start(ETK_BOX(e->hbox), e->statusbar[2], ETK_TRUE, ETK_TRUE, 0);   
   etk_statusbar_push(ETK_STATUSBAR(e->statusbar[2]), "1:1", 0);
   
   e->statusbar[3] = etk_statusbar_new();
   etk_box_pack_start(ETK_BOX(e->hbox), e->statusbar[3], ETK_FALSE, ETK_FALSE, 0);
   
   etk_widget_show_all(e->win);
}

int
main(int argc, char *argv[])
{
   if (!etk_init(&argc, &argv))
     {
	fprintf(stderr, "Could not init etk. Exiting...\n");
	return 0;
     };
   
   ecore_file_init();
   if(!_ex_options_init())
     fprintf(stderr, "WARNING: Exhibit could not set up its options files!\n"
	             "         You will not be able to save your preferences.\n");
   event_handlers = evas_list_append(event_handlers,
				     ecore_event_handler_add(ECORE_EXE_EVENT_DEL,
							     _ex_thumb_exe_exit,
							     NULL));
   
   epsilon_init();
   if(argc > 1)
     _ex_main_window_show(argv[1]);     
   else
     _ex_main_window_show(NULL);   
     
   etk_main();
   _ex_options_save(e);
   _ex_options_shutdown();
   ecore_file_shutdown();
   etk_shutdown();

   return 0;
}
