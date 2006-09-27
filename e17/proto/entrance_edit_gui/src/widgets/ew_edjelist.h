#ifndef _EW_EDJELIST_H
#define _EW_EDJELIST_H

#include "_ew_list.h"

Entrance_List ew_edjelist_new(const char*, int, int, int, int);
void ew_edjelist_add(Entrance_List, const char *, const char *, const char*, void *, size_t, void (*) (void*), void*);


#endif
