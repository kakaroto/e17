#ifndef _ENNA_UTIL_H_
#define _ENNA_UTIL_H_

#define MMAX(a,b) ((a) > (b) ? (a) : (b))
#define MMIN(a,b) ((a) > (b) ? (b) : (a))

EAPI char *enna_util_user_home_get();
EAPI int enna_util_has_suffix(char *str, Evas_List * patterns);
EAPI unsigned int enna_util_calculate_font_size(Evas_Coord w, Evas_Coord h);
EAPI void enna_util_switch_objects(Evas_Object * container, Evas_Object * obj1,
        Evas_Object * obj2);

#endif
