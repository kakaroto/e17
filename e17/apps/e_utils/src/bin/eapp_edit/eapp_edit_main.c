#include <Eet.h>
#include <Ewl.h>

static void _e_ewl_eapp_help(void);
static Ewl_Widget *_e_ewl_eapp_read(Eet_File *ef, char *key, char *lang,
    char *desc, Ewl_Widget *grid, int row, int checkbox);
static void _e_ewl_eapp_write(Eet_File *ef, char *key, char *lang,
    Ewl_Widget *source, int checkbox);

Ewl_Widget *name, *info, *comments, *exe, *wname, *wclass, *start, *wait;

char *file, *lang;

static void
_e_ewl_eapp_quit(Ewl_Widget *w, void *ev, void *data)
{
   ewl_main_quit();
}

static int
_e_ewl_eapp_save(Ewl_Widget *w, void *ev, void *data)
{
   Eet_File *ef;

   ef = eet_open(file, EET_FILE_MODE_RW);
   if (!ef)
     {
printf("ERROR: cannot open file %s for READ/WRITE\n", file);
return 0;
     }
   _e_ewl_eapp_write(ef, "app/info/name", lang, name, 0);
   _e_ewl_eapp_write(ef, "app/info/generic", lang, info, 0);
   _e_ewl_eapp_write(ef, "app/info/comments", lang, comments, 0);
   _e_ewl_eapp_write(ef, "app/info/exe", NULL, exe, 0);
   _e_ewl_eapp_write(ef, "app/window/name", NULL, wname, 0);
   _e_ewl_eapp_write(ef, "app/window/class", NULL, wclass, 0);
   _e_ewl_eapp_write(ef, "app/info/startup_notify", NULL, start, 1);
   _e_ewl_eapp_write(ef, "app/info/wait_exit", NULL, wait, 1);

   eet_close(ef);
   _e_ewl_eapp_quit(NULL, NULL, NULL);
   return 1;
}

