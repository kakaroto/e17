#ifndef EWL_MISC_H
#define EWL_MISC_H

/**
 * @defgroup Ewl_Misc Ewl_Misc: Miscellaneous Utility Functions
 * Provides important miscellaneous functionality such as manipulation of the
 * main loop.
 *
 * @{
 */

int             ewl_init(int *argc, char **argv);
void            ewl_print_help(void);
int             ewl_shutdown(void);
void            ewl_main(void);
void            ewl_main_quit(void);
void            ewl_configure_request(Ewl_Widget *w);
void            ewl_configure_queue(void);
void            ewl_configure_cancel_request(Ewl_Widget *w);
void            ewl_realize_request(Ewl_Widget *w);
void            ewl_realize_cancel_request(Ewl_Widget *w);
void            ewl_realize_queue(void);
void            ewl_destroy_request(Ewl_Widget *w);
int             ewl_garbage_collect_idler(void *data);
void            ewl_realize_phase_enter(void);
void            ewl_realize_phase_exit(void);
int             ewl_in_realize_phase(void);
unsigned int    ewl_engine_mask_get(void);
void            ewl_evas_destroy(Evas *evas);
void            ewl_evas_object_destroy(Evas_Object *obj);
char 	       *ewl_debug_get_indent(void);

#undef DEBUG_MALLOCDEBUG
#ifdef DEBUG_MALLOCDEBUG
char *strdup(const char *str);
#endif

/**
 * @}
 */

#endif
