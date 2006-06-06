#ifndef _CONFIG_H_
#define _CONFIG_H_
/**
 * @defgroup config
 * @{
 */

typedef struct _Emphasis_Config {
	const char *user_home_path;
	const char *file_path;
	const char *hostname;
	int port;
	const char *password;
	int crossfade;
	
	struct {
		int x;
		int y;
		int w;
		int h;
	} geometry;

	enum {
		EMPHASIS_PLAYER,
		EMPHASIS_PLAYLIST,
		EMPHASIS_FULL
	} display_mode;
		
} Emphasis_Config;

struct _Emphasis_Config_Gui {
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
	
void config_write(Emphasis_Config *config);
Emphasis_Config *config_load(void);
void config_str_change(Emphasis_Config *config, const char **str, const char *value);
void config_gui_init(Emphasis_Config_Gui *gui);
void config_gui_set(Emphasis_Config_Gui *gui, Emphasis_Config *config);
Emphasis_Config *config_new(void);
 /** @} */
#endif /*_CONFIG_H_*/
