/* state.h - ewl global state wrapped in a struct for future expansion */

#ifndef _STATE_H_
#define _STATE_H_

#include "includes.h"
#include "debug.h"
#include "ll.h"
#include "ewldb.h"
#include "option.h"
#include "event.h"
#include "widget.h"
#include "translators.h"
#include "theme.h"

/* used in ewl_event_propagate -- defined in ewlconteiner.h */
extern void ewl_container_event_propagate(EwlWidget  *container,
                                          EwlEvent   *ev);


/* placeholder -- just 'till i write the timer code :) */
typedef void                                EwlTimer;


typedef struct _EwlState                    EwlState;
static EwlState                            *_Ewl_Internal_State;

struct _EwlState
{
	char                       *application_name;
	char                       *theme_name;

	EwlLL                      *path_list;

	unsigned char               debug;
	/* eventually the widget registry will go here instaed */
	/*EwlRegistryEntry           *static_registry;*/
	unsigned int                num_static_registry_entries;
	EwlLL                      *dynamic_registry;
	
	EwlLL                      *widget_list;
	EwlWidget                  *grabbed;
	EwlWidget                  *focused;

	EwlEvent                   *event_queue;
	EwlBool                     events_pending; /* depricated */
	EwlBool                     quit_flag;
	
	EwlTimer                   *timer_queue;

	EwlEventTranslator         *translators[EWL_MAX_EVENT_TRANSLATORS];
	
	Display                    *disp;
	EwlBool                     render_dithered;
	EwlBool                     render_antialiased;
};

EwlState  *ewl_state_new();
void       ewl_state_init(EwlState *s);
EwlState  *ewl_state_dup(EwlState *s);
void       ewl_state_free(EwlState *s);

EwlState  *ewl_state_get();
void       ewl_state_set(EwlState *s);

/******************************************/
/* USER INITIALIZATION AND MAIN FUNCTIONS */
/******************************************/
void       ewl_init(int *argc, char ***argv);
EwlBool    ewl_main_iteration();
void       ewl_quit();
void       ewl_main();


/*******************************/
/* HIGH-LEVEL WIDGET FUNCTIONS */
/*******************************/
void       ewl_widget_show(EwlWidget *widget);
void       ewl_widget_hide(EwlWidget *widget);
EwlWidget *ewl_widget_grabbed_get();
EwlWidget *ewl_widget_focused_get();
void       ewl_widget_grab(EwlWidget *w);
void       ewl_widget_focus(EwlWidget *w);
void       ewl_widget_ungrab(EwlWidget *w);
void       ewl_widget_unfocus(EwlWidget *w);
void       ewl_widget_add(EwlWidget *widget);


/******************************/
/* HIGH-LEVEL EVENT FUNCTIONS */
/******************************/
void       ewl_event_queue(EwlEvent *ev);
EwlBool    ewl_events_pending();


/******************************/
/* HIGH-LEVEL TIMER FUNCTIONS */
/******************************/
EwlBool    ewl_timers_pending();
EwlTimer  *ewl_timer_handle(EwlTimer *t);


/****************************************/
/* MISC LIBRARY-WIDE PARAMTER FUNCTIONS */
/****************************************/
void       ewl_set_application_name(char *name);
char      *ewl_get_application_name();

void       ewl_set_theme(char *name);
char      *ewl_get_theme();

void       ewl_insert_path(char *path);
void       ewl_remove_path(char *path);
EwlLL     *ewl_get_path_list();
char     **ewl_get_path_strings(int *length);

void       ewl_render_antialiased_set(EwlBool a);
void       ewl_render_dithered_set(EwlBool d);
EwlBool    ewl_render_antialiased_get();
EwlBool    ewl_render_dithered_get();

void       ewl_set_display(Display *disp);
Display   *ewl_get_display();


/*********************/
/* PRIVATE FUNCTIONS */
/*********************/
void       ewl_x_init();

void       ewl_state_set_application_name(char *name);
char      *ewl_state_get_application_name();

void       ewl_state_set_theme(char *name);
char      *ewl_state_get_theme();

void       ewl_state_insert_path(char *path);
void       ewl_state_remove_path(char *path);
EwlLL     *ewl_state_get_path_list();
char     **ewl_state_get_path_strings(int *length);

EwlBool    ewl_state_render_antialiased_get();
EwlBool    ewl_state_render_dithered_get();
void       ewl_state_render_antialiased_set(EwlBool a);
void       ewl_state_render_dithered_set(EwlBool d);

void       ewl_state_set_display(Display *disp);
Display   *ewl_state_get_display();

/* char       ewl_state_dynreg_insert(); */
/* char       ewl_state_dynreg_remove(); */
/* int        ewl_state_dynreg_count();  */

EwlBool    ewl_state_widget_add(EwlWidget *widget);
EwlBool    ewl_state_widget_remove(EwlWidget *widget);
int        ewl_state_widget_count();

/* event functions */
EwlBool    ewl_state_event_add(EwlEvent *event);
EwlBool    ewl_state_event_remove(EwlEvent *event);
int        ewl_state_event_count();

EwlEvent  *ewl_event_translate(EwlEvent *queue, XEvent *xev);
void       ewl_event_propagate(EwlEvent *ev);
EwlEvent  *ewl_event_handle(EwlEvent *ev);

void       ewl_state_translators_init(EwlState *s);

/* make -pedantic shut up about my static global var */
static void __depricated_die_pedantic_die_Ewl_Internal_State()
{
	if (_Ewl_Internal_State) fprintf(stderr,"DON'T CALL THIS FUNCTION!!\n");
	if (0) __depricated_die_pedantic_die_Ewl_Internal_State();
	return;
}

#endif /* _STATE_H_ */
