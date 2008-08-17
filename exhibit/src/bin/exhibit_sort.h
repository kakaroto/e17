/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#ifndef _EX_SORT_H
#define _EX_SORT_H

void       _ex_sort_label_mouse_down_cb(Etk_Object *object, void *event_info, void *data);
Etk_Bool       _ex_sort_name_cb(Etk_Object *obj, void *data);
Etk_Bool       _ex_sort_size_cb(Etk_Object *obj, void *data);
Etk_Bool       _ex_sort_date_cb(Etk_Object *obj, void *data);
Etk_Bool       _ex_sort_resol_cb(Etk_Object *obj, void *data);
int        _ex_sort_cmp_name(const void *p1, const void *p2);
int        _ex_sort_cmp_date(const void *p1, const void *p2);
int        _ex_sort_cmp_size(const void *p1, const void *p2);
int        _ex_sort_cmp_resol(const void *p1, const void *p2);
  
#endif
