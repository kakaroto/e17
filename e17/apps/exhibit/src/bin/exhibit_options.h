#ifndef _EX_OPTIONS_H
#define _EX_OPTIONS_H

Ex_Options     *_ex_options_new();
Ex_Options     *_ex_options_default(Ex_Options *o);
void            _ex_options_free(Exhibit *e);
void            _ex_options_save(Exhibit *e);
Ex_Options     *_ex_options_load(Ex_Options *e);
void            _ex_options_window_show(Exhibit *e);
Etk_Bool        _ex_options_window_delete_cb(void *data);
  
#endif
