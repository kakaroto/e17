#include <Elementary.h>
#include <dirent.h>

typedef struct _App App;

struct _App
{
   Evas_Object *win;
   Evas_Object *bg;
   Evas_Object *box;
   Evas_Object *frame;
   Evas_Object *button_box;
   struct {
      Evas_Object *cancel;
      Evas_Object *generate;
   } button;
   Evas_Object *table;
   struct {
      Evas_Object *directory;
      Evas_Object *name;
      Evas_Object *description;
      Evas_Object *author;
      Evas_Object *email;
      Evas_Object *license;
      Evas_Object *template;
   } fields;
   struct { /* not bitfields to get the address of */
      Eina_Bool directory;
      Eina_Bool name;
      Eina_Bool description;
      Eina_Bool author;
      Eina_Bool email;
      Eina_Bool license;
      Eina_Bool template;
   } mask;
   struct {
      Evas_Object *box;
      Evas_Object *desc;
      Evas_Object *screenshot;
   } view;
   Ecore_Exe *exe;
};

static int _log_domain = -1;
#define CRITICAL(...) EINA_LOG_DOM_CRIT(_log_domain, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_log_domain, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(_log_domain, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(_log_domain, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_log_domain, __VA_ARGS__)

static void
on_win_del_req(void *data, Evas_Object *obj, void *event_info)
{
   App *app = data;
   if (app->exe)
     return;
   elm_exit();
}

static void
on_button_cancel_clicked(void *data, Evas_Object *obj, void *event_info)
{
   App *app = data;
   if (app->exe)
     return;
   elm_exit();
}

static char *
entry_get(Evas_Object *entry)
{
   const char *markup = elm_entry_entry_get(entry);
   char *utf8;
   size_t i, len;

   if ((!markup) || (strcmp(markup, "<br>") == 0))
     return NULL;
   utf8 = elm_entry_markup_to_utf8(markup);
   if (!utf8)
     return NULL;

   len = strlen(utf8);
   if (len < 1)
     {
	free(utf8);
	return NULL;
     }
   if (utf8[len - 1] == '\n')
     {
	utf8[len - 1] = '\0';
	len--;
     }

   for (i = 0; i < len; i++)
     if (utf8[i] == '\'')
       utf8[i] = '"';

   return utf8;
}

static Eina_Bool
on_generator_stderr(void *data, int type, void *event)
{
   App *app = data;
   Ecore_Exe_Event_Data *e = event;
   int i;
   if (app->exe != e->exe)
     return EINA_TRUE;

   printf("XXXX TODO: show inwin with error message: [close] [try again]\n");

   for (i = 0; e->lines[i].line; i++)
       ERR(e->lines[i].line);

   return EINA_TRUE;
}

static Eina_Bool
on_generator_del(void *data, int type, void *event)
{
   App *app = data;
   Ecore_Exe_Event_Del *e = event;

   if (app->exe != e->exe)
     return EINA_TRUE;

   DBG("generator finished with code %d", e->exit_code);
   app->exe = NULL;

   printf("XXXX TODO: show inwin with okay: [close] [another project]\n");

   elm_object_disabled_set(app->button.cancel, 0);

   elm_entry_entry_set(app->fields.directory, NULL);
   elm_entry_entry_set(app->fields.name, NULL);
   elm_entry_entry_set(app->fields.description, NULL);

   return EINA_FALSE;
}

static void
on_button_generate_clicked(void *data, Evas_Object *obj, void *event_info)
{
   App *app = data;
   char *directory, *name, *description, *author, *email;
   const char *license, *template;
   char buf[8192];

   directory = entry_get(app->fields.directory);
   name = entry_get(app->fields.name);
   description = entry_get(app->fields.description);
   author = entry_get(app->fields.author);
   email = entry_get(app->fields.email);

   license = elm_hoversel_label_get(app->fields.license);
   template = elm_hoversel_label_get(app->fields.template);

   snprintf(buf, sizeof(buf),
	    "./project-generate.sh '%s' '%s' '%s' '%s' '%s' '%s' '%s'",
	    directory, name, description, author, email, license, template);

   INF("execute: %s", buf);

   elm_object_disabled_set(app->button.generate, 1);
   elm_object_disabled_set(app->button.cancel, 1);

   setenv("GENERATOR_BATCH", "1", 0);
   app->exe = ecore_exe_pipe_run
     (buf,
      ECORE_EXE_PIPE_READ | ECORE_EXE_PIPE_READ_LINE_BUFFERED |
      ECORE_EXE_PIPE_ERROR | ECORE_EXE_PIPE_ERROR_LINE_BUFFERED,
      app);

   free(directory);
   free(name);
   free(description);
   free(author);
   free(email);
}

