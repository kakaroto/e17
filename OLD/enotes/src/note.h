
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
#include <Ecore_X.h>
#include <Edje.h>
#include <Ewl.h>

#include "../config.h"
#include "debug.h"
#include "config.h"
#include "controlcentre.h"
#include "saveload.h"
#include "ipc.h"
#include "menu.h"
#include "msgbox.h"


#define NOTE_PART "Main"

#define EDJE_SIGNAL_NOTE_CLOSE "ENOTES_QUIT"
#define EDJE_SIGNAL_NOTE_MINIMISE "ENOTES_MINIMIZE"
#define EDJE_SIGNAL_NOTE_SAVE "ENOTES_SAVE"
#define EDJE_SIGNAL_NOTE_SHADE "ENOTES_SHADE"
#define EDJE_SIGNAL_NOTE_UNSHADE "ENOTES_UNSHADE"

#define EDJE_CONTAINER "EnoteContainer"
#define EDJE_TEXT_USER "EnoteUser"
#define EDJE_TEXT_DATE "EnoteDate"
#define EDJE_TEXT_TITLE "EnoteTitle"

#define EDJE_VSCROLLBAR_BTN_INCR "vscroll-up"
#define EDJE_VSCROLLBAR_BTN_DECR "vscroll-down"
#define EDJE_VSCROLLBAR_SEEKER "vtrough"

#define EDJE_HSCROLLBAR_BTN_INCR "hscroll-right"
#define EDJE_HSCROLLBAR_BTN_DECR "hscroll-left"
#define EDJE_HSCROLLBAR_SEEKER "htrough"

#define EDJE_SCROLLPANE "scrollpane"
#define EDJE_SCROLLBAR_BUTTON "scroll-button"

#define EDJE_INFO_SCROLLBARS "EnoteInfoScrollbars"
#define EDJE_INFO_FONTNAME "EnoteInfoFontName"
#define EDJE_INFO_FONTSIZE "EnoteInfoFontSize"
#define EDJE_INFO_FONTSTYLE "EnoteInfoFontStyle"

#define NOTE_SAVED_SIGNAL "ENOTES,SAVED"
#define NOTE_LOADED_SIGNAL "ENOTES,LOADED"

#define COMPARE_INTERVAL 0.01
#define TITLE_LENGTH 20

#define DEF_CONTENT "Edit me. :-)\nYou know you want to!"

typedef struct _note Note;
typedef struct _note {
	/* Widgets */
	Ecore_Evas     *win;
	Evas           *evas;
	Evas_Object    *edje;
	Evas_Object    *dragger;
	Evas_Object    *eventer;
	int             shaded;

	Evas_Object    *eo;
	Ewl_Widget     *emb;
	Ewl_Widget     *content;
	Ewl_Widget     *pane;

	Ewl_Widget     *dialog;
	Menu           *menu;

	/* Comparison Strings and Timer */
	Ecore_Timer    *timcomp;
	char           *txt_title;

	/* Animation Vars */
	int             stage;
} _note;

Evas_List      *gbl_notes;

/* High Level */
void            new_note(void);
void            new_note_with_values(int x, int y, int width, int height,
				     int shaded, char *content);
Note           *new_note_with_values_return(int x, int y, int width, int height,
					    int shaded, char *content);

/* Lists and Allocation */
Evas_List      *append_note(void);
void            remove_note(Evas_List * note);

/* GUI Setup */
void            setup_note(Evas_List ** note, int x, int y, int width,
			   int height, int shaded, char *content);
void            configure_scrollbars(Ewl_Widget * pane, char *edjefn);
void            configure_scrollbars_default(Ewl_Widget * pane);

/* Menu Callbacks */
void            cb_menu_rightclick(Note * p, Evas * e, Evas_Object * obj,
				   void *ev_info);
void            cb_ewl_new_note(void *data);

/* Ecore Callbacks */
void            note_ecore_close(Ecore_Evas * ee);
void            note_ecore_resize(Ecore_Evas * ee);

/* Edje Callbacks */
void            note_edje_close(Evas_List * note, Evas_Object * o,
				const char *emission, const char *source);
void            note_edje_minimise(Evas_List * note, Evas_Object * o,
				   const char *emission, const char *source);
void            note_edje_save(Evas_List * note, Evas_Object * o,
			       const char *emission, const char *source);
void            note_edje_shade(Evas_List * note, Evas_Object * o,
				const char *emission, const char *source);
void            note_edje_unshade(Evas_List * note, Evas_Object * o,
				  const char *emission, const char *source);

/* Misc */
char           *get_date_string(void);
int             note_edje_close_timer(void *p);
int             timer_val_compare(void *data);
void            note_move_embed(Ewl_Widget * w, void *ev_data, void *user_data);
void            notes_update_themes(void);


/* External Interaction */
int             get_note_count();

Evas_List      *get_note_by_title(char *title);

char           *get_title_by_note(Evas_List * note);
char           *get_title_by_note_struct(Note * note);
char           *get_content_by_note(Evas_List * note);
char           *get_content_by_note_struct(Note * note);
char           *get_title_by_content(char *content);

Evas_List      *get_cycle_begin(void);
Evas_List      *get_cycle_next_note(Evas_List * note);
Evas_List      *get_cycle_previous_note(Evas_List * note);

void            update_enote_title(Evas_Object * edje, char *content);

#endif
