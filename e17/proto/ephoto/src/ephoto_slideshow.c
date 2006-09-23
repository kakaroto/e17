#include "ephoto.h"
Ecore_Timer *timer;

typedef struct _Slide_Config Slide_Config;
Slide_Config *parse_slideshow_config(void);

struct _Slide_Config
{
 Ewl_Widget *win;
 Ewl_Widget *fullscreen;
 Ewl_Widget *custom;
 Ewl_Widget *spinner;
 Ewl_Widget *show_name;
 Ewl_Widget *random;
 Ewl_Widget *loop;
 Ewl_Widget *wentry;
 Ewl_Widget *hentry;
 Ewl_Widget *zoom_image;
 Ewl_Widget *aspect_image;
 Ewl_Widget *image;
 int full_size;
 int custom_size;
 int length;
 int name_show;
 int zoom;
 int keep_aspect;
 int random_order;
 int loop_slide;
 int w_size;
 int h_size;
};

void destroy_slideshow(Ewl_Widget *w, void *event, void *data)
{
 ecore_timer_del(timer);
 ewl_widget_destroy(w);
}

int change_picture(void *data)
{
 char *image_path;
 int total;
 int temp;
 Slide_Config *sc;
 
 sc = data;
 if (sc->random_order)
 {
  total = ecore_dlist_nodes(current_thumbs);
  temp = rand()%total;
  ecore_dlist_goto_index(current_thumbs, temp);
 }
 else
 {
  ecore_dlist_next(current_thumbs);
 }
 image_path = ecore_dlist_current(current_thumbs);
 if(image_path)
 {
  ewl_image_file_set(EWL_IMAGE(sc->image), image_path, NULL);
 }
 else
 {
  if (sc->loop_slide)
  {
   ecore_list_goto_first(current_thumbs);
   image_path = ecore_dlist_current(current_thumbs);
   if(image_path)
   {
    ewl_image_file_set(EWL_IMAGE(sc->image), image_path, NULL);
   }
   else
   {
    ecore_timer_del(time);
    ewl_widget_destroy(sc->image->parent->parent);
   }
  }
  else 
  {
   ecore_timer_del(timer);
   ewl_widget_destroy(sc->image->parent->parent);
  }
 }
}
 
void start_slideshow(Ewl_Widget *w, void *event, void *data)
{
 Ewl_Widget *window;
 Ewl_Widget *cell;
 Ewl_Widget *image;
 char *image_path;
 Slide_Config *sc;
 int ew, eh;
 
 sc = parse_slideshow_config();
 image_path = ecore_dlist_goto_first(current_thumbs);
 
 if (!image_path) return;
 
 srand((unsigned int)time((time_t *)NULL));
 
 window = ewl_window_new();
 ewl_window_title_set(EWL_WINDOW(window), "Ephoto Slideshow");
 ewl_window_name_set(EWL_WINDOW(window), "Ephoto Slideshow");
 if (sc->full_size) 
	ewl_window_fullscreen_set(EWL_WINDOW(window), 1);
 if (sc->custom_size)
 	ewl_object_maximum_size_set(EWL_OBJECT(window), sc->w_size, sc->h_size);
	ewl_object_minimum_size_set(EWL_OBJECT(window), sc->w_size, sc->h_size);
 ewl_callback_append(window, EWL_CALLBACK_DELETE_WINDOW, destroy_slideshow, NULL);
 ewl_callback_append(window, EWL_CALLBACK_CLICKED, destroy_slideshow, NULL); 
 ewl_window_dialog_set(EWL_WINDOW(window), 1);
 ewl_widget_show(window);

 cell = ewl_cell_new();
 ewl_object_fill_policy_set(EWL_OBJECT(cell), EWL_FLAG_FILL_ALL);
 ewl_container_child_append(EWL_CONTAINER(window), cell);
 ewl_widget_show(cell);

 ewl_object_current_size_get(EWL_OBJECT(window), &ew, &eh);
 
 image = ewl_image_new();
 ewl_image_file_set(EWL_IMAGE(image), image_path, NULL);
 if (sc->zoom)
 	ewl_object_fill_policy_set(EWL_OBJECT(image), EWL_FLAG_FILL_ALL);
 else
	ewl_object_fill_policy_set(EWL_OBJECT(image), EWL_FLAG_FILL_SHRINK);
 if (sc->keep_aspect)
	ewl_image_proportional_set(EWL_IMAGE(image), TRUE);
 ewl_image_size_set(EWL_IMAGE(image), ew, eh);
 ewl_object_alignment_set(EWL_OBJECT(image), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(cell), image);
 ewl_widget_show(image);

 sc->image = image;
 timer = ecore_timer_add(sc->length, change_picture, sc);
}

