#include <Ewl.h>
#include <ewl-config.h>

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
	Ewl_Widget *global_label;
	Ewl_Widget *global_fps_label;
	Ewl_Widget *global_fps;
	Ewl_Widget *fx_separator1;
	Ewl_Widget *plugins_label;
	Ewl_Widget *effects_table;

	Ewl_Widget *page_theme_label;
	Ewl_Widget *page_theme;
	Ewl_Widget *theme_name_label;
	Ewl_Widget *theme_name;
	Ewl_Widget *theme_cache_label;
	Ewl_Widget *theme_cache;
}
e_conf;

struct _confirm_win
{
	Ewl_Widget *win;
	Ewl_Widget *main_vbox, *button_hbox;
	Ewl_Widget *text;
	Ewl_Widget *button_save, *button_exit, *button_cancel;
}
confirm;

Ewl_Config user_settings;
Ewl_Config init_settings;
Ewl_Config default_settings;

void ewl_config_read_configs(void);
int ewl_config_read_config(Ewl_Config * conf);
void ewl_set_settings(Ewl_Config * c);
Ewl_Config *ewl_get_settings(void);
void ewl_save_user_config(Ewl_Config * c);

void ewl_config_save_cb(Ewl_Widget * w, void *ev_data, void *user_data);
void ewl_config_restore_cb(Ewl_Widget * w, void *ev_data, void *user_data);
void ewl_config_defaults_cb(Ewl_Widget * w, void *ev_data, void *user_data);
void ewl_config_exit_cb(Ewl_Widget * w, void *ev_data, void *user_data);
void ewl_config_create_confirm_dialog(void);
void ewl_config_destroy_confirm_dialog(Ewl_Widget * w, void *ev_data,
				       void *user_data);
