#ifndef _ENNA_UTIL_H_
#define _ENNA_UTIL_H_

#define MMAX(a,b) ((a) > (b) ? (a) : (b))
#define MMIN(a,b) ((a) > (b) ? (b) : (a))

char         *enna_util_user_home_get();
int           enna_util_has_suffix(char *str, Eina_List * patterns);
unsigned int  enna_util_calculate_font_size(Evas_Coord w, Evas_Coord h);
void          enna_util_switch_objects(Evas_Object * container, Evas_Object * obj1, Evas_Object * obj2);
unsigned char enna_util_uri_has_extension(const char *uri, int type);
#endif