static void
check_enable_generate(App *app)
{
   elm_object_disabled_set(app->button.generate, 1);
#define C(v) if (!app->mask.v) { DBG("field '" #v "' not set."); return; }
   C(directory);
   C(name);
   C(description);
   C(author);
   C(email);
   C(license);
   C(template);
#undef C
   elm_object_disabled_set(app->button.generate, 0);
}

static Evas_Object *
create_button(App *app, const char *label, void (*cb)(void *, Evas_Object *, void *))
{
   Evas_Object *bt = elm_button_add(app->win);
   if (!bt)
     {
	CRITICAL("Could not create '%s' button.", label);
	return NULL;
     }
   elm_button_label_set(bt, label);
   evas_object_smart_callback_add(bt, "clicked", cb, app);
   elm_box_pack_end(app->button_box, bt);
   evas_object_show(bt);
   return bt;
}

static Evas_Object *
create_label(App *app, const char *label, int row)
{
   Evas_Object *lb = elm_label_add(app->win);
   if (!lb)
     {
	CRITICAL("Could not create '%s' label.", label);
	return NULL;
     }
   evas_object_size_hint_weight_set(lb, 0.0, 0.0);
   evas_object_size_hint_align_set(lb, 1.0, 0.5);
   elm_label_label_set(lb, label);
   elm_table_pack(app->table, lb, 0, row, 1, 1);
   evas_object_show(lb);
   return lb;
}

static void
on_entry_changed(void *data, Evas_Object *entry, void *event_info)
{
   Eina_Bool *mask = data;
   const char *s = elm_entry_entry_get(entry);
   *mask = (s && (strcmp(s, "<br>") != 0));
   check_enable_generate(evas_object_data_get(entry, "app"));
}

static Evas_Object *
create_entry_full(App *app, const char *value, Eina_Bool single, Elm_Scroller_Policy phoriz, Elm_Scroller_Policy pvert, int x, int y, int w, int h, Eina_Bool *mask)
{
   Evas_Object *e, *sc;

   sc = elm_scroller_add(app->win);
   if (!sc)
     {
	CRITICAL("Could not create scroller for entry.");
	return NULL;
     }

   e = elm_entry_add(app->win);
   if (!e)
     {
	CRITICAL("Could not create entry.");
	return NULL;
     }
   evas_object_size_hint_weight_set(e, 1.0, 1.0);
   evas_object_size_hint_align_set(e, -1.0, -1.0);
   elm_entry_single_line_set(e, single);
   elm_entry_editable_set(e, 1);
   elm_entry_entry_set(e, value);
   evas_object_show(e);

   evas_object_data_set(e, "app", app);

   evas_object_smart_callback_add(e, "changed", on_entry_changed, mask);

   elm_scroller_content_set(sc, e);
   elm_scroller_content_min_limit(sc, 0, (pvert == ELM_SCROLLER_POLICY_OFF));
   elm_scroller_policy_set(sc, phoriz, pvert);
   evas_object_size_hint_weight_set
     (sc, 1.0, (pvert == ELM_SCROLLER_POLICY_OFF) ? 0.0 : 1.0);
   evas_object_size_hint_align_set(sc, -1.0, -1.0);
   elm_table_pack(app->table, sc, x, y, w, h);
   evas_object_show(sc);

   return e;
}

static Evas_Object *
create_entry(App *app, const char *value, int row, Eina_Bool *mask)
{
   return create_entry_full
     (app, value, 1, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF,
      1, row, 7, 1, mask);
}

static void
on_license_changed(void *data, Evas_Object *hoversel, void *event_info)
{
   App *app = data;
   Elm_Hoversel_Item *it = event_info;
   elm_hoversel_label_set(hoversel, elm_hoversel_item_label_get(it));
   app->mask.license = EINA_TRUE;
   check_enable_generate(app);
}

