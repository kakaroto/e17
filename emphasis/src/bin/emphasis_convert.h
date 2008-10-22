#ifndef EMPHASIS_CONVERT_H_
#define EMPHASIS_CONVERT_H_

Emphasis_Song *convert_mpd_song(mpd_Song * src);
Eina_List     *convert_mpd_data(MpdData * src);

#endif /* EMPHASIS_CONVERT_H */
