#ifndef EMPHASIS_CONFIG_GUI_H_
#define EMPHASIS_CONFIG_GUI_H_

struct _Emphasis_Config_Gui
{
  Etk_Widget *window;
  Etk_Widget *vbox;

  Etk_Widget *connection_frame;
  Etk_Widget *connection_vbox;

  Etk_Widget *hostname_hbox;
  Etk_Widget *hostname_label;
  Etk_Widget *hostname_entry;

  Etk_Widget *port_hbox;
  Etk_Widget *port_label;
  Etk_Widget *port_spin;

  Etk_Widget *password_hbox;
  Etk_Widget *password_label;
  Etk_Widget *password_entry;

  Etk_Widget *buttons_box_hbox;
  Etk_Widget *buttons_box_ok;
  Etk_Widget *buttons_box_apply;
  Etk_Widget *buttons_box_cancel;

  Etk_Widget *interface_frame;
  Etk_Widget *interface_vbox;

  Etk_Widget *rowheight_hbox;
  Etk_Widget *rowheight_label;
  Etk_Widget *rowheight_spin;
};

typedef struct _Emphasis_Config_Gui Emphasis_Config_Gui;

Emphasis_Config_Gui *config_gui_init(void);
void config_gui_set(Emphasis_Config_Gui *gui, Emphasis_Config *config);

void cb_config_show(Etk_Object *object, void *data);
void cb_config_hide(Etk_Object *object, void *data);
void cb_config_write(Etk_Object *object, void *data);

#endif /* EMPHASIS_CONFIG_GUI_H_ */