void initial_config()
{
 FILE *file;
 char temp[PATH_MAX];
 char path[PATH_MAX];

 snprintf(temp, PATH_MAX, "%s/.ephoto", getenv("HOME"));
 snprintf(path, PATH_MAX, "%s/slideshow_config", temp);
 if (!ecore_file_exists(temp)) ecore_file_mkdir(temp);

 file = fopen(path, "w");
 if (file != NULL)
 {
  fprintf(file, "Fullscreen=1\n");
  fprintf(file, "Custom=0\n");
  fprintf(file, "Width=640\n");
  fprintf(file, "Height=480\n");
  fprintf(file, "Random=0\n");
  fprintf(file, "Loop=0\n");
  fprintf(file, "Zoom=1\n");
  fprintf(file, "Aspect=0\n");
  fprintf(file, "Length=3\n");
  fprintf(file, "FileName=0\n");
  fclose(file);
 }
}

Slide_Config *parse_slideshow_config()
{
 char path[PATH_MAX];
 FILE *file;
 char text[PATH_MAX];
 char *temp;
 Slide_Config *sc;
 
 sc = calloc(1, sizeof(Slide_Config));
 snprintf(path, PATH_MAX, "%s/.ephoto/slideshow_config", getenv("HOME"));

 if (!ecore_file_exists(path)) initial_config();
 
 file = fopen(path, "r");

 if (file != NULL)
 {
  while(fgets(text,PATH_MAX,file)!=NULL)
  {
   if(!strncmp(text, "Fullscreen", 10))
   {
    temp = strrchr(text, '=')+1;
    sc->full_size = atoi(temp);
   }
   if(!strncmp(text, "Custom", 6))
   {
    temp = strrchr(text, '=')+1;
    sc->custom_size = atoi(temp);
   }
   if(!strncmp(text, "Width", 5))
   {
    temp = strrchr(text, '=')+1;
    sc->w_size = atoi(temp);
   }
   if(!strncmp(text, "Height", 6))
   {
    temp = strrchr(text, '=')+1;
    sc->h_size = atoi(temp);
   }
   if(!strncmp(text, "Random", 6))
   {
    temp = strrchr(text, '=')+1;
    sc->random_order = atoi(temp);
   }
   if(!strncmp(text, "Loop", 4))
   {
    temp = strrchr(text, '=')+1;
    sc->loop_slide = atoi(temp);
   }
   if(!strncmp(text, "Zoom", 4))
   {
    temp = strrchr(text, '=')+1;
    sc->zoom = atoi(temp);
   }
   if(!strncmp(text, "Aspect", 6))
   {
    temp = strrchr(text, '=')+1;
    sc->keep_aspect = atoi(temp);
   }
   if(!strncmp(text, "Length", 6))
   {
    temp = strrchr(text, '=')+1;
    sc->length = atoi(temp);
   }
   if(!strncmp(text, "FileName", 8))
   {
    temp = strrchr(text, '=')+1;
    sc->name_show = atoi(temp);
   }
  }
  fclose(file);
 }
 return(sc);
}
	
void config_cancel(Ewl_Widget *w, void *event, void *data)
{
 Ewl_Widget *win;
 win = data;
 ewl_widget_destroy(win);
}

