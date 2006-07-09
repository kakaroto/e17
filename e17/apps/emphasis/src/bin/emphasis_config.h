#ifndef _CONFIG_H_
#define _CONFIG_H_
/**
 * @defgroup config
 * @{
 */

#include <Ecore_Config.h>

#define MPD_HOSTNAME_KEY  "/mpd/hostname"
#define MPD_PORT_KEY      "/mpd/port"
#define MPD_PASSWORD_KEY  "/mpd/password"
#define MPD_CROSSFADE_KEY "/mpd/crossfade" /*TODO handle the crossfade */

#define EMP_GEOMETRY_X_KEY  "/geometry/x"
#define EMP_GEOMETRY_Y_KEY  "/geometry/y"
#define EMP_GEOMETRY_W_KEY  "/geometry/w"
#define EMP_GEOMETRY_H_KEY  "/geometry/h"
#define EMP_MODE_KEY        "/emphasis/mode"

#define EMP_COLWIDTH_TITLE_KEY      "/emphasis/colwidth/title"
#define EMP_COLWIDTH_TIME_KEY       "/emphasis/colwidth/time"
#define EMP_COLWIDTH_ARTIST_KEY     "/emphasis/colwidth/artist"
#define EMP_COLWIDTH_ALBUM_KEY      "/emphasis/colwidth/album"

typedef enum 
{
	EMPHASIS_SMALL=0,
	EMPHASIS_FULL=1
} Emphasis_Mode;

typedef struct _Emphasis_Config 
{
	char *hostname;
	int port;
	char *password;
	int crossfade;
	
	struct {
		int x;
		int y;
		int w;
		int h;
	} geometry;

	struct {
		int title;
		int time;
		int artist;
		int album;
	} colwidth;

	Emphasis_Mode mode;
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
	
void config_save(Emphasis_Config *config);
Emphasis_Config *config_load(void);
void config_str_change(Emphasis_Config *config, const char **str, const char *value);
Emphasis_Config_Gui *config_gui_init(void);
void config_gui_set(Emphasis_Config_Gui *gui, Emphasis_Config *config);
Emphasis_Config *config_new(void);
void config_free(Emphasis_Config *config);
 /** @} */
#endif /*_CONFIG_H_*/
