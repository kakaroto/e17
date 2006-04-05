#ifdef E_TYPEDEFS
#else
#ifndef CALENDAR_FUNC_H
#define CALENDAR_FUNC_H
#include "e_mod_main.h"
void fill_in_caltable(int FirstDay, Calendar_Face *face, int StartUp);
int calculate_skew(Calendar *calendar);
void calendar_add_dates(Calendar_Face *face, Calendar *calendar, int skew);
#endif
#endif
