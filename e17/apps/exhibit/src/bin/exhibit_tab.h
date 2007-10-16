/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#ifndef _EX_TAB_H
#define _EX_TAB_H

Ex_Tab *_ex_tab_new(Exhibit *e, char *dir);
void    _ex_tab_delete(void);    
void    _ex_tab_select(Ex_Tab *tab);
void    _ex_tab_current_zoom_in(Exhibit *e);
void    _ex_tab_current_zoom_out(Exhibit *e);
void    _ex_tab_current_zoom_one_to_one(Exhibit *e);
void    _ex_tab_current_fit_to_window(Exhibit *e);
Ex_Tab *_ex_tab_find_by_itree(Etk_Tree *itree);
#endif