static Ewl_Widget *
_e_ewl_eapp_read(Eet_File *ef, char *key, char *lang, char *desc,
Ewl_Widget *grid, int row, int checkbox)
{
   char buf[4096];
   char *ret, *ret_buf;
   int size_ret;
   Ewl_Widget *cell1, *cell2, *text, *part;

   if (lang) snprintf(buf, sizeof(buf), "%s[%s]", key, lang);
   else snprintf(buf, sizeof(buf), "%s", key);
   ret = (char *) eet_read(ef, buf, &size_ret);
   ret_buf = malloc(size_ret + 1);
   snprintf(ret_buf, size_ret + 1, "%s", ret);
                  
   cell1 = ewl_cell_new();
   cell2 = ewl_cell_new();
   text = ewl_text_new(desc);
   if (checkbox) 
     {
part = ewl_checkbutton_new("");
	ewl_checkbutton_checked_set(EWL_CHECKBUTTON(part), ret_buf[0] == 1);
     }
   else{
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
   free(ret_buf);

   return part;
}

static void
_e_ewl_eapp_write(Eet_File *ef, char *key, char *lang, Ewl_Widget *source,
int checkbox)
{
   char buf[4096];
   char *ret;
   int size_ret;
   int delete = 0;

   if (checkbox)
     {
ret[0] = ewl_checkbutton_is_checked(EWL_CHECKBUTTON(source));
size_ret = 1;
     }
   else
     {
ret = ewl_entry_text_get(EWL_ENTRY(source));
size_ret = strlen(ret);
if (size_ret == 0)
delete = 1;
     }
   
   if (lang) snprintf(buf, sizeof(buf), "%s[%s]", key, lang);
   else snprintf(buf, sizeof(buf), "%s", key);
   if (delete)
     eet_delete(ef, buf);
   else
     eet_write(ef, buf, ret, size_ret, 0);
}

/* externally accessible functions */
int
main(int argc, char **argv)
{
   int i;
   Eet_File *ef;

   Ewl_Widget *main_win, *main_box, *grid, *cell, *content;
   
   /* handle some command-line parameters */
   for (i = 1; i < argc; i++)
     {
	if ((!strcmp(argv[i], "-lang")) && (i < (argc - 1)))
	  {
	     i++;
	     lang = argv[i];
	  }
	else if ((!strcmp(argv[i], "-h")) ||
		 (!strcmp(argv[i], "-help")) ||
		 (!strcmp(argv[i], "--h")) ||
		 (!strcmp(argv[i], "--help")))
	  {
	     _e_ewl_eapp_help();
	     exit(0);
	  }
	else
	  file = argv[i];
     }
   if (!file)
     {
	printf("ERROR: no file specified!\n");
	_e_ewl_eapp_help();
	exit(0);
     }
   eet_init();
   ef = eet_open(file, EET_FILE_MODE_READ);
   if (!ef)
     {
	printf("ERROR: cannot open file %s for READ\n", file);
	return -1;
     }
   ewl_init(&argc, argv);

   main_win = ewl_window_new();
   ewl_window_title_set(EWL_WINDOW(main_win), "Eapp Editor");
   ewl_window_class_set(EWL_WINDOW(main_win), "Eapp");

   ewl_callback_append(main_win, EWL_CALLBACK_DELETE_WINDOW, _e_ewl_eapp_quit, NULL);
   ewl_object_size_request(EWL_OBJECT(main_win), 210, 200);
   ewl_widget_show(main_win);

   main_box = ewl_vbox_new();
   ewl_container_child_append(EWL_CONTAINER(main_win), main_box);
   ewl_widget_show(main_box);

   grid = ewl_grid_new(3, 10);
   ewl_container_child_append(EWL_CONTAINER(main_box), grid);
   ewl_widget_show(grid);
   ewl_object_fill_policy_set(EWL_OBJECT(grid), EWL_FLAG_FILL_ALL);

   name = _e_ewl_eapp_read(ef, "app/info/name", lang, "App name", grid, 3, 0);
   info = _e_ewl_eapp_read(ef, "app/info/generic", lang, "Generic info", grid, 4, 0);
   comments = _e_ewl_eapp_read(ef, "app/info/comments", lang, "Comments", grid, 5, 0);
   exe = _e_ewl_eapp_read(ef, "app/info/exe", NULL, "Executable", grid, 6, 0);
   wname = _e_ewl_eapp_read(ef, "app/window/name", NULL, "Window name", grid, 7, 0);
   wclass = _e_ewl_eapp_read(ef, "app/window/class", NULL, "Window class", grid, 8, 0);
   start = _e_ewl_eapp_read(ef, "app/info/startup_notify", NULL, "Startup notify", grid, 9, 1);
   wait = _e_ewl_eapp_read(ef, "app/info/wait_exit", NULL, "Wait exit", grid, 10, 1);
   
   eet_close(ef);

   cell = ewl_cell_new();
   content = ewl_image_new(file, "icon");
   ewl_widget_show(content);
   ewl_container_child_append(EWL_CONTAINER(cell), content);
   ewl_widget_show(cell);
   ewl_grid_add(EWL_GRID(grid), cell, 1, 1, 1, 2);

   grid = ewl_hbox_new();
   ewl_container_child_append(EWL_CONTAINER(main_box), grid);
   ewl_object_fill_policy_set(EWL_OBJECT(grid), EWL_FLAG_FILL_HFILL);
   ewl_widget_show(grid);

   content = ewl_button_new("Save");
   ewl_callback_append(content, EWL_CALLBACK_CLICKED, _e_ewl_eapp_save, file);
   ewl_widget_show(content);
   ewl_container_child_append(EWL_CONTAINER(grid), content);
   
   content = ewl_button_new("Cancel");
   ewl_callback_append(content, EWL_CALLBACK_CLICKED, _e_ewl_eapp_quit, NULL);
   ewl_widget_show(content);
   ewl_container_child_append(EWL_CONTAINER(grid), content);

printf("WARNING:\n");
printf("DO NOT click save unless you know you have the latest EWL (14/12/04)\n");
printf("you have been warned\n");

   ewl_main();

   ewl_shutdown();
   eet_shutdown();
   /* just return 0 to keep the compiler quiet */
   return 0;
}

static void
_e_ewl_eapp_help(void)
{
   printf("USAGE:\n"
	  "enlightenment_ewl_eapp file.eapp\n"
	  );
}
