#ifndef _EW_TEXTLIST_H
#define _EW_TEXTLIST_H

#include "_ew_list.h"

Entrance_List ew_textlist_new(const char*, int, int, int, int);
void ew_textlist_add(Entrance_List, const char *, void *, size_t, void (*) (void*), void*);


#endif
