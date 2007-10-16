/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#ifndef _EX_OPTIONS_H
#define _EX_OPTIONS_H

int             _ex_options_init(void);
int             _ex_options_shutdown(void);
Ex_Options     *_ex_options_new(void);
void            _ex_options_default(Exhibit *o);
void            _ex_options_free(Exhibit *e);
int             _ex_options_save(Exhibit *e);
int             _ex_options_load(Exhibit *e);
void            _ex_options_window_show(void);

#endif
