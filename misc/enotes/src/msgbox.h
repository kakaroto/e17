
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


#ifndef MSGBOX_H
#define MSGBOX_H 1

#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ewl.h>

#include "debug.h"
#include "config.h"

typedef struct {
	Ewl_Widget     *win;
	Ewl_Widget     *vbox;
	Ewl_Widget     *hbox;
	Ewl_Widget     *okbtn;
	Ewl_Widget     *msg;
} MsgBox;

extern MainConfig *main_config;

/* Making a Message Box */
void            msgbox(char *title, char *content);

/* Callbacks */
void            msgbox_okbtn_clicked(Ewl_Widget * widget, void *ev_data,
				     void *data);
void            msgbox_close(MsgBox * p);

#endif