static Evas_Object *
create_license(App *app, int row)
{
   Evas_Object *o;
   DIR *dir;
   struct dirent *de;

   o = elm_hoversel_add(app->win);
   if (!o)
     {
	CRITICAL("Could not create hoversel for licenses.");
	return NULL;
     }
   elm_hoversel_hover_parent_set(o, app->win);
   elm_hoversel_label_set(o, "Choose a license...");

   dir = opendir("licenses");
   if (!dir)
     {
	CRITICAL("Could not open licenses dir.");
	return NULL;
     }

   while ((de = readdir(dir)))
     {
	if (de->d_name[0] == '.')
	  continue;
	elm_hoversel_item_add
	  (o, de->d_name, NULL, ELM_ICON_NONE, on_license_changed, app);
     }
   closedir(dir);

   evas_object_size_hint_weight_set(o, 1.0, 0.0);
   evas_object_size_hint_align_set(o, -1.0, -1.0);
   elm_table_pack(app->table, o, 1, row, 7, 1);
   evas_object_show(o);

   return o;
}

static void
on_template_changed(void *data, Evas_Object *hoversel, void *event_info)
{
   App *app = data;
   Elm_Hoversel_Item *it = event_info;
   struct stat st;
   const char *template;
   char *buf, path[PATH_MAX];
   size_t r;
   FILE *fp;

   template = elm_hoversel_item_label_get(it);
   elm_hoversel_label_set(hoversel, template);
   app->mask.template = EINA_TRUE;
   check_enable_generate(app);

   snprintf(path, sizeof(path), "main_templates/%s.png", template);
   if (!elm_image_file_set(app->view.screenshot, path, NULL))
     WRN("no screenshot for template '%s': %s", template, path);
   else
     {
	Evas_Coord w, h;
	elm_image_object_size_get(app->view.screenshot, &w, &h);
	evas_object_size_hint_min_set(app->view.screenshot, w, h);
	evas_object_size_hint_max_set(app->view.screenshot, w, h);
     }

   snprintf(path, sizeof(path), "main_templates/%s.txt", template);

   if (stat(path, &st) != 0)
     {
	WRN("no description file for template '%s' at '%s'",
	    template, path);
	return;
     }
   if (st.st_size < 1)
     {
	WRN("empty description '%s'", path);
	return;
     }

   buf = alloca(st.st_size + 1);
   if (!buf)
     return;

   fp = fopen(path, "rb+");
   if (!fp)
     {
	WRN("could not open file: %s", strerror(errno));
	return;
     }

   /* TODO: change to fd_handler */
   r = fread(buf, 1, st.st_size, fp);
   buf[r] = '\0';
   elm_entry_entry_set(app->view.desc, buf);

   fclose(fp);
}

static Evas_Object *
create_template(App *app, int row)
{
   Evas_Object *o;
   DIR *dir;
   struct dirent *de;

   o = elm_hoversel_add(app->win);
   if (!o)
     {
	CRITICAL("Could not create hoversel for templates.");
	return NULL;
     }
   elm_hoversel_hover_parent_set(o, app->win);
   elm_hoversel_label_set(o, "Choose a template...");

   dir = opendir("main_templates");
   if (!dir)
     {
	CRITICAL("Could not open templates dir.");
	return NULL;
     }

   while ((de = readdir(dir)))
     {
	const char *p;
	char buf[NAME_MAX];

	if (de->d_name[0] == '.')
	  continue;

	p = strchr(de->d_name, '.');
	if (!p)
	  continue;
	else if (p - de->d_name >= NAME_MAX)
	  continue;
	else if (strcmp(p + 1, "c") != 0)
	  continue;

	strncpy(buf, de->d_name, p - de->d_name);
	buf[p - de->d_name] = '\0';

	elm_hoversel_item_add
	  (o, buf, NULL, ELM_ICON_NONE, on_template_changed, app);
     }
   closedir(dir);

   evas_object_size_hint_weight_set(o, 1.0, 0.0);
   evas_object_size_hint_align_set(o, -1.0, -1.0);
   elm_table_pack(app->table, o, 1, row, 7, 1);
   evas_object_show(o);

   return o;
}

