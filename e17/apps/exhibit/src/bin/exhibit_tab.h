/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#ifndef _EX_TAB_H
#define _EX_TAB_H

Ex_Tab        *_ex_tab_new(Exhibit *e, char *dir);
void           _ex_tab_dir_set(Ex_Tab *tab, char *dir);
void           _ex_tab_delete(Ex_Tab *tab);    
void           _ex_tab_select(Ex_Tab *tab);
    

#endif
