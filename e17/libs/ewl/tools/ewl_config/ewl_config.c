#include <Ewl.h>
#include <ewl-config.h>

struct __ewl_config
{
	char *render_method;
	int font_cache;
	int image_cache;
	int enable_debug;
	int debug_level;
	float max_fps;
	float timeout;
	char *theme;
};

typedef struct __ewl_config _Ewl_Config;

struct _ewl_config_main
{
	Ewl_Widget *main_win;
	Ewl_Widget *main_vbox;
	Ewl_Widget *button_hbox;
	Ewl_Widget *button_save;
	Ewl_Widget *button_restore;
	Ewl_Widget *button_defaults;
	Ewl_Widget *button_exit;
	Ewl_Widget *notebook;

	Ewl_Widget *page_evas_label;
	Ewl_Widget *page_evas;
	Ewl_Widget *render_method_label;
	Ewl_Widget *render_method_software;
	Ewl_Widget *render_method_hardware;
	Ewl_Widget *render_method_x11;
	Ewl_Widget *font_cache_label;
	Ewl_Widget *font_cache;
	Ewl_Widget *image_cache_label;
	Ewl_Widget *image_cache;

	Ewl_Widget *page_debug_label;
	Ewl_Widget *page_debug;
	Ewl_Widget *enable_debug;
	Ewl_Widget *debug_level_label;
	Ewl_Widget *debug_level;

	Ewl_Widget *page_fx_label;
	Ewl_Widget *page_fx;
	Ewl_Widget *max_fps_label;
	Ewl_Widget *max_fps;
	Ewl_Widget *timeout_label;
	Ewl_Widget *timeout;

	Ewl_Widget *page_theme_label;
	Ewl_Widget *page_theme;
	Ewl_Widget *theme_name_label;
	Ewl_Widget *theme_name;
}
e_conf;

_Ewl_Config user_settings;
_Ewl_Config system_settings;
_Ewl_Config init_settings;
_Ewl_Config default_settings;

void ewl_config_read_configs(void);
int ewl_config_read_config(char *path, _Ewl_Config * conf);
void ewl_set_settings(_Ewl_Config * c);
_Ewl_Config *ewl_get_settings(void);
void ewl_save_user_config(_Ewl_Config * c);

void ewl_config_save_cb(Ewl_Widget * w, void *ev_data, void *user_data);
void ewl_config_restore_cb(Ewl_Widget * w, void *ev_data, void *user_data);
void ewl_config_defaults_cb(Ewl_Widget * w, void *ev_data, void *user_data);
void ewl_config_exit_cb(Ewl_Widget * w, void *ev_data, void *user_data);