static Evas_Object *
create_description(App *app)
{
   Evas_Object *e, *sc;

   sc = elm_scroller_add(app->win);
   if (!sc)
     {
	CRITICAL("Could not create scroller for entry.");
	return NULL;
     }

   e = elm_entry_add(app->win);
   if (!e)
     {
	CRITICAL("Could not create entry.");
	return NULL;
     }
   evas_object_size_hint_weight_set(e, 1.0, 1.0);
   evas_object_size_hint_align_set(e, -1.0, -1.0);
   elm_entry_single_line_set(e, 0);
   elm_entry_editable_set(e, 0);
   evas_object_show(e);

   elm_scroller_content_set(sc, e);
   elm_scroller_content_min_limit(sc, 0, 0);
   elm_scroller_policy_set(sc, ELM_SCROLLER_POLICY_AUTO, ELM_SCROLLER_POLICY_AUTO);
   evas_object_size_hint_weight_set(sc, 1.0, 1.0);
   evas_object_size_hint_align_set(sc, -1.0, -1.0);
   elm_box_pack_end(app->view.box, sc);
   evas_object_show(sc);

   return e;
}

static Evas_Object *
create_screenshot(App *app)
{
   Evas_Object *i, *sc;

   sc = elm_scroller_add(app->win);
   if (!sc)
     {
	CRITICAL("Could not create scroller for entry.");
	return NULL;
     }

   i = elm_image_add(app->win);
   if (!i)
     {
	CRITICAL("Could not create entry.");
	return NULL;
     }
   evas_object_size_hint_align_set(i, 0.5, 0.5);
   evas_object_size_hint_weight_set(i, 1.0, 1.0);
   evas_object_size_hint_align_set(i, -1.0, -1.0);
   elm_image_smooth_set(i, 1);
   evas_object_show(i);

   elm_scroller_content_set(sc, i);
   evas_object_size_hint_align_set(sc, -1.0, -1.0);
   evas_object_size_hint_weight_set(sc, 1.0, 1.0);
   evas_object_size_hint_min_set(sc, 400, 400);
   elm_box_pack_end(app->view.box, sc);
   evas_object_show(sc);

   return i;
}

