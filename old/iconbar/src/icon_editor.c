#include "icon_editor.h"


void _icon_editor_gui_init();
void _icon_editor_gui_update();

static void _editor_close_win (Ewl_Widget *widget, void *ev_data, void *user_data);
static void _editor_realize (Ewl_Widget *widget, void *ev_data, void *user_data);
static void _editor_button_cb (Ewl_Widget *widget, void *ev_data, void *user_data);
static void _editor_icon_image_cb (Ewl_Widget *widget, void *ev_data, void *user_data);


Icon_Editor *editor;

int
icon_editor_init(int *argc, char **argv)
{
  ewl_init(argc, argv);
  editor = calloc(1, sizeof(Icon_Editor));

  _icon_editor_gui_init();
  _icon_editor_gui_update();
  return 1;
}

void
icon_editor_shutdown()
{
  /* FIXME: how do we clean up the ewl widgets? */
  ewl_shutdown();
}

void
icon_editor_show()
{
  if (!editor) return;

  ewl_widget_show(editor->win);
}

void
icon_editor_hide()
{
  if (!editor) return;

  ewl_widget_hide(editor->win);
  ewl_widget_hide(editor->filesel.win);
}


static void
_editor_close_win (Ewl_Widget *widget, void *ev_data, void *user_data)
{
}

static void
_editor_realize (Ewl_Widget *widget, void *ev_data, void *user_data)
{
  _icon_editor_gui_update();
}

static void
_editor_icon_image_cb (Ewl_Widget *widget, void *ev_data, void *user_data)
{
  if (!editor || !editor->icon_image) return;
  /* FIXME: use a file dialog */
  //ewl_image_set_file(EWL_IMAGE(editor->icon_image), "test.png");
  ewl_widget_show(editor->filesel.win);
}

static void
_editor_button_cb (Ewl_Widget *widget, void *ev_data, void *user_data)
{
  if (!editor) return;
  
  if (widget == editor->ok_but)
  {
    icon_editor_file_save();
    /* FIXME do this somewhere else? */
    /* make sure it reloads even if the file is the same */
    edje_object_file_set(editor->icon->image, "", "icon");
    edje_object_file_set(editor->icon->image, editor->file, "icon");
    iconbar_fix(editor->icon->iconbar->obj);
  }

  icon_editor_hide();
}


static void
_editor_filesel_cb (Ewl_Widget *widget, void *ev_data, void *user_data)
{
  char *file = ev_data;

  if (file)
  {
    icon_editor_image_file_set(file);
  }
  ewl_widget_hide(editor->filesel.win);
}

/* icon edit functions */
int
icon_editor_icon_set(Icon *icon)
{
  if (!editor) return 0;

  printf("icon set!\n");
  editor->icon = icon;
  icon_editor_file_set(editor->icon->file);
  return 1;
}

int
icon_editor_file_set(char *file)
{
  Evas_List *l;
 
  if (!editor) return 0;

  if (editor->edf) free(editor->edf);
  editor->edf = edje_edit_file_load(file);
  if (!editor->edf)
  {
    printf("can't load file\n");
    return 0;
  }
  
  editor->group = edje_edit_file_part_collection_get(editor->edf, "icon");
  if (!editor->group) 
  {
    printf("can't find icon in file\n");
    return 0;
  }

  for (l = editor->group->programs; l; l = l->next)
  {
    Edje_Program *prog = l->data;

    if (!strcmp(prog->name, "exec"))
    {
      editor->exec_prog = prog;
      break;
    }
  }
  if (!editor->exec_prog)
  {
    printf("can't find exec prog\n");
    return 0;
  }
 
  if (editor->file) free(editor->file);
  editor->file = strdup(file);

  _icon_editor_gui_update();
  
  return 1;
}

void
icon_editor_file_save()
{
  if (!editor || !editor->edf) return;

  icon_editor_exec_set(ewl_entry_text_get(EWL_ENTRY(editor->exec.entry)));

  if (editor->icon_image)
  {
    int w, h;
    Evas_Object *im;

    im = EWL_IMAGE(editor->icon_image)->image;
    evas_object_image_size_get(im, &w, &h);
    icon_editor_image_data_set(evas_object_image_data_get(im, FALSE), w, h);
  }
  /* FIXME create a new file name based on the name */
  /* (delete the old file first?) */
  edje_edit_file_save(editor->edf, editor->file);
}