int
main(int argc, char **argv)
{
	memset(&e_conf, 0, sizeof(struct _ewl_config_main));

	ewl_config_read_configs();

	ewl_init(argc, argv);

	ewl_theme_data_set_default("/appearance/box/horizontal/base/visible",
				   "no");
	ewl_theme_data_set_default("/appearance/box/vertical/base/visible",
				   "no");

	e_conf.main_win = ewl_window_new();
	ewl_window_resize(e_conf.main_win, 446, 300);
	ewl_window_set_min_size(e_conf.main_win, 446, 300);
	ewl_window_set_title(e_conf.main_win, "EWL Configuration Program");
	ewl_widget_show(e_conf.main_win);

	e_conf.main_vbox = ewl_vbox_new();
	ewl_box_set_spacing(e_conf.main_vbox, 10);
	ewl_container_append_child(EWL_CONTAINER(e_conf.main_win),
				   e_conf.main_vbox);
	ewl_widget_show(e_conf.main_vbox);

	e_conf.notebook = ewl_notebook_new();
	ewl_container_append_child(EWL_CONTAINER(e_conf.main_vbox),
				   e_conf.notebook);
	ewl_widget_show(e_conf.notebook);

	e_conf.button_hbox = ewl_hbox_new();
	ewl_box_set_spacing(e_conf.button_hbox, 5);
	ewl_object_set_custom_size(EWL_OBJECT(e_conf.button_hbox), 415, 17);
	ewl_object_set_alignment(EWL_OBJECT(e_conf.button_hbox),
				 EWL_ALIGNMENT_CENTER);
	ewl_object_set_fill_policy(EWL_OBJECT(e_conf.button_hbox),
				   EWL_FILL_POLICY_NORMAL);
	ewl_container_append_child(EWL_CONTAINER(e_conf.main_vbox),
				   e_conf.button_hbox);
	ewl_widget_show(e_conf.button_hbox);

	e_conf.button_save = ewl_button_new("Save");
	ewl_object_set_custom_size(EWL_OBJECT(e_conf.button_save), 100, 17);
	ewl_container_append_child(EWL_CONTAINER(e_conf.button_hbox),
				   e_conf.button_save);
	ewl_callback_append(e_conf.button_save, EWL_CALLBACK_CLICKED,
			    ewl_config_save_cb, NULL);
	ewl_widget_show(e_conf.button_save);

	e_conf.button_restore = ewl_button_new("Restore");
	ewl_object_set_custom_size(EWL_OBJECT(e_conf.button_restore), 100,
				   17);
	ewl_container_append_child(EWL_CONTAINER(e_conf.button_hbox),
				   e_conf.button_restore);
	ewl_callback_append(e_conf.button_restore, EWL_CALLBACK_CLICKED,
			    ewl_config_restore_cb, NULL);
	ewl_widget_show(e_conf.button_restore);

	e_conf.button_defaults = ewl_button_new("Defaults");
	ewl_object_set_custom_size(EWL_OBJECT(e_conf.button_defaults), 100,
				   17);
	ewl_container_append_child(EWL_CONTAINER(e_conf.button_hbox),
				   e_conf.button_defaults);
	ewl_callback_append(e_conf.button_defaults, EWL_CALLBACK_CLICKED,
			    ewl_config_defaults_cb, NULL);
	ewl_widget_show(e_conf.button_defaults);

	e_conf.button_exit = ewl_button_new("Exit");
	ewl_object_set_custom_size(EWL_OBJECT(e_conf.button_exit), 100, 17);
	ewl_container_append_child(EWL_CONTAINER(e_conf.button_hbox),
				   e_conf.button_exit);
	ewl_callback_append(e_conf.button_exit, EWL_CALLBACK_CLICKED,
			    ewl_config_exit_cb, NULL);
	ewl_widget_show(e_conf.button_exit);

	/* Evas Page */

	e_conf.page_evas_label = ewl_text_new();
	ewl_text_set_text(e_conf.page_evas_label, "Evas Settings");
	ewl_text_set_font_size(e_conf.page_evas_label, 8);
	ewl_widget_show(e_conf.page_evas_label);

	e_conf.page_evas = ewl_vbox_new();
	ewl_box_set_spacing(e_conf.page_evas, 5);
	ewl_widget_show(e_conf.page_evas);

	e_conf.render_method_label = ewl_text_new();
	ewl_text_set_text(e_conf.render_method_label, "Render Method");
	ewl_container_append_child(EWL_CONTAINER(e_conf.page_evas),
				   e_conf.render_method_label);
	ewl_widget_show(e_conf.render_method_label);

	e_conf.render_method_software =
		ewl_radiobutton_new("Software Engine");
	ewl_container_append_child(EWL_CONTAINER(e_conf.page_evas),
				   e_conf.render_method_software);
	ewl_widget_show(e_conf.render_method_software);

	e_conf.render_method_hardware =
		ewl_radiobutton_new("Hardware Engine");
	ewl_radiobutton_set_chain(e_conf.render_method_hardware,
				  e_conf.render_method_software);
	ewl_container_append_child(EWL_CONTAINER(e_conf.page_evas),
				   e_conf.render_method_hardware);
	ewl_widget_show(e_conf.render_method_hardware);

	e_conf.render_method_x11 = ewl_radiobutton_new("X11 Engine");
	ewl_radiobutton_set_chain(e_conf.render_method_x11,
				  e_conf.render_method_hardware);
	ewl_container_append_child(EWL_CONTAINER(e_conf.page_evas),
				   e_conf.render_method_x11);
	ewl_widget_show(e_conf.render_method_x11);

	e_conf.font_cache_label = ewl_text_new();
	ewl_text_set_text(e_conf.font_cache_label, "Font Cache (kB)");
	ewl_container_append_child(EWL_CONTAINER(e_conf.page_evas),
				   e_conf.font_cache_label);
	ewl_widget_show(e_conf.font_cache_label);

	e_conf.font_cache = ewl_spinner_new();
	ewl_spinner_set_min_val(e_conf.font_cache, (1024 * 1024 * 0.5));
	ewl_spinner_set_max_val(e_conf.font_cache, (1024 * 1024 * 250));
	ewl_spinner_set_digits(e_conf.font_cache, 0);
	ewl_spinner_set_step(e_conf.font_cache, 1);
	ewl_container_append_child(EWL_CONTAINER(e_conf.page_evas),
				   e_conf.font_cache);
	ewl_widget_show(e_conf.font_cache);

	e_conf.image_cache_label = ewl_text_new();
	ewl_text_set_text(e_conf.image_cache_label, "Image Cache (kB)");
	ewl_container_append_child(EWL_CONTAINER(e_conf.page_evas),
				   e_conf.image_cache_label);
	ewl_widget_show(e_conf.image_cache_label);

	e_conf.image_cache = ewl_spinner_new();
	ewl_spinner_set_min_val(e_conf.image_cache, (1024 * 1024 * 0.5));
	ewl_spinner_set_max_val(e_conf.image_cache, (1024 * 1024 * 250));
	ewl_spinner_set_digits(e_conf.image_cache, 0);
	ewl_spinner_set_step(e_conf.image_cache, 1);
	ewl_container_append_child(EWL_CONTAINER(e_conf.page_evas),
				   e_conf.image_cache);
	ewl_widget_show(e_conf.image_cache);

	ewl_notebook_append_page(e_conf.notebook, e_conf.page_evas,
				 e_conf.page_evas_label);

	/* Debug Page */

	e_conf.page_debug_label = ewl_text_new();
	ewl_text_set_text(e_conf.page_debug_label, "Debug Settings");
	ewl_text_set_font_size(e_conf.page_debug_label, 8);
	ewl_widget_show(e_conf.page_debug_label);

	e_conf.page_debug = ewl_vbox_new();
	ewl_box_set_spacing(e_conf.page_debug, 5);
	ewl_widget_show(e_conf.page_debug);

	e_conf.enable_debug = ewl_checkbutton_new("Enable Debug ?");
	ewl_container_append_child(EWL_CONTAINER(e_conf.page_debug),
				   e_conf.enable_debug);
	ewl_widget_show(e_conf.enable_debug);

	e_conf.debug_level_label = ewl_text_new();
	ewl_text_set_text(e_conf.debug_level_label, "Debug Level");
	ewl_container_append_child(EWL_CONTAINER(e_conf.page_debug),
				   e_conf.debug_level_label);
	ewl_widget_show(e_conf.debug_level_label);

	e_conf.debug_level = ewl_spinner_new();
	ewl_spinner_set_min_val(e_conf.debug_level, 0.0);
	ewl_spinner_set_max_val(e_conf.debug_level, 20.0);
	ewl_spinner_set_digits(e_conf.debug_level, 0);
	ewl_spinner_set_step(e_conf.debug_level, 1);
	ewl_container_append_child(EWL_CONTAINER(e_conf.page_debug),
				   e_conf.debug_level);
	ewl_widget_show(e_conf.debug_level);

	ewl_notebook_append_page(e_conf.notebook, e_conf.page_debug,
				 e_conf.page_debug_label);

	/* FX Page */

	e_conf.page_fx_label = ewl_text_new();
	ewl_text_set_text(e_conf.page_fx_label, "FX Settings");
	ewl_text_set_font_size(e_conf.page_fx_label, 8);
	ewl_widget_show(e_conf.page_fx_label);

	e_conf.page_fx = ewl_vbox_new();
	ewl_box_set_spacing(e_conf.page_fx, 5);
	ewl_widget_show(e_conf.page_fx);


	e_conf.max_fps_label = ewl_text_new();
	ewl_text_set_text(e_conf.max_fps_label, "Max FPS");
	ewl_container_append_child(EWL_CONTAINER(e_conf.page_fx),
				   e_conf.max_fps_label);
	ewl_widget_show(e_conf.max_fps_label);

	e_conf.max_fps = ewl_spinner_new();
	ewl_container_append_child(EWL_CONTAINER(e_conf.page_fx),
				   e_conf.max_fps);
	ewl_widget_show(e_conf.max_fps);

	e_conf.timeout_label = ewl_text_new();
	ewl_text_set_text(e_conf.timeout_label, "FX Timeout");
	ewl_container_append_child(EWL_CONTAINER(e_conf.page_fx),
				   e_conf.timeout_label);
	ewl_widget_show(e_conf.timeout_label);

	e_conf.timeout = ewl_spinner_new();
	ewl_container_append_child(EWL_CONTAINER(e_conf.page_fx),
				   e_conf.timeout);
	ewl_widget_show(e_conf.timeout);

	ewl_notebook_append_page(e_conf.notebook, e_conf.page_fx,
				 e_conf.page_fx_label);

	/* Theme Page */
	e_conf.page_theme_label = ewl_text_new();
	ewl_text_set_text(e_conf.page_theme_label, "Theme Settings");
	ewl_text_set_font_size(e_conf.page_theme_label, 8);
	ewl_widget_show(e_conf.page_theme_label);

	e_conf.page_theme = ewl_vbox_new();
	ewl_box_set_spacing(e_conf.page_theme, 5);
	ewl_widget_show(e_conf.page_theme);


	e_conf.theme_name_label = ewl_text_new();
	ewl_text_set_text(e_conf.theme_name_label, "Theme Name");
	ewl_container_append_child(EWL_CONTAINER(e_conf.page_theme),
				   e_conf.theme_name_label);
	ewl_widget_show(e_conf.theme_name_label);

	e_conf.theme_name = ewl_entry_new();
	ewl_container_append_child(EWL_CONTAINER(e_conf.page_theme),
				   e_conf.theme_name);
	ewl_widget_show(e_conf.theme_name);

	ewl_notebook_append_page(e_conf.notebook, e_conf.page_theme,
				 e_conf.page_theme_label);

	ewl_set_settings(&init_settings);

	ewl_main();

	exit(1);
}

