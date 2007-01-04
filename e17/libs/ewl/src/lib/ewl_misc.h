/* vim: set sw=8 ts=8 sts=8 noexpandtab: */
#ifndef EWL_MISC_H
#define EWL_MISC_H

/**
 * @addtogroup Ewl_Misc Ewl_Misc: Miscellaneous Utility Functions
 * Provides important miscellaneous functionality such as manipulation of the
 * main loop.
 *
 * @{
 */

int 	 ewl_init(int *argc, char **argv);
void 	 ewl_print_help(void);
int 	 ewl_shutdown(void);
void 	 ewl_main(void);
void 	 ewl_main_quit(void);
void 	 ewl_configure_request(Ewl_Widget *w);
void 	 ewl_realize_request(Ewl_Widget *w);
void 	 ewl_realize_cancel_request(Ewl_Widget *w);
void 	 ewl_destroy_request(Ewl_Widget *w);
void 	 ewl_evas_destroy(Evas *evas);
void  	 ewl_evas_object_destroy(Evas_Object *obj);
char  	*ewl_debug_indent_get(int mod_dir);

/*
 * Internal stuff
 */
typedef void (*Ewl_Shutdown_Hook)(void);
void 	 ewl_shutdown_add(Ewl_Shutdown_Hook hook);

#undef DEBUG_MALLOCDEBUG
#ifdef DEBUG_MALLOCDEBUG
char *strdup(const char *str);
#endif

/**
 * @}
 */

#endif
