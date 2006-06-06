#include "emphasis.h"
#include "emphasis_config.h"


void 
config_write(Emphasis_Config *config)
{
	char *str_port, *str_x, *str_y, *str_w, *str_h;
	char *config_dir;
	Eet_File *ef;
	
	config_dir = ecore_file_get_dir(config->file_path);
	if (!ecore_file_exists(config_dir))
		ecore_file_mkdir(config_dir);
	else if (!ecore_file_is_dir(config_dir))
	{
		printf("Error %s is not a directory\n", config_dir);
		return;
	}

	asprintf(&str_port, "%i", config->port);
	asprintf(&str_x, "%i", config->geometry.x);
	asprintf(&str_y, "%i", config->geometry.y);
	asprintf(&str_w, "%i", config->geometry.w);
	asprintf(&str_h, "%i", config->geometry.h);
	ef = eet_open(config->file_path, EET_FILE_MODE_WRITE);
	if (ef)
	{
		eet_write(ef, "Hostname", config->hostname, strlen(config->hostname)+1, 0);
		eet_write(ef, "Port", str_port, strlen(str_port)+1, 0);
		if (config->password)
			eet_write(ef, "Password", config->password, strlen(config->password)+1, 0);
		eet_write(ef, "x", str_x, sizeof(int), 0);
		eet_write(ef, "y", str_y, sizeof(int), 0);
		eet_write(ef, "w", str_w, sizeof(int), 0);
		eet_write(ef, "h", str_h, sizeof(int), 0);
		eet_close(ef);
	}
	free(str_port);
	free(str_x);
	free(str_y);
	free(str_w);
	free(str_h);
}

/**
 * @brief Load the config file
 * @return The data set into the config file or the default parameters for a mpd connection
 */
Emphasis_Config *
config_load(void)
{
	Emphasis_Config *config;
	Eet_File *ef;
	char *hostname, *password, *file_path;
	
	config = config_new();
	
	ef = eet_open(config->file_path, EET_FILE_MODE_READ);
	if (ef)
	{
		hostname = eet_read(ef, "Hostname", 0);
		config->port = atoi(eet_read(ef, "Port", 0));
		password = eet_read(ef, "Password", 0);
		config->geometry.x = atoi(eet_read(ef, "x", 0));
		config->geometry.y = atoi(eet_read(ef, "y", 0));
		config->geometry.w = atoi(eet_read(ef, "w", 0));
		config->geometry.h = atoi(eet_read(ef, "h", 0));
		eet_close(ef);
		
		config_str_change(config, &(config->hostname), hostname);
		config_str_change(config, &(config->password), password);
	}
	
	return config;
}

Emphasis_Config *
config_new(void)
{
	Emphasis_Config *config;
	char *hostname, *password, *file_path;
	
	config = malloc(sizeof(Emphasis_Config));
	
	config_str_change(config, &(config->user_home_path), strdup(getenv("HOME")));
	asprintf(&file_path, "%s/.e/emphasis/emphasis.cfg", config->user_home_path); 
	config->file_path = strdup(file_path);
	config->hostname = strdup("localhost");
	config->port = 6600;
	config->password = NULL;
	config->crossfade = 0;
	config->geometry.x = 0;
	config->geometry.y = 0;
	config->geometry.w = 500;
	config->geometry.h = 400;
	
	return config;
}
	
void
config_str_change(Emphasis_Config *config, const char **str, const char *value)
{
	if (!value)
		return;

	*str = strdup(value);
}

