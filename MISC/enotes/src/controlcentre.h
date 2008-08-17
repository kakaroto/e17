
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


#ifndef CONTROLCENTRE_H
#define CONTROLCENTRE_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_X.h>
#include <Edje.h>
#include <Esmart/Esmart_Container.h>
#include <Esmart/Esmart_Draggies.h>

#include "debug.h"
#include "config.h"
#include "note.h"
#include "../config.h"


/* Note Defaults */
#define NOTE_CONTENT "Fill me in.\nYou know you want to."

/* Part */
#define CC_PART "ControlCenter"
#define CC_EDJE "%s/themes/%s.edj"

/* Callbacks */
#define EDJE_SIGNAL_CC_CLOSE "ENOTES_QUIT"
#define EDJE_SIGNAL_CC_NEW "ENOTES_NOTE_NEW"
#define EDJE_SIGNAL_CC_SAVELOAD "ENOTES_NOTES_SAVELOAD"
#define EDJE_SIGNAL_CC_SETTINGS "ENOTES_SETTINGS"
#define EDJE_SIGNAL_CC_MINIMIZE "ENOTES_CONTROL_MINIMIZE"
#define EDJE_SIGNAL_CC_SHADE "ENOTES_CONTROL_SHADE"
#define EDJE_SIGNAL_CC_UNSHADE "ENOTES_CONTROL_UNSHADE"

/* Configuration */
#define DEF_CC_CONFIG_LOC "%s/.e/notes/cc.xml"

typedef struct {
	Ecore_Evas     *win;
	Evas           *evas;
	Evas_Object    *dragger;
	Evas_Object    *edje;
} ControlCentre;

typedef struct {
	int             x;
	int             y;
	int             width;
	int             height;
	int             shaded;
} CCPos;

extern ControlCentre *controlcentre;
extern MainConfig *main_config;

/* Setting the Control Centre up */
void            setup_cc(void);
void            setup_cc_with_pos(int x, int y);

/* Configuration */
CCPos          *get_cc_pos();
void            set_cc_pos();

/* Callbacks */
void            cc_resize(Ecore_Evas * ee);
void            cc_close_win(Ecore_Evas * ee);
void            cc_close(void *data, Evas_Object * obj, const char *emission,
			 const char *source);
void            cc_saveload(void *data, Evas_Object * obj, const char *emission,
			    const char *source);
void            cc_newnote(void *data, Evas_Object * obj, const char *emission,
			   const char *source);
void            cc_settings(void *data, Evas_Object * obj, const char *emission,
			    const char *source);
void            cc_minimize(void *data, Evas_Object * obj, const char *emission,
			    const char *source);
void            cc_shade(void *data, Evas_Object * obj, const char *emission,
			 const char *source);
void            cc_unshade(void *data, Evas_Object * obj, const char *emission,
			   const char *source);

/*  Theme Change  */
void            cc_update_theme();

#endif