char *
icon_editor_exec_get()
{
  char tmp[PATH_MAX], *exec;
  
  if (!editor || !editor->edf || !editor->exec_prog) return NULL;

  if (sscanf(editor->exec_prog->state, "exec %s", tmp))
  {
    exec = editor->exec_prog->state + 5;
    return strdup(exec);
  }
  else return NULL;
}

void
icon_editor_exec_set(char *exec)
{
  char *state = NULL;
  
  if (!editor) return;

  state = calloc(1, PATH_MAX * sizeof(char));


  if (!editor->edf || !editor->exec_prog) return;

  snprintf(state, PATH_MAX, "exec %s", exec);

  printf("setting exec sig: %s\n", state);

  if (editor->exec_prog->state) free(editor->exec_prog->state);
  editor->exec_prog->state = state;
  return;
}

void
icon_editor_image_data_get(void **data, int *w, int *h)
{
  Evas_List *l;
  if (!editor || !editor->edf || !editor->group) return;
  for (l = editor->group->parts; l; l = l->next)
  {
    Edje_Part *part = l->data;

    if (!strcmp(part->name, "icon"))
    {
      int id = part->default_desc->image.id;
      Edje_Edit_Image *im = evas_list_nth(editor->edf->images, id);

      printf("data get: w x h = %d x %d\n", im->w, im->h);
      if (data) *data = im->data;
      if (w) *w = im->w;
      if (h) *h = im->h;
    }
  }
}

void
icon_editor_image_file_set(char *file)
{
  void *data;
  int w, h;

  printf("file: %s\n", file);
  ewl_image_file_set(EWL_IMAGE(editor->icon_image), file, NULL);
  icon_editor_image_data_get(&data, &w, &h);
  icon_editor_image_data_set(data, w, h);
}

void
icon_editor_image_data_set(void *data, int w, int h)
{
  Evas_List *l;
  if (!editor || !editor->edf || !editor->group) return;
  for (l = editor->group->parts; l; l = l->next)
  {
    Edje_Part *part = l->data;

    if (!strcmp(part->name, "icon"))
    {
      int id = part->default_desc->image.id;
      Edje_Edit_Image *im = evas_list_nth(editor->edf->images, id);

      /* make sure we have different data before freeing */
      if (im->data && im->data != data) free(im->data);
      im->data = data;
      im->w = w;
      im->h = h;
    }
  }
}

/******************** gui *******************/

void
_icon_editor_gui_update()
{
  char *tmp;
  Evas_Object *im_obj;
  void *im_data = NULL;
  int im_w = 0, im_h = 0;

  tmp = icon_editor_exec_get();
  ewl_entry_text_set(EWL_ENTRY(editor->exec.entry), tmp);
  free(tmp);

  /* FIXME: set the name */
  im_obj = EWL_IMAGE(editor->icon_image)->image;

  icon_editor_image_data_get(&im_data, &im_w, &im_h);
  if (!im_data) return;
  evas_object_image_size_set(im_obj, im_w, im_h);
  evas_object_image_data_copy_set(im_obj, im_data);
  evas_object_image_alpha_set(im_obj, 1);
  evas_object_image_data_update_add(im_obj, 0, 0, im_w, im_h);
  evas_object_image_fill_set(im_obj, 0, 0, im_w, im_h);
  printf("wxh: %d x %d\n", im_w, im_h);
  ewl_object_size_request(EWL_OBJECT(editor->icon_image), im_w, im_h);
  ewl_object_maximum_size_set(EWL_OBJECT(editor->icon_image), im_w, im_h);
  ewl_object_minimum_size_set(EWL_OBJECT(editor->icon_image), im_w, im_h);
 
}