static Eina_Bool
create_main_win(App *app)
{
   Evas_Object *label;

   app->win = elm_win_add(NULL, "generator-ui", ELM_WIN_BASIC);
   if (!app->win)
     {
	CRITICAL("Could not create window.");
	return EINA_FALSE;
     }
   elm_win_title_set(app->win, "Elementary Project Generator");
   evas_object_smart_callback_add
     (app->win, "delete-request", on_win_del_req, app);

   app->bg = elm_bg_add(app->win);
   if (!app->bg)
     {
	CRITICAL("Could not create background.");
	return EINA_FALSE;
     }
   evas_object_size_hint_weight_set(app->bg, 1.0, 1.0);
   elm_win_resize_object_add(app->win, app->bg);
   evas_object_show(app->bg);

   app->box = elm_box_add(app->win);
   if (!app->box)
     {
	CRITICAL("Could not create box.");
	return EINA_FALSE;
     }
   elm_box_horizontal_set(app->box, EINA_FALSE);
   elm_box_homogenous_set(app->box, EINA_FALSE);
   evas_object_size_hint_weight_set(app->box, 1.0, 1.0);
   elm_win_resize_object_add(app->win, app->box);
   evas_object_show(app->box);

   app->frame = elm_frame_add(app->win);
   if (!app->frame)
     {
	CRITICAL("Could not create frame.");
	return EINA_FALSE;
     }
   elm_frame_label_set(app->frame, "Define your project:");
   evas_object_size_hint_weight_set(app->frame, 1.0, 1.0);
   evas_object_size_hint_align_set(app->frame, -1.0, -1.0);
   evas_object_size_hint_min_set(app->frame, 640, 480);
   elm_box_pack_end(app->box, app->frame);
   evas_object_show(app->frame);

   app->button_box = elm_box_add(app->win);
   if (!app->button_box)
     {
	CRITICAL("Could not create box.");
	return EINA_FALSE;
     }
   elm_box_horizontal_set(app->button_box, EINA_TRUE);
   elm_box_homogenous_set(app->button_box, EINA_FALSE);
   evas_object_size_hint_weight_set(app->button_box, 1.0, 0.0);
   evas_object_size_hint_align_set(app->button_box, -1.0, 0.5);
   elm_box_pack_end(app->box, app->button_box);
   evas_object_show(app->button_box);

   app->button.cancel = create_button(app, "Cancel", on_button_cancel_clicked);
   if (!app->button.cancel)
     return EINA_FALSE;

   app->button.generate = create_button(app, "Generate", on_button_generate_clicked);
   if (!app->button.generate)
     return EINA_FALSE;
   elm_object_disabled_set(app->button.generate, EINA_TRUE);

   app->table = elm_table_add(app->win);
   if (!app->table)
     {
	CRITICAL("Could not create table.");
	return EINA_FALSE;
     }
   evas_object_size_hint_weight_set(app->table, 1.0, 1.0);
   evas_object_size_hint_align_set(app->table, -1.0, -1.0);
   elm_frame_content_set(app->frame, app->table);
   evas_object_show(app->table);

   label = create_label(app, "Directory:", 0);
   if (!label)
     return EINA_FALSE;

   app->fields.directory = create_entry(app, NULL, 0, &app->mask.directory);
   if (!app->fields.directory)
     return EINA_FALSE;

   label = create_label(app, "Name:", 1);
   if (!label)
     return EINA_FALSE;

   app->fields.name = create_entry(app, NULL, 1, &app->mask.name);
   if (!app->fields.name)
     return EINA_FALSE;

   label = create_label(app, "Description:", 2);
   if (!label)
     return EINA_FALSE;

   app->fields.description = create_entry_full
     (app, NULL, 0, ELM_SCROLLER_POLICY_AUTO, ELM_SCROLLER_POLICY_AUTO,
      0, 3, 8, 1, &app->mask.description);

   label = create_label(app, "Author:", 4);
   if (!label)
     return EINA_FALSE;

   app->fields.author = create_entry(app, NULL, 4, &app->mask.author);
   if (!app->fields.author)
     return EINA_FALSE;

   label = create_label(app, "Email:", 5);
   if (!label)
     return EINA_FALSE;

   app->fields.email = create_entry(app, NULL, 5, &app->mask.email);
   if (!app->fields.email)
     return EINA_FALSE;

   label = create_label(app, "License:", 6);
   if (!label)
     return EINA_FALSE;

   app->fields.license = create_license(app, 6);
   if (!app->fields.license)
     return EINA_FALSE;

   label = create_label(app, "Template:", 7);
   if (!label)
     return EINA_FALSE;

   app->fields.template = create_template(app, 7);
   if (!app->fields.template)
     return EINA_FALSE;

   app->view.box = elm_box_add(app->win);
   if (!app->view.box)
     {
	CRITICAL("Could not create box.");
	return EINA_FALSE;
     }
   elm_box_horizontal_set(app->view.box, EINA_TRUE);
   elm_box_homogenous_set(app->view.box, EINA_FALSE);
   evas_object_size_hint_weight_set(app->view.box, 1.0, 1.0);
   evas_object_size_hint_align_set(app->view.box, -1.0, -1.0);
   elm_table_pack(app->table, app->view.box, 0, 8, 8, 2);
   evas_object_show(app->view.box);

   app->view.desc = create_description(app);
   if (!app->view.desc)
     return EINA_FALSE;

   app->view.screenshot = create_screenshot(app);
   if (!app->view.screenshot)
     return EINA_FALSE;

   evas_object_resize(app->win, 800, 600);
   evas_object_show(app->win);

   return EINA_TRUE;
}

EAPI int
elm_main(int argc, char **argv)
{
   App app;
   int r = 0;

   _log_domain = eina_log_domain_register("generator-ui", NULL);
   if (_log_domain < 0)
     {
	EINA_LOG_CRIT("could not create log domain 'generator-ui'.");
	return -1;
     }

   memset(&app, 0, sizeof(app));
   if (!create_main_win(&app))
     {
	r = -1;
	goto end;
     }

   ecore_event_handler_add(ECORE_EXE_EVENT_ERROR, on_generator_stderr, &app);
   ecore_event_handler_add(ECORE_EXE_EVENT_DEL, on_generator_del, &app);

   elm_run();
 end:
   eina_log_domain_unregister(_log_domain);
   _log_domain = -1;
   elm_shutdown();
   return r;
}
ELM_MAIN()
