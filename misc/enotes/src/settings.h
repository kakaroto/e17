
/**************************************************
 **               E  -  N O T E S                **
 **                                              **
 **  The contents of this file are released to   **
 **  the public under the General Public Licence **
 **  Version 2.                                  **
 **                                              **
 **  By  Thomas Fletcher (www.fletch.vze.com)    **
 **                                              **
 **************************************************/


#ifndef SETTINGS_HH
#define SETTINGS_HH 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ewl.h>

#include "debug.h"
#include "config.h"

#define SETTINGS_X 0
#define SETTINGS_Y 0
#define SETTINGS_W 450
#define SETTINGS_H 350

typedef struct {
	Ewl_Widget     *caption;
	Ewl_Widget     *entry;
} Settings_Opt;

typedef struct {
	Ecore_Evas     *win;
	Evas           *evas;
	Evas_Object    *eo;
	Ewl_Widget     *emb;
	Ewl_Widget     *vbox;
	Ewl_Widget     *tree;
	Ewl_Widget     *hbox;
	Ewl_Widget     *savebtn;
	Ewl_Widget     *revertbtn;
	Ewl_Widget     *closebtn;
	Settings_Opt    render_method, theme, intro, debug, cc;
} Settings;

extern MainConfig *main_config;

/* High Level */
void            setup_settings(void);

/* Setting up the Window */
void            setup_settings_win(Settings * s);
void            fill_tree(void);
void            settings_setup_button(Ewl_Widget * c, Ewl_Widget ** b,
				      char *label);

/* Setting up the Options */
Settings_Opt    setup_settings_opt(Ewl_Widget * tree, char *caption,
				   char *value);
Settings_Opt    setup_settings_opt_int(Ewl_Widget * tree,
				       char *caption, int value);

/* Callbacks */
void            ecore_settings_resize(Ecore_Evas * ee);
void            ecore_settings_close(Ecore_Evas * ee);
void            ewl_settings_revert(Ewl_Widget * widget, void *ev_data,
				    Ewl_Widget * p);
void            ewl_settings_close(Ewl_Widget * o, void *ev_data,
				   Ecore_Evas * ee);
void            ewl_settings_save(Ewl_Widget * o, void *ev_data, void *data);

/* XML */
void            save_settings(void);

#endif
