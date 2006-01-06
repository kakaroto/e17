#ifndef _EX_OPTIONS_H
#define _EX_OPTIONS_H

Ex_Options     *_ex_options_new();
void            _ex_options_free(Exhibit *e);
void            _ex_options_save(Exhibit *e);
void            _ex_options_load(Exhibit *e);
void            _ex_options_window_show(Exhibit *e);
Etk_Bool        _ex_options_window_delete_cb(void *data);
  
#endif
