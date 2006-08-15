#include "emphasis.h"
#include "emphasis_config_gui.h"

Emphasis_Config_Gui *
config_gui_init(void)
{
  Emphasis_Config_Gui *gui;
  gui = malloc(sizeof(Emphasis_Config_Gui));

  gui->window = etk_window_new();
  etk_window_title_set(ETK_WINDOW(gui->window), "Emphasis Configuration");
  etk_window_wmclass_set(ETK_WINDOW(gui->window), 
                         "Emphasis Configuration", "emphasis");
  etk_container_border_width_set(ETK_CONTAINER(gui->window), 2);

  gui->vbox = etk_vbox_new(ETK_FALSE, 0);
  etk_container_add(ETK_CONTAINER(gui->window), gui->vbox);

  gui->connection_frame = etk_frame_new("Connection settings");
  etk_box_append(ETK_BOX(gui->vbox), gui->connection_frame, 
                 ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

  gui->connection_vbox = etk_vbox_new(ETK_TRUE, 5);
  etk_container_add(ETK_CONTAINER(gui->connection_frame), gui->connection_vbox);

  gui->hostname_hbox = etk_hbox_new(ETK_TRUE, 0);
  gui->hostname_label = etk_label_new("Hostname :");
  gui->hostname_entry = etk_entry_new();
  etk_box_append(ETK_BOX(gui->hostname_hbox), gui->hostname_label,
                 ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
  etk_box_append(ETK_BOX(gui->hostname_hbox), gui->hostname_entry,
                 ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
  etk_box_append(ETK_BOX(gui->connection_vbox), gui->hostname_hbox,
                 ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

  gui->port_hbox = etk_hbox_new(ETK_TRUE, 0);
  gui->port_label = etk_label_new("Port :");
// gui->port_spin = etk_spin_button_new(0, 65536, 1); 
  etk_box_append(ETK_BOX(gui->port_hbox), gui->port_label, 
                 ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
/* etk_box_append(ETK_BOX(gui->port_hbox), gui->port_spin, 
                 ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0); */
  etk_box_append(ETK_BOX(gui->connection_vbox), gui->port_hbox, 
                 ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

  gui->password_hbox = etk_hbox_new(ETK_TRUE, 0);
  gui->password_label = etk_label_new("password :");
  gui->password_entry = etk_entry_new();
  etk_entry_password_set(ETK_ENTRY(gui->password_entry), ETK_TRUE);
  etk_box_append(ETK_BOX(gui->password_hbox), gui->password_label,
                 ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
  etk_box_append(ETK_BOX(gui->password_hbox), gui->password_entry,
                 ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
  etk_box_append(ETK_BOX(gui->connection_vbox), gui->password_hbox,
                 ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

/**/ 
  gui->interface_frame = etk_frame_new("Interface settings");
  etk_box_append(ETK_BOX(gui->vbox), gui->interface_frame, 
                 ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);

  gui->interface_vbox = etk_vbox_new(ETK_TRUE, 5);
  etk_container_add(ETK_CONTAINER(gui->interface_frame), gui->interface_vbox);

  gui->rowheight_hbox = etk_hbox_new(ETK_TRUE, 0);
  gui->rowheight_label = etk_label_new("Row Height :");
//  gui->rowheight_spin = etk_spin_button_new(0, 50, 1);
  etk_box_append(ETK_BOX(gui->rowheight_hbox), gui->rowheight_label,
                 ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
/* etk_box_append(ETK_BOX(gui->rowheight_hbox), gui->rowheight_spin, 
                 ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0); */
  etk_box_append(ETK_BOX(gui->interface_vbox), gui->rowheight_hbox,
                 ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
/**/ 
  gui->buttons_box_hbox = etk_hbox_new(ETK_TRUE, 5);
  gui->buttons_box_ok = etk_button_new_with_label("Ok");
  gui->buttons_box_apply = etk_button_new_with_label("Apply");
  gui->buttons_box_cancel = etk_button_new_with_label("Cancel");
  etk_box_append(ETK_BOX(gui->buttons_box_hbox), gui->buttons_box_ok,
                 ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
  etk_box_append(ETK_BOX(gui->buttons_box_hbox), gui->buttons_box_apply,
                 ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
  etk_box_append(ETK_BOX(gui->buttons_box_hbox), gui->buttons_box_cancel,
                 ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
  etk_box_append(ETK_BOX(gui->vbox), gui->buttons_box_hbox, 
                 ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
  return gui;
}

void
config_gui_set(Emphasis_Config_Gui *gui, Emphasis_Config *config)
{
//  char *port;
  etk_entry_text_set(ETK_ENTRY(gui->hostname_entry), config->hostname);
//  asprintf(&port, "%d", config->port);
//  etk_entry_text_set(ETK_ENTRY(gui->port_spin), port);
  if (config->password != NULL && strlen(config->password) != 0)
    etk_entry_text_set(ETK_ENTRY(gui->password_entry), config->password);
}

void
cb_config_show(Etk_Object *object, void *data)
{
  UNUSED(object)
  Emphasis_Gui *gui;
  Emphasis_Config *config;

  gui = data;
  gui->config_gui = config_gui_init();
  config = config_load();
  config_gui_set(gui->config_gui, config);
  etk_widget_show_all(gui->config_gui->window);

  etk_signal_connect("clicked", 
                     ETK_OBJECT(gui->config_gui->buttons_box_ok),
                     ETK_CALLBACK(cb_config_write), gui);
  etk_signal_connect("clicked",
                     ETK_OBJECT(gui->config_gui->buttons_box_apply),
                     ETK_CALLBACK(cb_config_write), gui);
  etk_signal_connect("clicked",
                     ETK_OBJECT(gui->config_gui->buttons_box_cancel),
                     ETK_CALLBACK(cb_config_hide), gui->config_gui);
}

void
cb_config_hide(Etk_Object *object, void *data)
{
  UNUSED(object)
  Emphasis_Config_Gui *gui;

  gui = data;
  etk_widget_hide(ETK_WIDGET(gui->window));
}

void
cb_config_write(Etk_Object *object, void *data)
{
  UNUSED(object)
  Emphasis_Config *config;
  Emphasis_Gui *gui;
/*  const char *port; */

  config = config_load();
  gui = data;

  config->hostname =
    (char *) etk_entry_text_get(ETK_ENTRY(gui->config_gui->hostname_entry));
/*  port = etk_entry_text_get(ETK_ENTRY(gui->config_gui->port_spin)); */
/*  config->port = atoi(port); */
  config->password =
    (char *) etk_entry_text_get(ETK_ENTRY(gui->config_gui->password_entry));

  config_save(config);
  cb_config_hide(NULL, gui->config_gui);
  emphasis_try_connect(gui);
}

