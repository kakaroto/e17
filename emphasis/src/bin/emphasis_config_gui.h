#ifndef EMPHASIS_CONFIG_GUI_H_
#define EMPHASIS_CONFIG_GUI_H_

#define EMPHASIS_CONFIG_GLADE "/config.glade"

typedef struct _Emphasis_Config_Gui Emphasis_Config_Gui;
struct _Emphasis_Config_Gui
{
  Enhance *en;

  Etk_Widget *window;
  Etk_Widget *hostname;
  Etk_Widget *password;
  Etk_Widget *port;

  Etk_Widget *xfade;
  Etk_Widget *stop;
  Etk_Widget *aspect;

  void *data;
};

void emphasis_init_configgui   (Emphasis_Config_Gui *configgui);
void emphasis_configgui_autoset(Emphasis_Config_Gui *configgui);
void emphasis_configgui_save   (Emphasis_Config_Gui *configgui);
;
#endif /* EMPHASIS_CONFIG_GUI_H_ */
