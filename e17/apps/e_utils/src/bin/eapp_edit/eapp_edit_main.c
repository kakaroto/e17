#include <Eet.h>
#include <Ewl.h>
#include <Engrave.h>
#include <Ecore_X.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static void _eapp_edit_help(void);
static Ewl_Widget *_eapp_edit_read(Eet_File *ef, char *key, char *lang,
    char *desc, Ewl_Widget *grid, int row, int checkbox);
static void _eapp_edit_write(Eet_File *ef, char *key, char *lang,
    Ewl_Widget *source, int checkbox);

static void _eapp_edit_window_configure_cb(Ewl_Widget *w, void *ev_data, void *user_data);
static void _eapp_edit_drag_start(Ewl_Widget * w, void *ev_data, void *user_data);
static void _eapp_edit_drag_end(Ewl_Widget * w, void *ev_data, void *user_data);

Ewl_Widget *name, *info, *comments, *exe, *wname, *wclass, *start, *wait;
Ewl_Widget *icon, *dialog, *dialog_win, *main_win;

char *file, *lang, *icon_file;
char *new_win_class;

static void
_eapp_edit_quit(Ewl_Widget *w, void *ev, void *data) {
  ewl_main_quit();
}

static void
_eapp_edit_save(Ewl_Widget *w, void *ev, void *data) {
  Eet_File *ef;

  if (icon_file) {
    Engrave_File *eet;
    Engrave_Image *image;
    Engrave_Group *grp;
    Engrave_Part *part;
    Engrave_Part_State *ps;
    char *idir, *ifile;

    ifile = strrchr(icon_file, '/');
    *ifile = '\0';
    idir = strdup(icon_file);

    *ifile = '/';
    ifile ++;

    eet = engrave_file_new();
    engrave_file_image_dir_set(eet, idir);
    image = engrave_image_new(ifile, ENGRAVE_IMAGE_TYPE_COMP, 0);
    engrave_file_image_add(eet, image);

    grp = engrave_group_new();
    engrave_group_name_set(grp, "icon");
    engrave_group_max_size_set(grp, 48, 48);
    engrave_file_group_add(eet, grp);

    part = engrave_part_new(ENGRAVE_PART_TYPE_IMAGE);
    engrave_part_name_set(part, "image");
    engrave_part_mouse_events_set(part, 0);
    engrave_group_part_add(grp, part);

    ps = engrave_part_state_new();
    engrave_part_state_name_set(ps, "default", 0.0);
    engrave_part_state_aspect_set(ps, 1.0, 1.0);
    engrave_part_state_image_normal_set(ps, image);
    engrave_part_state_add(part, ps);

    engrave_eet_output(eet, file);
    engrave_file_free(eet);

    free(idir);
  } 
  
  /* if the file does not exist it should do now... */
  ef = eet_open(file, EET_FILE_MODE_READ_WRITE);
  if (!ef) {
    printf("ERROR: cannot open file %s for READ/WRITE\n \
  (if a new icon you must supply an image)\n", file);
    return;
  }

  _eapp_edit_write(ef, "app/info/name", lang, name, 0);
  _eapp_edit_write(ef, "app/info/generic", lang, info, 0);
  _eapp_edit_write(ef, "app/info/comments", lang, comments, 0);
  _eapp_edit_write(ef, "app/info/exe", NULL, exe, 0);
  _eapp_edit_write(ef, "app/window/name", NULL, wname, 0);
  _eapp_edit_write(ef, "app/window/class", NULL, wclass, 0);
  _eapp_edit_write(ef, "app/info/startup_notify", NULL, start, 1);
  _eapp_edit_write(ef, "app/info/wait_exit", NULL, wait, 1);

  eet_close(ef);
  _eapp_edit_quit(NULL, NULL, NULL);
}