void
ewl_config_read_configs(void)
{
	char *home;
	char user_config[1024];
	char system_config[1024];
	int user_read;
	int system_read;

	home = getenv("HOME");

	if (!home)
	  {
		  printf("ERROR: Environment variable $HOME was not found.\n"
			 "Try export HOME in a bash-like environment\n"
			 "or setenv HOME in a sh like environment.\n");
		  exit(-1);
	  }

	snprintf(user_config, 1024, "%s/.e/ewl/config/system.db", home);
	snprintf(system_config, 1024, PACKAGE_DATA_DIR "/config/system.db");

	user_read = ewl_config_read_config(user_config, &user_settings);
	system_read = ewl_config_read_config(system_config, &system_settings);

	if (user_read)
		ewl_config_read_config(user_config, &init_settings);
	else if (system_read)
		ewl_config_read_config(system_config, &init_settings);
	else if (!home)
	  {
		  printf("Environment variable $HOME could not be found."
			 "And we couldnt open the system db, exiting.....");
		  exit(-1);
	  }
}

int
ewl_config_read_config(char *path, _Ewl_Config * conf)
{
	E_DB_File *edb;

	if (!path || !conf)
		return -1;

	edb = e_db_open_read(path);

	if (!edb)
	  {
		  printf("ERROR: Failed to open %s read only\n", path);
		  return -1;
	  }

	/* Evas stuff */
	conf->render_method = e_db_str_get(edb, "/evas/render_method");

	if (!conf->render_method)
		conf->render_method = strdup("default");

	if (!e_db_int_get(edb, "/evas/font_cache", &conf->font_cache))
		conf->font_cache = 1024 * 1024 * 2;

	if (!e_db_int_get(edb, "/evas/image_cache", &conf->image_cache))
		conf->image_cache = 1024 * 1024 * 8;

	/* Debug stuff */
	if (!e_db_int_get(edb, "/debug/enable", &conf->enable_debug))
		conf->enable_debug = 0;

	if (!e_db_int_get(edb, "/debug/level", &conf->debug_level))
		conf->debug_level = 0;

	/* FX stuff */
	if (!e_db_float_get(edb, "/fx/max_fps", &conf->max_fps))
		conf->max_fps = 25.0;

	if (!e_db_float_get(edb, "/fx/timeout", &conf->timeout))
		conf->timeout = 2.0;

	/* Theme stuff */
	conf->theme = e_db_str_get(edb, "/theme/name");

	if (!conf->theme)
		conf->theme = strdup("default");

	return 1;
}

