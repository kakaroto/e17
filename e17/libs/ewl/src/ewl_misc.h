#ifndef __EWL_MISC_H__
#define __EWL_MISC_H__

/**
 * @defgroup Ewl_Misc Misc: Miscellaneous Utility Functions
 * Provides important miscellaneous functionality such as manipulation of the
 * main loop.
 */

typedef struct Ewl_Options Ewl_Options;

struct Ewl_Options
{
	int             debug_level;
	char           *xdisplay;
};

void            ewl_init(int *argc, char **argv);
void            ewl_shutdown(void);
void            ewl_main(void);
void            ewl_main_quit(void);
void            ewl_configure_request(Ewl_Widget * w);
void            ewl_configure_queue(void);
void            ewl_configure_cancel_request(Ewl_Widget *w);
void            ewl_realize_request(Ewl_Widget *w);
void            ewl_realize_queue(void);
void            ewl_destroy_request(Ewl_Widget *w);
void            ewl_garbage_collect(void);
void            ewl_enter_realize_phase(void);
void            ewl_exit_realize_phase(void);
int             ewl_in_realize_phase(void);
void            ewl_evas_destroy(Evas *evas);
void            ewl_evas_object_destroy(Evas_Object *obj);

/* #define DEBUG_MALLOCDEBUG 1 */
#ifdef DEBUG_MALLOCDEBUG
char *strdup(const char *str);
#endif

#endif				/* __EWL_MISC_H__ */