static Ewl_Widget *
_eapp_edit_read(Eet_File *ef, char *key, char *lang, char *desc,
    Ewl_Widget *grid, int row, int checkbox) {
  char buf[4096];
  char *ret, *ret_buf;
  int size_ret;
  Ewl_Widget *cell1, *cell2, *text, *part;

  if (ef) {
    if (lang)
      snprintf(buf, sizeof(buf), "%s[%s]", key, lang);
    else
      snprintf(buf, sizeof(buf), "%s", key);
    ret = (char *) eet_read(ef, buf, &size_ret);
    ret_buf = malloc(size_ret + 1);
    snprintf(ret_buf, size_ret + 1, "%s", ret);
  } else
    ret_buf = "";
                  
  cell1 = ewl_cell_new();
  cell2 = ewl_cell_new();
  text = ewl_text_new(desc);
  if (checkbox) {
    part = ewl_checkbutton_new("");
    ewl_checkbutton_checked_set(EWL_CHECKBUTTON(part), ret_buf[0] == 1);
  } else {
    part = ewl_entry_new(ret_buf);
  }
  ewl_container_child_append(EWL_CONTAINER(cell1), text);
  ewl_container_child_append(EWL_CONTAINER(cell2), part);
  ewl_widget_show(text);
  ewl_widget_show(part);
  ewl_widget_show(cell1);
  ewl_widget_show(cell2);
  ewl_grid_add(EWL_GRID(grid), cell1, 1, 1, row, row);
  ewl_grid_add(EWL_GRID(grid), cell2, 2, 3, row, row);

  if (ef)
    free(ret_buf);

  return part;
}

static void
_eapp_edit_write(Eet_File *ef, char *key, char *lang, Ewl_Widget *source,
    int checkbox) {
  char buf[4096];
  char *ret;
  char ret_char;
  int size_ret;
  int delete;

  ret = NULL;
  delete = 0;
  if (checkbox) {
    ret_char = ewl_checkbutton_is_checked(EWL_CHECKBUTTON(source));
    size_ret = 1;
  } else {
    ret = ewl_text_text_get(EWL_TEXT(source));
    if (ret)
      size_ret = strlen(ret);
    else
      size_ret = 0;
    if (size_ret == 0)
      delete = 1;
  }
   
  if (lang)
    snprintf(buf, sizeof(buf), "%s[%s]", key, lang);
  else
    snprintf(buf, sizeof(buf), "%s", key);
  if (delete)
    eet_delete(ef, buf);
  else {
    if (checkbox)
      eet_write(ef, buf, &ret_char, 1, 0);
    else
      eet_write(ef, buf, ret, size_ret, 0);
  }
}

void
_eapp_edit_dialog_destroy(Ewl_Widget * w, void *ev_data, void *user_data) {
  ewl_widget_hide(w);
}

void
_eapp_edit_dialog_changed(Ewl_Widget * w, void *ev_data, void *user_data) {
  int *click;
  int iw;
  int ih;
  
  click = (int *)ev_data;

  switch (*click) {
    case EWL_RESPONSE_OPEN:
      icon_file = ewl_filedialog_file_get(EWL_FILEDIALOG(w));
      iw = ewl_object_current_w_get(EWL_OBJECT(icon));
      ih = ewl_object_current_h_get(EWL_OBJECT(icon));
      ewl_image_file_set(EWL_IMAGE(icon), icon_file, "");
      break;
    case EWL_RESPONSE_CANCEL:
      break;
  }
  ewl_widget_hide(dialog_win);
}

