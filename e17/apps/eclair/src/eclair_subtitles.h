#ifndef _ECLAIR_SUBTITLES_H_
#define _ECLAIR_SUBTITLES_H_

#include <Evas.h>
#include "eclair_types.h"

struct _Eclair_Subtitle
{
   double start, end;
   char *subtitle;
};

struct _Eclair_Subtitles
{
   Evas_List *subtitles;
   Evas_Bool enable;
};

void eclair_subtitles_init(Eclair_Subtitles *subtitles);
void eclair_subtitles_free(Eclair_Subtitles *subtitles);
char *eclair_subtitles_get_current_subtitle(Eclair_Subtitles *subtitles, double current_time);
void eclair_subtitles_display_current_subtitle(Eclair_Subtitles *subtitles, double current_time, Evas_Object *subtitles_text);
void eclair_subtitles_sort(Eclair_Subtitles *subtitles);
Evas_Bool eclair_subtitles_load_from_media_file(Eclair_Subtitles *subtitles, const char *media_file);
Evas_Bool eclair_subtitles_load_srt(Eclair_Subtitles *subtitles, const char *path);

#endif