int
main(int argc, char **argv)
{
	Ewd_List *avail;

	memset(&e_conf, 0, sizeof(struct _ewl_config_main));
	memset(&confirm, 0, sizeof(struct _confirm_win));

	ewl_init(argc, argv);

	avail = ewl_fx_get_available();

	ewl_config_read_configs();

	e_conf.main_win = ewl_window_new();
	ewl_window_resize(EWL_WINDOW(e_conf.main_win), 446, 300);
	ewl_window_set_min_size(EWL_WINDOW(e_conf.main_win), 446, 300);
	ewl_window_set_title(EWL_WINDOW(e_conf.main_win),
			"EWL Configuration Program");
	ewl_callback_append(e_conf.main_win, EWL_CALLBACK_DELETE_WINDOW,
			    ewl_config_exit_cb, NULL);
	ewl_widget_show(e_conf.main_win);

	e_conf.main_vbox = ewl_vbox_new();
	ewl_box_set_spacing(EWL_BOX(e_conf.main_vbox), 10);
	ewl_container_append_child(EWL_CONTAINER(e_conf.main_win),
				   e_conf.main_vbox);
	ewl_widget_show(e_conf.main_vbox);

	e_conf.notebook = ewl_notebook_new();
	ewl_container_append_child(EWL_CONTAINER(e_conf.main_vbox),
				   e_conf.notebook);
	ewl_widget_show(e_conf.notebook);

	e_conf.button_hbox = ewl_hbox_new();
	ewl_box_set_spacing(EWL_BOX(e_conf.button_hbox), 5);
	ewl_object_set_padding(EWL_OBJECT(e_conf.button_hbox), 0, 0, 0, 10);
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

	e_conf.page_evas_label = ewl_text_new("Evas Settings");
	ewl_text_set_font_size(EWL_TEXT(e_conf.page_evas_label), 8);
	ewl_widget_show(e_conf.page_evas_label);

	e_conf.page_evas = ewl_vbox_new();
	ewl_box_set_spacing(EWL_BOX(e_conf.page_evas), 5);
	ewl_object_set_padding(EWL_OBJECT(e_conf.page_evas), 10, 5, 5, 0);
	ewl_widget_show(e_conf.page_evas);

	e_conf.render_method_label = ewl_text_new("Render Method");
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

	e_conf.font_cache_label = ewl_text_new("Font Cache (kB)");
	ewl_container_append_child(EWL_CONTAINER(e_conf.page_evas),
				   e_conf.font_cache_label);
	ewl_widget_show(e_conf.font_cache_label);

	e_conf.font_cache = ewl_spinner_new();
	ewl_spinner_set_min_val(EWL_SPINNER(e_conf.font_cache), (1024.0 * 0.5));
	ewl_spinner_set_max_val(EWL_SPINNER(e_conf.font_cache), (1024.0 * 256));
	ewl_spinner_set_digits(EWL_SPINNER(e_conf.font_cache), 0);
	ewl_spinner_set_step(EWL_SPINNER(e_conf.font_cache), 1.0);
	ewl_container_append_child(EWL_CONTAINER(e_conf.page_evas),
				   e_conf.font_cache);
	ewl_widget_show(e_conf.font_cache);

	e_conf.image_cache_label = ewl_text_new("Image Cache (kB)");
	ewl_container_append_child(EWL_CONTAINER(e_conf.page_evas),
				   e_conf.image_cache_label);
	ewl_widget_show(e_conf.image_cache_label);

	e_conf.image_cache = ewl_spinner_new();
	ewl_spinner_set_min_val(EWL_SPINNER(e_conf.image_cache),
			(1024.0 * 0.5));
	ewl_spinner_set_max_val(EWL_SPINNER(e_conf.image_cache),
			(1024.0 * 256));
	ewl_spinner_set_digits(EWL_SPINNER(e_conf.image_cache), 0);
	ewl_spinner_set_step(EWL_SPINNER(e_conf.image_cache), 1.0);
	ewl_container_append_child(EWL_CONTAINER(e_conf.page_evas),
				   e_conf.image_cache);
	ewl_widget_show(e_conf.image_cache);

	ewl_notebook_append_page(EWL_NOTEBOOK(e_conf.notebook),
		e_conf.page_evas, e_conf.page_evas_label);

	/* Debug Page */

	e_conf.page_debug_label = ewl_text_new("Debug Settings");
	ewl_text_set_font_size(EWL_TEXT(e_conf.page_debug_label), 8);
	ewl_widget_show(e_conf.page_debug_label);

	e_conf.page_debug = ewl_vbox_new();
	ewl_object_set_padding(EWL_OBJECT(e_conf.page_debug), 10, 5, 5, 0);
	ewl_box_set_spacing(EWL_BOX(e_conf.page_debug), 5);
	ewl_widget_show(e_conf.page_debug);

	e_conf.enable_debug = ewl_checkbutton_new("Enable Debug ?");
	ewl_container_append_child(EWL_CONTAINER(e_conf.page_debug),
				   e_conf.enable_debug);
	ewl_widget_show(e_conf.enable_debug);

	e_conf.debug_level_label = ewl_text_new("Debug Level");
	ewl_container_append_child(EWL_CONTAINER(e_conf.page_debug),
				   e_conf.debug_level_label);
	ewl_widget_show(e_conf.debug_level_label);

	e_conf.debug_level = ewl_spinner_new();
	ewl_spinner_set_min_val(EWL_SPINNER(e_conf.debug_level), 0.0);
	ewl_spinner_set_max_val(EWL_SPINNER(e_conf.debug_level), 20.0);
	ewl_spinner_set_digits(EWL_SPINNER(e_conf.debug_level), 0);
	ewl_spinner_set_step(EWL_SPINNER(e_conf.debug_level), 1);
	ewl_container_append_child(EWL_CONTAINER(e_conf.page_debug),
				   e_conf.debug_level);
	ewl_widget_show(e_conf.debug_level);

	ewl_notebook_append_page(EWL_NOTEBOOK(e_conf.notebook),
			e_conf.page_debug, e_conf.page_debug_label);

	/* FX Page */
	e_conf.page_fx_label = ewl_text_new("FX Settings");
	ewl_text_set_font_size(EWL_TEXT(e_conf.page_fx_label), 8);
	ewl_widget_show(e_conf.page_fx_label);

	e_conf.page_fx = ewl_vbox_new();
	ewl_object_set_padding(EWL_OBJECT(e_conf.page_fx), 10, 5, 5, 0);
	ewl_box_set_spacing(EWL_BOX(e_conf.page_fx), 5);
	ewl_widget_show(e_conf.page_fx);


	e_conf.global_label = ewl_text_new("Global Settings");
	ewl_container_append_child(EWL_CONTAINER(e_conf.page_fx),
				   e_conf.global_label);
	ewl_widget_show(e_conf.global_label);

	e_conf.global_fps_label = ewl_text_new("Frames Per Second");
	ewl_container_append_child(EWL_CONTAINER(e_conf.page_fx),
				   e_conf.global_fps_label);
	ewl_widget_show(e_conf.global_fps_label);

	e_conf.global_fps = ewl_spinner_new();
	ewl_spinner_set_min_val(EWL_SPINNER(e_conf.global_fps), 1.0);
	ewl_spinner_set_max_val(EWL_SPINNER(e_conf.global_fps), 80.0);
	ewl_spinner_set_digits(EWL_SPINNER(e_conf.global_fps), 1);
	ewl_spinner_set_step(EWL_SPINNER(e_conf.global_fps), 1.0);
	ewl_container_append_child(EWL_CONTAINER(e_conf.page_fx),
				   e_conf.global_fps);
	ewl_widget_show(e_conf.global_fps);

	if (avail && !ewd_list_is_empty(avail))
	  {
		  Ewl_FX_Proto *fxp;
		  Ewl_Widget *label, *settings_button, *about_button;
		  char *name;
		  int i = 0;

		  e_conf.fx_separator1 = ewl_vseparator_new();
		  ewl_object_set_padding(EWL_OBJECT(e_conf.fx_separator1),
					 0, 0, 5, 5);
		  ewl_container_append_child(EWL_CONTAINER(e_conf.page_fx),
					     e_conf.fx_separator1);
		  ewl_widget_show(e_conf.fx_separator1);

		  e_conf.plugins_label = ewl_text_new("Plugins");
		  ewl_container_append_child(EWL_CONTAINER(e_conf.page_fx),
					     e_conf.plugins_label);
		  ewl_widget_show(e_conf.plugins_label);

		  e_conf.effects_table = ewl_grid_new(3, ewd_list_nodes(avail));
		  ewl_container_append_child(EWL_CONTAINER(e_conf.page_fx),
					     e_conf.effects_table);
		  ewl_widget_show(e_conf.effects_table);

		  ewd_list_goto_first(avail);

		  while ((name = ewd_list_next(avail)) != NULL)
		    {
			    ++i;

			    fxp = ewl_fx_proto_get(name);

			    if (!fxp)
			      {
				      ewl_fx_plugin_load(name);

				      fxp = ewl_fx_proto_get(name);

				      if (!fxp)
					      continue;
			      }

			    label = ewl_text_new(name);
			    ewl_grid_add(EWL_GRID(e_conf.effects_table),
					     label, 1, 1, i, i);
			    ewl_widget_show(label);

			    settings_button = ewl_button_new("Settings");
			    ewl_object_set_custom_size(EWL_OBJECT
						       (settings_button), 50,
						       17);
			    ewl_object_set_fill_policy(EWL_OBJECT
						       (settings_button),
						       EWL_FILL_POLICY_NORMAL);
			    if (fxp->create_settings_dialog)
				    ewl_callback_append(settings_button,
							EWL_CALLBACK_CLICKED,
							EWL_CALLBACK_FUNCTION
							(fxp->
							 create_settings_dialog),
							NULL);
			    else
				    ewl_widget_disable(settings_button);

			    ewl_grid_add(EWL_GRID(e_conf.effects_table),
					     settings_button, 2, 2, i, i);
			    ewl_widget_show(settings_button);

			    about_button = ewl_button_new("About");
			    ewl_object_set_custom_size(EWL_OBJECT
						       (about_button), 40,
						       17);
			    ewl_object_set_fill_policy(EWL_OBJECT
						       (about_button),
						       EWL_FILL_POLICY_NORMAL);
			    if (fxp->create_about_dialog)
				    ewl_callback_append(about_button,
							EWL_CALLBACK_CLICKED,
							EWL_CALLBACK_FUNCTION
							(fxp->
							 create_about_dialog),
							NULL);
			    else
				    ewl_widget_disable(about_button);

			    ewl_grid_add(EWL_GRID(e_conf.effects_table),
					     about_button, 3, 3, i, i);
			    ewl_widget_show(about_button);
		    }
	  }

	ewl_notebook_append_page(EWL_NOTEBOOK(e_conf.notebook), e_conf.page_fx,
				 e_conf.page_fx_label);

	/* Theme Page */
	e_conf.page_theme_label = ewl_text_new("Theme Settings");
	ewl_text_set_font_size(EWL_TEXT(e_conf.page_theme_label), 8);
	ewl_widget_show(e_conf.page_theme_label);

	e_conf.page_theme = ewl_vbox_new();
	ewl_object_set_padding(EWL_OBJECT(e_conf.page_theme), 10, 5, 5, 0);
	ewl_box_set_spacing(EWL_BOX(e_conf.page_theme), 5);
	ewl_widget_show(e_conf.page_theme);


	e_conf.theme_name_label = ewl_text_new("Theme Name");
	ewl_container_append_child(EWL_CONTAINER(e_conf.page_theme),
				   e_conf.theme_name_label);
	ewl_widget_show(e_conf.theme_name_label);

	e_conf.theme_name = ewl_entry_new(NULL);
	ewl_container_append_child(EWL_CONTAINER(e_conf.page_theme),
				   e_conf.theme_name);
	ewl_widget_show(e_conf.theme_name);

	e_conf.theme_cache = ewl_checkbutton_new("Cache Theme Data?");
	ewl_container_append_child(EWL_CONTAINER(e_conf.page_theme),
				   e_conf.theme_cache);
	ewl_widget_show(e_conf.theme_cache);

	ewl_notebook_append_page(EWL_NOTEBOOK(e_conf.notebook),
			e_conf.page_theme, e_conf.page_theme_label);

	ewl_set_settings(&init_settings);

	ewl_main();

	exit(1);
}