void save_config(Ewl_Widget *w, void *event, void *data)
{
 FILE *file;
 Slide_Config *sc;
 char temp[PATH_MAX];
 char path[PATH_MAX];

 snprintf(temp, PATH_MAX, "%s/.ephoto", getenv("HOME"));
 snprintf(path, PATH_MAX, "%s/slideshow_config", temp);
 if (!ecore_file_exists(temp)) ecore_file_mkdir(temp);
 
 sc = data;
 
 file = fopen(path, "w");
 if (file != NULL)
 {
  fprintf(file, "Fullscreen=%d\n", 
	  ewl_checkbutton_is_checked(EWL_CHECKBUTTON(sc->fullscreen)));
  fprintf(file, "Custom=%d\n",
	  ewl_checkbutton_is_checked(EWL_CHECKBUTTON(sc->custom)));
  fprintf(file, "Width=%s\n", ewl_text_text_get(EWL_TEXT(sc->wentry)));
  fprintf(file, "Height=%s\n", ewl_text_text_get(EWL_TEXT(sc->hentry)));
  fprintf(file, "Random=%d\n", 
	  ewl_checkbutton_is_checked(EWL_CHECKBUTTON(sc->random)));
  fprintf(file, "Loop=%d\n", 
	  ewl_checkbutton_is_checked(EWL_CHECKBUTTON(sc->loop)));
  fprintf(file, "Zoom=%d\n", 
	  ewl_checkbutton_is_checked(EWL_CHECKBUTTON(sc->zoom_image)));
  fprintf(file, "Aspect=%d\n",
	  ewl_checkbutton_is_checked(EWL_CHECKBUTTON(sc->aspect_image)));
  fprintf(file, "Length=%d\n", (int)ewl_range_value_get(EWL_RANGE(sc->spinner)));
  fprintf(file, "FileName=%d\n", 
	  ewl_checkbutton_is_checked(EWL_CHECKBUTTON(sc->show_name)));
  fclose(file);
 }
 ewl_widget_destroy(sc->win);
}
 