void
config_gui_init(Emphasis_Config_Gui *gui)
{
	gui->window = etk_window_new();
	etk_window_title_set(ETK_WINDOW(gui->window),"Emphasis Configuration");
	etk_window_wmclass_set(ETK_WINDOW(gui->window), "Emphasis Configuration", "emphasis");
	etk_container_border_width_set(ETK_CONTAINER(gui->window), 2);
	
	gui->vbox = etk_vbox_new(ETK_FALSE, 0);
	etk_container_add(ETK_CONTAINER(gui->window), gui->vbox);
	
	gui->connection_frame = etk_frame_new("Connection settings");
	etk_box_pack_start(ETK_BOX(gui->vbox), gui->connection_frame, ETK_TRUE, ETK_TRUE, 0);
	
	gui->connection_vbox = etk_vbox_new(ETK_TRUE, 5);
	etk_container_add(ETK_CONTAINER(gui->connection_frame), gui->connection_vbox);
	
	gui->hostname_hbox = etk_hbox_new(ETK_TRUE, 0);
	gui->hostname_label = etk_label_new("Hostname :");
	gui->hostname_entry = etk_entry_new();
	etk_box_pack_start(ETK_BOX(gui->hostname_hbox), gui->hostname_label, ETK_TRUE, ETK_TRUE, 0);
	etk_box_pack_start(ETK_BOX(gui->hostname_hbox), gui->hostname_entry, ETK_TRUE, ETK_TRUE, 0);
	etk_box_pack_start(ETK_BOX(gui->connection_vbox), gui->hostname_hbox, ETK_TRUE, ETK_TRUE, 5);
	
	gui->port_hbox = etk_hbox_new(ETK_TRUE, 0);
	gui->port_label = etk_label_new("Port :");
//	gui->port_spin = etk_spin_button_new(0, 65536, 1);
	etk_box_pack_start(ETK_BOX(gui->port_hbox), gui->port_label, ETK_TRUE, ETK_TRUE, 0);
	etk_box_pack_start(ETK_BOX(gui->port_hbox), gui->port_spin, ETK_TRUE, ETK_TRUE, 0);
	etk_box_pack_start(ETK_BOX(gui->connection_vbox), gui->port_hbox, ETK_TRUE, ETK_TRUE, 5);
	
	gui->password_hbox = etk_hbox_new(ETK_TRUE, 0);
	gui->password_label = etk_label_new("password :");
	gui->password_entry = etk_entry_new();
	etk_entry_password_set(ETK_ENTRY(gui->password_entry), ETK_TRUE);
	etk_box_pack_start(ETK_BOX(gui->password_hbox), gui->password_label, ETK_TRUE, ETK_TRUE, 0);
	etk_box_pack_start(ETK_BOX(gui->password_hbox), gui->password_entry, ETK_TRUE, ETK_TRUE, 0);
	etk_box_pack_start(ETK_BOX(gui->connection_vbox), gui->password_hbox, ETK_TRUE, ETK_TRUE, 5);
	
	/**/
	gui->interface_frame = etk_frame_new("Interface settings");
	etk_box_pack_start(ETK_BOX(gui->vbox), gui->interface_frame, ETK_TRUE, ETK_TRUE, 0);
	
	gui->interface_vbox = etk_vbox_new(ETK_TRUE, 5);
	etk_container_add(ETK_CONTAINER(gui->interface_frame), gui->interface_vbox);
	
	gui->rowheight_hbox = etk_hbox_new(ETK_TRUE, 0);
	gui->rowheight_label = etk_label_new("Row Height :");
//	gui->rowheight_spin = etk_spin_button_new(0, 50, 1);
	etk_box_pack_start(ETK_BOX(gui->rowheight_hbox), gui->rowheight_label, ETK_TRUE, ETK_TRUE, 0);
	etk_box_pack_start(ETK_BOX(gui->rowheight_hbox), gui->rowheight_spin, ETK_TRUE, ETK_TRUE, 0);
	etk_box_pack_start(ETK_BOX(gui->interface_vbox), gui->rowheight_hbox, ETK_TRUE, ETK_TRUE, 5);
	/**/
	
	gui->buttons_box_hbox = etk_hbox_new(ETK_TRUE, 5);
	gui->buttons_box_ok = etk_button_new_with_label("Ok");
	gui->buttons_box_apply = etk_button_new_with_label("Apply");
	gui->buttons_box_cancel = etk_button_new_with_label("Cancel");
	etk_box_pack_start(ETK_BOX(gui->buttons_box_hbox), gui->buttons_box_ok, ETK_TRUE, ETK_FALSE, 0);
	etk_box_pack_start(ETK_BOX(gui->buttons_box_hbox), gui->buttons_box_apply, ETK_TRUE, ETK_FALSE, 0);
	etk_box_pack_start(ETK_BOX(gui->buttons_box_hbox), gui->buttons_box_cancel, ETK_TRUE, ETK_FALSE, 0);
	etk_box_pack_start(ETK_BOX(gui->vbox), gui->buttons_box_hbox, ETK_FALSE, ETK_FALSE, 0);
}

void
config_gui_set(Emphasis_Config_Gui *gui, Emphasis_Config *config)
{
	char *port;
	etk_entry_text_set(ETK_ENTRY(gui->hostname_entry), config->hostname);
	asprintf(&port, "%d", config->port);
	etk_entry_text_set(ETK_ENTRY(gui->port_spin), port);
	if (config->password)
		etk_entry_text_set(ETK_ENTRY(gui->password_entry), config->password);
}