void
ewl_config_read_configs(void)
{
	int user_read;

	user_read = ewl_config_read_config(&user_settings);

	if (user_read != -1)
		ewl_config_read_config(&init_settings);
	else if (user_read == -1)
	  {
		  printf("Couldnt open user config, please check permissions\n");
		  exit(-1);
	  }
}

int
ewl_config_read_config(Ewl_Config * conf)
{
	if (!conf)
		return -1;

	/* Evas stuff */
	conf->evas.render_method =
		ewl_config_get_str("system", "/evas/render_method");
	if (!conf->evas.render_method)
		conf->evas.render_method = strdup("software");

	conf->evas.font_cache =
		ewl_config_get_int("system", "/evas/font_cache");
	if (!conf->evas.font_cache)
		conf->evas.font_cache = 1024.0 * 1024.0 * 2.0;

	conf->evas.image_cache =
		ewl_config_get_int("system", "/evas/image_cache");
	if (!conf->evas.image_cache)
		conf->evas.image_cache = 1024.0 * 1024.0 * 8.0;

	/* Debug stuff */
	conf->debug.enable = ewl_config_get_int("system", "/debug/enable");

	conf->debug.level = ewl_config_get_int("system", "/debug/level");
	if (!conf->debug.level)
		conf->debug.level = 0;

	/* Theme stuff */
	conf->theme.name = ewl_config_get_str("system", "/theme/name");
	if (!conf->theme.name)
		conf->theme.name = strdup("default");

	conf->theme.cache = ewl_config_get_int("system", "/theme/cache");

	conf->fx.fps = ewl_config_get_int("system", "/fx/fps");

	return 1;
}