void
ewl_set_settings(_Ewl_Config * c)
{
	if (!strncasecmp(c->render_method, "software", 8))
		ewl_radiobutton_set_checked(e_conf.render_method_software, 1);
	else if (!strncasecmp(c->render_method, "hardware", 8))
		ewl_radiobutton_set_checked(e_conf.render_method_hardware, 1);
	else if (!strncasecmp(c->render_method, "x11", 3))
		ewl_radiobutton_set_checked(e_conf.render_method_x11, 1);
	else
		ewl_radiobutton_set_checked(e_conf.render_method_software, 1);

	ewl_spinner_set_value(e_conf.font_cache, (double) (c->font_cache));
	ewl_spinner_set_value(e_conf.image_cache, (double) (c->image_cache));

	ewl_checkbutton_set_checked(e_conf.enable_debug, c->enable_debug);
	ewl_spinner_set_value(e_conf.debug_level, (double) (c->debug_level));

	ewl_spinner_set_value(e_conf.max_fps, (double) (c->max_fps));
	ewl_spinner_set_value(e_conf.timeout, (double) (c->timeout));

	ewl_entry_set_text(e_conf.theme_name, c->theme);
}

_Ewl_Config *
ewl_get_settings(void)
{
	_Ewl_Config *c;

	c = NEW(_Ewl_Config, 1);
	memset(c, 0, sizeof(_Ewl_Config));

	if (ewl_radiobutton_is_checked(e_conf.render_method_software))
		c->render_method = strdup("software");
	else if (ewl_radiobutton_is_checked(e_conf.render_method_hardware))
		c->render_method = strdup("hardware");
	else if (ewl_radiobutton_is_checked(e_conf.render_method_x11))
		c->render_method = strdup("x11");

	c->font_cache = (int) (ewl_spinner_get_value(e_conf.font_cache));
	c->image_cache = (int) (ewl_spinner_get_value(e_conf.image_cache));

	if (ewl_checkbutton_is_checked(e_conf.enable_debug))
		c->enable_debug = 1;
	else
	  {
		  c->enable_debug = 0;
		  c->debug_level = 0;
	  }

	if (c->enable_debug)
		c->debug_level =
			(int) (ewl_spinner_get_value(e_conf.debug_level));

	c->max_fps = (float) (ewl_spinner_get_value(e_conf.max_fps));
	c->timeout = (float) (ewl_spinner_get_value(e_conf.timeout));

	c->theme = ewl_entry_get_text(e_conf.theme_name);

	if (!c->theme)
		c->theme = strdup("default");

	return c;
}