void
_eapp_edit_dialog_show(Ewl_Widget * w, void *ev_data, void *user_data) {
  
  if (!dialog_win) {
    dialog_win = ewl_window_new();
    ewl_window_title_set(EWL_WINDOW(dialog_win), "Eapp Editor Icon Selection");
    ewl_window_name_set(EWL_WINDOW(dialog_win), "Eapp Editor Icon Selection");
    ewl_window_class_set(EWL_WINDOW(dialog_win), "Eapp Editor");
    ewl_object_size_request(EWL_OBJECT(dialog_win), 300, 200);
    ewl_callback_append(dialog_win, EWL_CALLBACK_DELETE_WINDOW,
        _eapp_edit_dialog_destroy, NULL);
    ewl_widget_show(dialog_win);

    dialog = ewl_filedialog_new(EWL_FILEDIALOG_TYPE_OPEN);
    ewl_callback_append(dialog, EWL_CALLBACK_VALUE_CHANGED,
        _eapp_edit_dialog_changed, NULL);
    ewl_container_child_append(EWL_CONTAINER(dialog_win), dialog);
    ewl_widget_show(dialog);
  }

  ewl_widget_show(dialog_win);
}

/* externally accessible functions */
int
main(int argc, char **argv) {
  int i;
  Eet_File *ef;
  struct stat st;

  Ewl_Widget *main_box, *grid, *cell, *content;

  icon_file = NULL;
  dialog_win = NULL;
  ef = NULL;
  /* handle some command-line parameters */
  for (i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-l") || !strcmp(argv[i], "--lang")) {
      if (i < (argc - 1)) {
        lang = argv[++i];
      } else {
        printf("missing argument for -lang\n");
      }
    } else if (!strcmp(argv[i], "-c") || 
        !strcmp(argv[i], "--win-class")) {
      if (i < (argc - 1)) {
        new_win_class = argv[++i];
      } else {
        printf("missing argument for -win-class\n");
      }
    } else if ((!strcmp(argv[i], "-h")) ||
        (!strcmp(argv[i], "-help")) ||
        (!strcmp(argv[i], "--h")) ||
        (!strcmp(argv[i], "--help"))) {
      _eapp_edit_help();
      exit(0);
    } else
      file = argv[i];
  }
  if (!file) {
    printf("ERROR: no file specified!\n");
    _eapp_edit_help();
    exit(0);
  }
  ecore_init();
  eet_init();
  if (stat(file, &st) < 0)
    printf("file %s not found, will create when you save\n", file);
  else {
    ef = eet_open(file, EET_FILE_MODE_READ);
    if (!ef) {
      printf("ERROR: cannot open file %s for READ\n", file);
      return -1;
    }
  }
  ewl_init(&argc, argv);

  main_win = ewl_window_new();
  ewl_window_title_set(EWL_WINDOW(main_win), "Eapp Editor");
  ewl_window_class_set(EWL_WINDOW(main_win), "Eapp Editor");

  ewl_callback_append(main_win, EWL_CALLBACK_DELETE_WINDOW, _eapp_edit_quit, NULL);
  ewl_callback_append(main_win, EWL_CALLBACK_CONFIGURE, _eapp_edit_window_configure_cb, NULL);
  ewl_object_size_request(EWL_OBJECT(main_win), 210, 200);
  ewl_widget_show(main_win);

  main_box = ewl_vbox_new();
  ewl_container_child_append(EWL_CONTAINER(main_win), main_box);
  ewl_widget_show(main_box);

  grid = ewl_grid_new(3, 10);
  ewl_container_child_append(EWL_CONTAINER(main_box), grid);
  ewl_widget_show(grid);
  ewl_object_fill_policy_set(EWL_OBJECT(grid), EWL_FLAG_FILL_ALL);

  name = _eapp_edit_read(ef, "app/info/name", lang, "App name", grid, 3, 0);
  info = _eapp_edit_read(ef, "app/info/generic", lang, "Generic info", grid, 4, 0);
  comments = _eapp_edit_read(ef, "app/info/comments", lang, "Comments", grid, 5, 0);
  exe = _eapp_edit_read(ef, "app/info/exe", NULL, "Executable", grid, 6, 0);
  wname = _eapp_edit_read(ef, "app/window/name", NULL, "Window name", grid, 7, 0);
  wclass = _eapp_edit_read(ef, "app/window/class", NULL, "Window class", grid, 8, 0);
  if (new_win_class)
    ewl_text_text_set(EWL_TEXT(wclass), new_win_class);
  start = _eapp_edit_read(ef, "app/info/startup_notify", NULL, "Startup notify", grid, 9, 1);
  wait = _eapp_edit_read(ef, "app/info/wait_exit", NULL, "Wait exit", grid, 10, 1);
   
  if (ef)
    eet_close(ef);

  cell = ewl_cell_new();
  content = ewl_button_new("Set Icon");
  ewl_callback_append(content, EWL_CALLBACK_CLICKED, _eapp_edit_dialog_show, NULL);
  ewl_widget_show(content);
  ewl_container_child_append(EWL_CONTAINER(cell), content);
  ewl_widget_show(cell);
  ewl_grid_add(EWL_GRID(grid), cell, 2, 2, 1, 1);

  cell = ewl_cell_new();
  icon = ewl_image_new(file, "icon");
  ewl_image_proportional_set(EWL_IMAGE(icon), TRUE);
  ewl_image_scale_to(EWL_IMAGE(icon), 32, 32);
  ewl_widget_show(icon);
  ewl_container_child_append(EWL_CONTAINER(cell), icon);
  ewl_object_fill_policy_set(EWL_OBJECT(cell), EWL_FLAG_FILL_ALL);
  ewl_widget_show(cell);
  ewl_grid_add(EWL_GRID(grid), cell, 1, 1, 1, 2);

  ewl_callback_append(icon, EWL_CALLBACK_MOUSE_DOWN, _eapp_edit_drag_start, NULL);
  ewl_callback_append(icon, EWL_CALLBACK_MOUSE_UP, _eapp_edit_drag_end, NULL);

  grid = ewl_hbox_new();
  ewl_container_child_append(EWL_CONTAINER(main_box), grid);
  ewl_object_fill_policy_set(EWL_OBJECT(grid), EWL_FLAG_FILL_HFILL);
  ewl_widget_show(grid);

  content = ewl_button_new("Save");
  ewl_callback_append(content, EWL_CALLBACK_CLICKED, _eapp_edit_save, file);
  ewl_widget_show(content);
  ewl_container_child_append(EWL_CONTAINER(grid), content);
   
  content = ewl_button_new("Cancel");
  ewl_callback_append(content, EWL_CALLBACK_CLICKED, _eapp_edit_quit, NULL);
  ewl_widget_show(content);
  ewl_container_child_append(EWL_CONTAINER(grid), content);

  ewl_main();

  ewl_shutdown();
  eet_shutdown();
  ecore_shutdown();
  /* just return 0 to keep the compiler quiet */
  return 0;
}

static void
_eapp_edit_help(void) {
  printf("USAGE:\n"
      "e_util_eapp_edit [options] file.eap\n\n"
      "if file.eap does not exist a new icon file will be created\n\n"
      "options:\n"
      "  -h --help	View this screen\n"
      "  -l --lang [str]	Set laguage for meta data\n"
      "  -c --win-class [str]	Set the window class to use (used by window managers\n");
}

static void
_eapp_edit_window_configure_cb(Ewl_Widget *w, void *ev_data, void *user_data) {
   ecore_x_dnd_aware_set((Ecore_X_Window) EWL_WINDOW(w)->window, 1);	
}

static void
_eapp_edit_drag_start(Ewl_Widget * w, void *ev_data, void *user_data) {
   char *name;

   if (!file)
     return;
   ecore_x_dnd_type_set((Ecore_X_Window) EWL_WINDOW(main_win)->window,
     "text/uri-list", 1);
   ecore_x_dnd_begin((Ecore_X_Window) EWL_WINDOW(main_win)->window, file,
     strlen(file) * sizeof(char));
}

static void
_eapp_edit_drag_end(Ewl_Widget * w, void *ev_data, void *user_data) {
   ecore_x_dnd_drop();
}