void
ewl_set_settings(Ewl_Config * c)
{
	if (c->evas.render_method
	    && !strncasecmp(c->evas.render_method, "software", 8))
		ewl_radiobutton_set_checked(e_conf.render_method_software, 1);
	else if (c->evas.render_method
		 && !strncasecmp(c->evas.render_method, "hardware", 8))
		ewl_radiobutton_set_checked(e_conf.render_method_hardware, 1);
	else if (c->evas.render_method
		 && !strncasecmp(c->evas.render_method, "x11", 3))
		ewl_radiobutton_set_checked(e_conf.render_method_x11, 1);
	else
		ewl_radiobutton_set_checked(e_conf.render_method_software, 1);

	ewl_spinner_set_value(EWL_SPINNER(e_conf.font_cache),
			      (double) (c->evas.font_cache) / 1024.0);
	ewl_spinner_set_value(EWL_SPINNER(e_conf.image_cache),
			      (double) (c->evas.image_cache) / 1024.0);

	ewl_checkbutton_set_checked(EWL_CHECKBUTTON(e_conf.enable_debug),
			c->debug.enable);
	ewl_spinner_set_value(EWL_SPINNER(e_conf.debug_level),
			(double) (c->debug.level));

	ewl_spinner_set_value(EWL_SPINNER(e_conf.global_fps),
			(double) (c->fx.fps));

	ewl_entry_set_text(EWL_ENTRY(e_conf.theme_name), c->theme.name);

	ewl_checkbutton_set_checked(EWL_CHECKBUTTON(e_conf.theme_cache),
			c->theme.cache);
}