void
ewl_save_config(_Ewl_Config * c, char *path)
{
	E_DB_File *edb;

	if (!c)
		return;

	edb = e_db_open(path);

	if (!edb)
	  {
		  printf("ERROR: Failed to open %s\n", path);
		  return;
	  }

	e_db_float_set(edb, "/evas/font_cache", c->font_cache);
	e_db_float_set(edb, "/evas/image_cache", c->image_cache);
	e_db_str_set(edb, "/evas/render_method", c->render_method);

	e_db_int_set(edb, "/debug/enable", c->enable_debug);
	e_db_int_set(edb, "/debug/level", c->debug_level);

	e_db_float_set(edb, "/fx/max_fps", c->max_fps);
	e_db_float_set(edb, "/fx/timeout", c->timeout);

	e_db_str_set(edb, "/theme/name", c->theme);

	e_db_close(edb);
	e_db_flush();
}

void
ewl_config_save_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	_Ewl_Config *c;
	char *home;
	char user_config[1024];

	home = getenv("HOME");

	if (!home)
	  {
		  printf("ERROR: Environment variable $HOME was not found.\n"
			 "Try export HOME in a bash-like environment\n"
			 "or setenv HOME in a sh like environment.\n");
		  exit(-1);
	  }

	snprintf(user_config, 1024, "%s/.e/ewl/config/system.db", home);

	c = ewl_get_settings();

	ewl_save_config(c, user_config);

	FREE(c->render_method);
	FREE(c->theme);
	FREE(c);

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}

void
ewl_config_restore_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	ewl_set_settings(&init_settings);

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}

void
ewl_config_defaults_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}

void
ewl_config_exit_cb(Ewl_Widget * w, void *user_data, void *ev_data)
{
	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}
