#ifdef E_TYPEDEFS
#else /* 
 */
#ifndef CALENDAR_TEXT_FUNC_H
#define CALENDAR_TEXT_FUNC_H
#include "e_mod_main.h"
void                calendar_face_set_text(Calendar * calendar);

void               calendar_update_text_fonts(Calendar * calendar,
                                               Calendar_Face * face);

void               set_day_label(Evas_Object * label, int FirstDay, int month,
                                  int year);

void               free_Calfonts(Calendar * calendar);


#endif /* 
 */
#endif /* 
 */
    