Ewl_Config *
ewl_get_settings(void)
{
	Ewl_Config *c;

	c = NEW(Ewl_Config, 1);
	memset(c, 0, sizeof(Ewl_Config));

	if (ewl_radiobutton_is_checked(e_conf.render_method_software))
		c->evas.render_method = strdup("software");
	else if (ewl_radiobutton_is_checked(e_conf.render_method_hardware))
		c->evas.render_method = strdup("hardware");
	else if (ewl_radiobutton_is_checked(e_conf.render_method_x11))
		c->evas.render_method = strdup("x11");

	c->evas.font_cache =
		(float)(ewl_spinner_get_value(EWL_SPINNER(e_conf.font_cache)))
		* 1024.0;
	c->evas.image_cache =
		(float)(ewl_spinner_get_value(EWL_SPINNER(e_conf.image_cache)))
		* 1024.0;

	if (ewl_checkbutton_is_checked(EWL_CHECKBUTTON(e_conf.enable_debug)))
		c->debug.enable = 1;
	else
	  {
		  c->debug.enable = 0;
		  c->debug.level = 0;
	  }

	if (c->debug.enable)
		c->debug.level =
			(int)(ewl_spinner_get_value(EWL_SPINNER(e_conf.debug_level)));

	c->theme.name = ewl_entry_get_text(EWL_ENTRY(e_conf.theme_name));

	if (!c->theme.name)
		c->theme.name = strdup("default");

	c->theme.cache = ewl_checkbutton_is_checked(EWL_CHECKBUTTON(e_conf.theme_cache));
	c->fx.fps = ewl_spinner_get_value(EWL_SPINNER(e_conf.global_fps));

	return c;
}

void
ewl_save_config(Ewl_Config * c)
{
	if (!c)
		return;

	ewl_config_set_int("system", "/evas/font_cache", c->evas.font_cache);
	ewl_config_set_int("system", "/evas/image_cache",
			   c->evas.image_cache);
	ewl_config_set_str("system", "/evas/render_method",
			   c->evas.render_method);
	ewl_config_set_int("system", "/debug/enable", c->debug.enable);
	ewl_config_set_int("system", "/debug/level", c->debug.level);

	ewl_config_set_str("system", "/theme/name", c->theme.name);
	ewl_config_set_int("system", "/theme/cache", c->theme.cache);
	ewl_config_set_int("system", "/fx/fps", c->fx.fps);
}

void
ewl_config_save_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Config *c;
	char *home;
	char pe[1024];

	home = getenv("HOME");

	if (!home)
	  {
		  printf("ERROR: Environment variable $HOME was not found.\n"
			 "Try export HOME in a bash-like environment\n"
			 "or setenv HOME in a sh like environment.\n");
		  exit(-1);
	  }

	snprintf(pe, 1024, "%s/.e", home);
	mkdir(pe, 0755);
	snprintf(pe, 1024, "%s/.e/ewl", home);
	mkdir(pe, 0755);
	snprintf(pe, 1024, "%s/.e/ewl/config", home);
	mkdir(pe, 0755);

	c = ewl_get_settings();

	ewl_save_config(c);

	FREE(c->evas.render_method);
	FREE(c->theme.name);
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
	Ewl_Config *nc, oc;

	nc = ewl_get_settings();
	ewl_config_read_config(&oc);

	if ((nc->debug.enable != oc.debug.enable ||
	     nc->debug.level != oc.debug.level ||
	     nc->evas.font_cache != oc.evas.font_cache ||
	     nc->evas.image_cache != oc.evas.image_cache ||
	     strcasecmp(nc->evas.render_method, oc.evas.render_method) ||
	     strcmp(nc->theme.name, oc.theme.name) ||
	     nc->theme.cache != oc.theme.cache || nc->fx.fps != oc.fx.fps) &&
			!confirm.win)
		ewl_config_create_confirm_dialog();
	else
	  {
		  ewl_widget_destroy(e_conf.main_win);

		  ewl_main_quit();
	  }
	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}

