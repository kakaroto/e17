
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


#ifndef NOTE_H
#define NOTE_H 1

#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Edje.h>
#include <Ewl.h>

#include "debug.h"
#include "config.h"
#include "saveload.h"
#include "ipc.h"
#include "../config.h"


#define NOTE_EDJE "%s/data/themes/%s.eet"
#define NOTE_PART "Main"

#define EDJE_SIGNAL_NOTE_CLOSE "ENOTES_QUIT"
#define EDJE_SIGNAL_NOTE_MINIMISE "ENOTES_MINIMIZE"
#define EDJE_EWL_CONTAINER "EnotesContainer"

#define COMPARE_INTERVAL 0.01

#define DEF_TITLE "New Note"
#define DEF_CONTENT "Edit me. :-)\nYou know you want to!"

typedef struct _note Note;
typedef struct _note {
	/* Widgets */
	Ecore_Evas     *win;
	Evas           *evas;
	Evas_Object    *edje;
	Evas_Object    *dragger;

	Evas_Object    *eo;
	Ewl_Widget     *emb;
	Ewl_Widget     *vbox;
	Ewl_Widget     *title;
	Ewl_Widget     *content;

	Ewl_Row        *saveload_row;

	/* Comparison Strings and Timer */
	Ecore_Timer    *timcomp;
	char           *txt_title;
} _note;

Evas_List      *gbl_notes;

/* High Level */
void            new_note(void);
void            new_note_with_values(int width, int height, char *title,
				     char *content);

/* Lists and Allocation */
Evas_List      *append_note(void);
void            remove_note(Evas_List * note);

/* GUI Setup */
void            setup_note(Evas_List ** note, int width, int height,
			   char *title, char *content);

/* Ecore Callbacks */
void            note_ecore_close(Ecore_Evas * ee);
void            note_ecore_resize(Ecore_Evas * ee);

/* Edje Callbacks */
void            note_edje_close(Evas_List * note, Evas_Object * o,
				const char *emission, const char *source);
void            note_edje_minimise(Evas_List * note, Evas_Object * o,
				   const char *emission, const char *source);

/* Misc */
char           *get_date_string(void);
int             note_edje_close_timer(void *p);
int             timer_val_compare(void *data);


/* External Interaction */
Evas_List      *get_note_by_title(char *title);
Evas_List      *get_note_by_content(char *content);

char           *get_title_by_note(Evas_List * note);
char           *get_content_by_note(Evas_List * note);

Evas_List      *get_cycle_begin(void);
Evas_List      *get_cycle_next_note(Evas_List * note);
Evas_List      *get_cycle_previous_note(Evas_List * note);

#endif