void create_slideshow_config(Ewl_Widget *w, void *event, void *data)
{
 char temp[PATH_MAX];
 Ewl_Widget *hbox, *main_hbox, *vbox, *border, *text, *save, *cancel;
 Slide_Config *sc;
 
 snprintf(temp, PATH_MAX, "%s/.ephoto/slideshow_config", getenv("HOME"));

 if (!ecore_file_exists(temp)) initial_config(NULL, NULL, NULL);
 sc = parse_slideshow_config();
 
 sc->win = ewl_window_new();
 ewl_window_title_set(EWL_WINDOW(sc->win), "Slideshow Configuration");
 ewl_window_name_set(EWL_WINDOW(sc->win), "Slideshow Configuration");
 ewl_window_dialog_set(EWL_WINDOW(sc->win), 1);
 ewl_object_size_request(EWL_OBJECT(sc->win), 400, 200);
 ewl_callback_append(sc->win, EWL_CALLBACK_DELETE_WINDOW, config_cancel, sc->win);
 ewl_widget_show(sc->win);

 vbox = ewl_vbox_new();
 ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);
 ewl_container_child_append(EWL_CONTAINER(sc->win), vbox);
 ewl_widget_show(vbox);

 main_hbox = ewl_hbox_new();
 ewl_object_fill_policy_set(EWL_OBJECT(main_hbox), EWL_FLAG_FILL_ALL);
 ewl_container_child_append(EWL_CONTAINER(vbox), main_hbox);
 ewl_object_alignment_set(EWL_OBJECT(main_hbox), EWL_FLAG_ALIGN_CENTER);
 ewl_widget_show(main_hbox);
      
 border = ewl_border_new();
 ewl_border_text_set(EWL_BORDER(border), "Window Size");
 ewl_border_label_alignment_set(EWL_BORDER(border), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(main_hbox), border);
 ewl_object_alignment_set(EWL_OBJECT(border), EWL_FLAG_ALIGN_CENTER);
 ewl_widget_show(border);

 hbox = ewl_hbox_new();
 ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_HFILL);
 ewl_container_child_append(EWL_CONTAINER(border), hbox);
 ewl_object_alignment_set(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_CENTER);
 ewl_widget_show(hbox);

 sc->fullscreen = ewl_radiobutton_new();
 ewl_button_label_set(EWL_BUTTON(sc->fullscreen), "Fullscreen");
 ewl_container_child_append(EWL_CONTAINER(hbox), sc->fullscreen);
 ewl_radiobutton_checked_set(EWL_RADIOBUTTON(sc->fullscreen), sc->full_size);
 ewl_object_alignment_set(EWL_OBJECT(sc->fullscreen), EWL_FLAG_ALIGN_LEFT);
 ewl_widget_show(sc->fullscreen);	
 
 sc->custom = ewl_radiobutton_new();
 ewl_button_label_set(EWL_BUTTON(sc->custom), "Custom");
 ewl_container_child_append(EWL_CONTAINER(hbox), sc->custom);
 ewl_radiobutton_checked_set(EWL_RADIOBUTTON(sc->custom), sc->custom_size);
 ewl_radiobutton_chain_set(EWL_RADIOBUTTON(sc->fullscreen), 
		 	   EWL_RADIOBUTTON(sc->custom));
 ewl_object_alignment_set(EWL_OBJECT(sc->custom), EWL_FLAG_ALIGN_RIGHT);
 ewl_widget_show(sc->custom);

 hbox = ewl_hbox_new();
 ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_HFILL);
 ewl_container_child_append(EWL_CONTAINER(border), hbox);
 ewl_widget_show(hbox);
 
 text = ewl_text_new();
 ewl_text_text_set(EWL_TEXT(text), "Width");
 ewl_object_fill_policy_set(EWL_OBJECT(text), EWL_FLAG_FILL_SHRINK);
 ewl_object_alignment_set(EWL_OBJECT(text), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(hbox), text);
 ewl_widget_show(text);

 sc->wentry = ewl_entry_new();
 ewl_text_text_set(EWL_TEXT(sc->wentry), "640");
 ewl_container_child_append(EWL_CONTAINER(hbox), sc->wentry);
 ewl_object_size_request(EWL_OBJECT(sc->wentry), 35, 15);
 ewl_widget_disable(sc->wentry);
 ewl_widget_show(sc->wentry);

 text = ewl_text_new();
 ewl_text_text_set(EWL_TEXT(text), "Height");
 ewl_object_fill_policy_set(EWL_OBJECT(text), EWL_FLAG_FILL_SHRINK);
 ewl_object_alignment_set(EWL_OBJECT(text), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(hbox), text);
 ewl_widget_show(text);

 sc->hentry = ewl_entry_new();
 ewl_text_text_set(EWL_TEXT(sc->hentry), "480");
 ewl_container_child_append(EWL_CONTAINER(hbox), sc->hentry);
 ewl_object_size_request(EWL_OBJECT(sc->hentry), 35, 15);
 ewl_widget_disable(sc->hentry);
 ewl_widget_show(sc->hentry);

 border = ewl_border_new();
 ewl_border_text_set(EWL_BORDER(border), "Order");
 ewl_border_label_alignment_set(EWL_BORDER(border), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(main_hbox), border);
 ewl_object_alignment_set(EWL_OBJECT(border), EWL_FLAG_ALIGN_CENTER);
 ewl_widget_show(border);

 sc->loop = ewl_checkbutton_new();
 ewl_button_label_set(EWL_BUTTON(sc->loop), "Loop Slideshow");
 ewl_checkbutton_checked_set(EWL_CHECKBUTTON(sc->loop), sc->loop_slide);
 ewl_container_child_append(EWL_CONTAINER(border), sc->loop);
 ewl_object_alignment_set(EWL_OBJECT(sc->loop), EWL_FLAG_ALIGN_CENTER);
 ewl_widget_show(sc->loop);

 sc->random = ewl_checkbutton_new();
 ewl_button_label_set(EWL_BUTTON(sc->random), "Random Order");
 ewl_checkbutton_checked_set(EWL_CHECKBUTTON(sc->random), sc->random_order);
 ewl_container_child_append(EWL_CONTAINER(border), sc->random);
 ewl_object_alignment_set(EWL_OBJECT(sc->random), EWL_FLAG_ALIGN_CENTER);
 ewl_widget_show(sc->random);

 hbox = ewl_hbox_new();
 ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_ALL);
 ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
 ewl_object_alignment_set(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_CENTER);
 ewl_widget_show(hbox);
 
 border = ewl_border_new();
 ewl_border_text_set(EWL_BORDER(border), "Image Size");
 ewl_border_label_alignment_set(EWL_BORDER(border), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(hbox), border);
 ewl_object_alignment_set(EWL_OBJECT(border), EWL_FLAG_ALIGN_CENTER);
 ewl_widget_show(border); 
 
 sc->zoom_image = ewl_checkbutton_new();
 ewl_button_label_set(EWL_BUTTON(sc->zoom_image), "Zoom Images to Fill Window");
 ewl_checkbutton_checked_set(EWL_CHECKBUTTON(sc->zoom_image), sc->zoom);
 ewl_container_child_append(EWL_CONTAINER(border), sc->zoom_image);
 ewl_object_alignment_set(EWL_OBJECT(sc->zoom_image), EWL_FLAG_ALIGN_CENTER);
 ewl_widget_show(sc->zoom_image);     
 
 sc->aspect_image = ewl_checkbutton_new();
 ewl_button_label_set(EWL_BUTTON(sc->aspect_image), "Keep Aspect");
 ewl_checkbutton_checked_set(EWL_CHECKBUTTON(sc->aspect_image), sc->keep_aspect);
 ewl_container_child_append(EWL_CONTAINER(border), sc->aspect_image);
 ewl_object_alignment_set(EWL_OBJECT(sc->aspect_image), EWL_FLAG_ALIGN_CENTER);
 ewl_widget_show(sc->aspect_image);  

 border = ewl_border_new();
 ewl_border_text_set(EWL_BORDER(border), "Transitions");
 ewl_border_label_alignment_set(EWL_BORDER(border), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(hbox), border);
 ewl_object_alignment_set(EWL_OBJECT(border), EWL_FLAG_ALIGN_CENTER);
 ewl_widget_show(border); 
 
 sc->spinner = ewl_spinner_new();
 ewl_spinner_digits_set(EWL_SPINNER(sc->spinner), 0);
 ewl_range_minimum_value_set(EWL_RANGE(sc->spinner), 1);
 ewl_range_maximum_value_set(EWL_RANGE(sc->spinner), 1000);
 ewl_range_value_set(EWL_RANGE(sc->spinner), sc->length);
 ewl_range_step_set(EWL_RANGE(sc->spinner), 1);
 ewl_container_child_append(EWL_CONTAINER(border), sc->spinner);
 ewl_object_alignment_set(EWL_OBJECT(sc->spinner), EWL_FLAG_ALIGN_CENTER);
 ewl_object_maximum_size_set(EWL_OBJECT(sc->spinner), 70, 25);
 ewl_widget_show(sc->spinner);
 
 sc->show_name = ewl_checkbutton_new();
 ewl_button_label_set(EWL_BUTTON(sc->show_name), "Show File Name On Change");
 ewl_checkbutton_checked_set(EWL_CHECKBUTTON(sc->show_name), sc->name_show);
 ewl_container_child_append(EWL_CONTAINER(border), sc->show_name);
 ewl_object_alignment_set(EWL_OBJECT(sc->show_name), EWL_FLAG_ALIGN_CENTER);
 ewl_widget_show(sc->show_name);
      
 hbox = ewl_hbox_new();
 ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_SHRINK);
 ewl_object_alignment_set(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_CENTER);
 ewl_container_child_append(EWL_CONTAINER(vbox), hbox);
 ewl_widget_show(hbox);
 
 save = ewl_button_new();
 ewl_button_stock_type_set(EWL_BUTTON(save), EWL_STOCK_SAVE);
 ewl_container_child_append(EWL_CONTAINER(hbox), save);
 ewl_object_fill_policy_set(EWL_OBJECT(save), EWL_FLAG_FILL_SHRINK);
 ewl_object_alignment_set(EWL_OBJECT(save), EWL_FLAG_ALIGN_CENTER);
 ewl_callback_append(save, EWL_CALLBACK_CLICKED, save_config, sc);
 ewl_widget_show(save);

 cancel = ewl_button_new();
 ewl_button_stock_type_set(EWL_BUTTON(cancel), EWL_STOCK_CANCEL);
 ewl_container_child_append(EWL_CONTAINER(hbox), cancel);
 ewl_object_fill_policy_set(EWL_OBJECT(cancel), EWL_FLAG_FILL_SHRINK);
 ewl_object_alignment_set(EWL_OBJECT(cancel), EWL_FLAG_ALIGN_CENTER);
 ewl_callback_append(cancel, EWL_CALLBACK_CLICKED, config_cancel, sc->win);
 ewl_widget_show(cancel);   
}
