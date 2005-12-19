#ifdef E_TYPEDEFS
#else
#ifndef CALENDAR_FUNC_H
#define CALENDAR_FUNC_H
#include "e_mod_main.h"
EAPI void  fill_in_caltable(int FirstDay, Calendar_Face *face, int StartUp);
EAPI int   calculate_skew(Calendar *calendar);
EAPI void  calendar_add_dates(Calendar_Face *face, Calendar *calendar, int skew);
#endif
#endif