void
ewl_config_create_confirm_dialog(void)
{
	confirm.win = ewl_window_new();
	ewl_window_resize(EWL_WINDOW(confirm.win), 473, 264);
	ewl_window_set_min_size(EWL_WINDOW(confirm.win), 473, 264);
	ewl_widget_show(confirm.win);

	confirm.main_vbox = ewl_vbox_new();
	ewl_container_append_child(EWL_CONTAINER(confirm.win),
				   confirm.main_vbox);
	ewl_widget_show(confirm.main_vbox);

	confirm.text = ewl_text_new("You have made modifications, what would you "
			  "like to do ?");
	ewl_object_set_padding(EWL_OBJECT(confirm.text), 0, 0, 20, 20);
	ewl_object_set_alignment(EWL_OBJECT(confirm.text),
				 EWL_ALIGNMENT_CENTER);
	ewl_container_append_child(EWL_CONTAINER(confirm.main_vbox),
				   confirm.text);
	ewl_widget_show(confirm.text);

	confirm.button_hbox = ewl_hbox_new();
	ewl_box_set_spacing(EWL_BOX(confirm.button_hbox), 5);
	ewl_object_set_custom_size(EWL_OBJECT(confirm.button_hbox), 340, 17);
	ewl_object_set_padding(EWL_OBJECT(confirm.button_hbox), 0, 0, 20, 20);
	ewl_object_set_alignment(EWL_OBJECT(confirm.button_hbox),
				 EWL_ALIGNMENT_CENTER);
	ewl_container_append_child(EWL_CONTAINER(confirm.main_vbox),
				   confirm.button_hbox);
	ewl_widget_show(confirm.button_hbox);

	confirm.button_save = ewl_button_new("Save & Exit");
	ewl_object_set_custom_size(EWL_OBJECT(confirm.button_save), 110, 17);
	ewl_container_append_child(EWL_CONTAINER(confirm.button_hbox),
				   confirm.button_save);
	ewl_callback_append(confirm.button_save, EWL_CALLBACK_CLICKED,
			    ewl_config_save_cb, NULL);
	ewl_callback_append(confirm.button_save, EWL_CALLBACK_CLICKED,
			    ewl_config_exit_cb, NULL);
	ewl_widget_show(confirm.button_save);

	confirm.button_exit = ewl_button_new("Exit without saving");
	ewl_object_set_custom_size(EWL_OBJECT(confirm.button_exit), 110, 17);
	ewl_container_append_child(EWL_CONTAINER(confirm.button_hbox),
				   confirm.button_exit);
	ewl_callback_append(confirm.button_exit, EWL_CALLBACK_CLICKED,
			    ewl_config_exit_cb, NULL);
	ewl_widget_show(confirm.button_exit);

	confirm.button_cancel = ewl_button_new("Cancel");
	ewl_object_set_custom_size(EWL_OBJECT(confirm.button_cancel), 110,
				   17);
	ewl_container_append_child(EWL_CONTAINER(confirm.button_hbox),
				   confirm.button_cancel);
	ewl_callback_append(confirm.button_cancel, EWL_CALLBACK_CLICKED,
			    ewl_config_destroy_confirm_dialog, NULL);
	ewl_widget_show(confirm.button_cancel);
}

void
ewl_config_destroy_confirm_dialog(Ewl_Widget * w, void *ev_data,
				  void *user_data)
{
	if (confirm.win)
		ewl_widget_destroy(confirm.win);

	confirm.win = NULL;

	return;
	w = NULL;
	ev_data = NULL;
	user_data = NULL;
}
