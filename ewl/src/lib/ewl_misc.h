/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_MISC_H
#define EWL_MISC_H

/**
 * @addtogroup Ewl_Misc Ewl_Misc: Miscellaneous Utility Functions
 * Provides important miscellaneous functionality such as manipulation of the
 * main loop.
 *
 * @{
 */

/**
 * @internal
 * Shutdown callback typedef
 */
typedef void (*Ewl_Shutdown_Hook)(void);

int      ewl_init(int *argc, char **argv);
void     ewl_print_help(void);
int      ewl_shutdown(void);
void     ewl_main(void);
void     ewl_main_iterate(void);
void     ewl_main_quit(void);
void     ewl_configure_request(Ewl_Widget *w);
void     ewl_realize_request(Ewl_Widget *w);
void     ewl_realize_cancel_request(Ewl_Widget *w);
void     ewl_destroy_request(Ewl_Widget *w);
void     ewl_canvas_destroy(void *evas);
void     ewl_canvas_object_destroy(void *obj);
void     ewl_debug_indent_print(int mod_dir);
void     ewl_shutdown_add(Ewl_Shutdown_Hook hook);

/*
 * Internal stuff
 */
#undef DEBUG_MALLOCDEBUG
#ifdef DEBUG_MALLOCDEBUG
char *strdup(const char *str);
#endif

/**
 * @}
 */

#endif
