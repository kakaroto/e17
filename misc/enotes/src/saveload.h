
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


#ifndef SAVELOAD_HH
#define SAVELOAD_HH 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ewl.h>

#include "note.h"
#include "storage.h"
#include "debug.h"
#include "config.h"
#include "../config.h"

#define SAVELOAD_X 0
#define SAVELOAD_Y 0
#define SAVELOAD_W 450
#define SAVELOAD_H 350

#define LOAD_X 0
#define LOAD_Y 0
#define LOAD_W 450
#define LOAD_H 350

#define MAX_TITLE 500

typedef struct {
	Ecore_Evas     *win;
	Evas           *evas;
	Evas_Object    *eo;
	Ewl_Widget     *emb;
	Ewl_Widget     *vbox;
	Ewl_Widget     *txt_selected;
	Ewl_Widget     *tree;
	Ewl_Widget     *hbox;
	Ewl_Widget     *savebtn;
	Ewl_Widget     *loadbtn;
	Ewl_Widget     *refreshbtn;
	Ewl_Widget     *closebtn;
} SaveLoad;

typedef struct {
	Ecore_Evas     *win;
	Evas           *evas;
	Evas_Object    *eo;
	Ewl_Widget     *emb;
	Ewl_Widget     *vbox;
	Ewl_Widget     *txt_selected;
	Ewl_Widget     *tree;
	Ewl_Widget     *hbox;
	Ewl_Widget     *loadbtn;
	Ewl_Widget     *refreshbtn;
	Ewl_Widget     *deletebtn;
	Ewl_Widget     *closebtn;
} Load;

extern MainConfig *main_config;


/** SAVE/LOAD **/
void            setup_saveload(void);
void            setup_saveload_win(void);
void            saveload_setup_button(Ewl_Widget * c, Ewl_Widget ** b,
				      char *label);
void            fill_saveload_tree(void);
void            setup_saveload_opt(Ewl_Widget * tree, char *caption);
void            ecore_saveload_resize(Ecore_Evas * ee);
void            ecore_saveload_close(Ecore_Evas * ee);
void            ewl_saveload_revert(Ewl_Widget * widget, void *ev_data,
				    Ewl_Widget * p);
void            ewl_saveload_close(Ewl_Widget * o, void *ev_data,
				   Ecore_Evas * ee);
void            ewl_saveload_load(Ewl_Widget * o, void *ev_data, void *null);
void            ewl_saveload_save(Ewl_Widget * o, void *ev_data, void *null);
void            ewl_saveload_listitem_click(Ewl_Widget * o, void *ev_data,
					    void *null);


/** LOAD **/
void            setup_load(void);
void            setup_load_win(void);
void            load_setup_button(Ewl_Widget * c, Ewl_Widget ** b, char *label);
void            fill_load_tree(void);
void            setup_load_opt(Ewl_Widget * tree, char *caption);
void            ecore_load_resize(Ecore_Evas * ee);
void            ecore_load_close(Ecore_Evas * ee);
void            ewl_load_revert(Ewl_Widget * widget, void *ev_data,
				Ewl_Widget * p);
void            ewl_load_close(Ewl_Widget * o, void *ev_data, Ecore_Evas * ee);
void            ewl_load_load(Ewl_Widget * o, void *ev_data, void *null);
void            ewl_load_delete(Ewl_Widget * o, void *ev_data, void *null);
void            ewl_load_listitem_click(Ewl_Widget * o, void *ev_data,
					void *null);


#endif
