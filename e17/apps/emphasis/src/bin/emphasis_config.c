#include "emphasis.h"
#include "emphasis_config.h"

void
set_defaults (void)
{
	ecore_config_string_default(MPD_HOSTNAME_KEY, "localhost");
	ecore_config_int_default(MPD_PORT_KEY, 6600);
	ecore_config_string_default(MPD_PASSWORD_KEY, NULL);
	ecore_config_int_default(MPD_CROSSFADE_KEY, 0);

	ecore_config_int_default(EMP_GEOMETRY_X_KEY, 0);
	ecore_config_int_default(EMP_GEOMETRY_Y_KEY, 0);
	ecore_config_int_default(EMP_GEOMETRY_W_KEY, 500);
	ecore_config_int_default(EMP_GEOMETRY_H_KEY, 400);

	ecore_config_int_default(EMP_MODE_KEY, EMPHASIS_FULL);
}

Emphasis_Config *
config_load(void)
{
	Emphasis_Config *config;

	config = malloc(sizeof(Emphasis_Config));
	set_defaults();
	ecore_config_load();

	config->hostname   = ecore_config_string_get(MPD_HOSTNAME_KEY);
	config->port       = ecore_config_int_get(MPD_PORT_KEY);
	config->password   = ecore_config_string_get(MPD_PASSWORD_KEY);
	config->crossfade  = ecore_config_int_get(MPD_CROSSFADE_KEY);

	config->geometry.x = ecore_config_int_get(EMP_GEOMETRY_X_KEY);
	config->geometry.y = ecore_config_int_get(EMP_GEOMETRY_Y_KEY);
	config->geometry.w = ecore_config_int_get(EMP_GEOMETRY_W_KEY);
	config->geometry.h = ecore_config_int_get(EMP_GEOMETRY_H_KEY);

	config->mode       = ecore_config_int_get(EMP_MODE_KEY);

	return config;
}

void
config_save(Emphasis_Config *config)
{
	ecore_config_string_set(MPD_HOSTNAME_KEY, config->hostname);
	ecore_config_int_set(MPD_PORT_KEY, config->port);
	ecore_config_string_set(MPD_PASSWORD_KEY, config->password);
	ecore_config_int_set(MPD_CROSSFADE_KEY, config->crossfade);
	                                          
	ecore_config_int_set(EMP_GEOMETRY_X_KEY, config->geometry.x);
	ecore_config_int_set(EMP_GEOMETRY_Y_KEY, config->geometry.y);
	ecore_config_int_set(EMP_GEOMETRY_W_KEY, config->geometry.w);
	ecore_config_int_set(EMP_GEOMETRY_H_KEY, config->geometry.h);

	ecore_config_int_set(EMP_MODE_KEY, config->mode);

	ecore_config_save();
}

Emphasis_Config_Gui *
config_gui_init(void)
{
	Emphasis_Config_Gui *gui;
	gui = malloc(sizeof(Emphasis_Config_Gui));

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
//	etk_box_pack_start(ETK_BOX(gui->port_hbox), gui->port_spin, ETK_TRUE, ETK_TRUE, 0);
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
//	etk_box_pack_start(ETK_BOX(gui->rowheight_hbox), gui->rowheight_spin, ETK_TRUE, ETK_TRUE, 0);
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

	return gui;
}

void
config_gui_set(Emphasis_Config_Gui *gui, Emphasis_Config *config)
{
//	char *port;
	etk_entry_text_set(ETK_ENTRY(gui->hostname_entry), config->hostname);
//	asprintf(&port, "%d", config->port);
//	etk_entry_text_set(ETK_ENTRY(gui->port_spin), port);
	if (config->password != NULL && strlen(config->password) != 0)
		etk_entry_text_set(ETK_ENTRY(gui->password_entry), config->password);
}

void
config_free(Emphasis_Config *config)
{
	if (config->hostname) { free(config->hostname); }
	if (config->password) { free(config->password); }
	free(config);
}