void
_icon_editor_gui_init()
{
  if (!editor) return;

  editor->win = ewl_window_new();
  ewl_window_title_set(EWL_WINDOW(editor->win), "Iconbar Icon Editor");
  ewl_window_name_set(EWL_WINDOW(editor->win), "Iconbar Icon Editor");
  ewl_callback_append(editor->win, EWL_CALLBACK_DELETE_WINDOW,
                      _editor_close_win, NULL);

  /* boxes */
  editor->main_vbox = ewl_vbox_new();
  ewl_container_child_append(EWL_CONTAINER(editor->win), editor->main_vbox);
  ewl_widget_show(editor->main_vbox);
  
  editor->top_hbox = ewl_hbox_new();
  ewl_container_child_append(EWL_CONTAINER(editor->main_vbox), editor->top_hbox);
  ewl_widget_show(editor->top_hbox);

  editor->bot_hbox = ewl_hbox_new();
  ewl_container_child_append(EWL_CONTAINER(editor->main_vbox), editor->bot_hbox);
  ewl_widget_show(editor->bot_hbox);

  /* image */
  editor->icon_image = ewl_image_new("test.png", NULL);
  ewl_container_child_append(EWL_CONTAINER(editor->top_hbox), editor->icon_image);
  ewl_object_padding_set(EWL_OBJECT(editor->icon_image), 5, 5, 5, 5);
  ewl_widget_show(editor->icon_image);
  ewl_callback_append(editor->icon_image, EWL_CALLBACK_REALIZE,
                      _editor_realize, NULL);
  ewl_callback_append(editor->icon_image, EWL_CALLBACK_MOUSE_UP,
                      _editor_icon_image_cb, NULL);

  /* vbox for entry hboxes */
  editor->right_vbox = ewl_vbox_new();
  ewl_container_child_append(EWL_CONTAINER(editor->top_hbox), editor->right_vbox);
  ewl_widget_show(editor->right_vbox);


  /* name */
  editor->name.hbox = ewl_hbox_new();
  ewl_container_child_append(EWL_CONTAINER(editor->right_vbox), editor->name.hbox);
  ewl_widget_show(editor->name.hbox);

  editor->name.label = ewl_text_new("Name: ");
  ewl_container_child_append(EWL_CONTAINER(editor->name.hbox), editor->name.label);
  ewl_widget_show(editor->name.label);

  editor->name.entry = ewl_entry_new("");
  ewl_container_child_append(EWL_CONTAINER(editor->name.hbox), editor->name.entry);
  ewl_widget_show(editor->name.entry);


  /* exec */
  editor->exec.hbox = ewl_hbox_new();
  ewl_container_child_append(EWL_CONTAINER(editor->right_vbox), editor->exec.hbox);
  ewl_widget_show(editor->exec.hbox);

  editor->exec.label = ewl_text_new("Exec:");
  ewl_container_child_append(EWL_CONTAINER(editor->exec.hbox), editor->exec.label);
  ewl_widget_show(editor->exec.label);

  editor->exec.entry = ewl_entry_new("");
  ewl_container_child_append(EWL_CONTAINER(editor->exec.hbox), editor->exec.entry);
  ewl_widget_show(editor->exec.entry);


  /* cancel and save buttons */
  editor->cancel_but = ewl_button_new("Cancel");
  ewl_object_fill_policy_set(EWL_OBJECT(editor->cancel_but), EWL_FLAG_FILL_NONE);
  ewl_object_padding_set(EWL_OBJECT(editor->cancel_but), 5, 5, 5, 5);
  ewl_object_alignment_set(EWL_OBJECT(editor->cancel_but), EWL_FLAG_ALIGN_RIGHT);
  ewl_container_child_append(EWL_CONTAINER(editor->bot_hbox), editor->cancel_but);
  ewl_widget_show(editor->cancel_but);
  ewl_callback_append(editor->cancel_but, EWL_CALLBACK_CLICKED,
                      _editor_button_cb, NULL);

  editor->ok_but = ewl_button_new("Save and Close");
  ewl_object_fill_policy_set(EWL_OBJECT(editor->ok_but), EWL_FLAG_FILL_NONE);
  ewl_object_padding_set(EWL_OBJECT(editor->ok_but), 5, 5, 5, 5);
  ewl_object_alignment_set(EWL_OBJECT(editor->ok_but), EWL_FLAG_ALIGN_RIGHT);
  ewl_container_child_append(EWL_CONTAINER(editor->bot_hbox), editor->ok_but);
  ewl_widget_show(editor->ok_but);
  ewl_callback_append(editor->ok_but, EWL_CALLBACK_CLICKED,
                      _editor_button_cb, NULL);

  editor->filesel.win = ewl_window_new();

  editor->filesel.dialog = ewl_filedialog_new(EWL_FILEDIALOG_TYPE_OPEN);
  ewl_container_child_append(EWL_CONTAINER(editor->filesel.win), editor->filesel.dialog);
  ewl_widget_show(editor->filesel.dialog);
  ewl_callback_append(editor->filesel.dialog, EWL_CALLBACK_VALUE_CHANGED, _editor_filesel_cb, NULL);
}



