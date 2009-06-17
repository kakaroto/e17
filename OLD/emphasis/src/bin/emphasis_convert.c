#include "emphasis.h"
#include "emphasis_convert.h"

Emphasis_Song *
convert_mpd_song(mpd_Song * src)
{
  Emphasis_Song *dest;

  if (!src)
    return NULL;

  dest = malloc(sizeof(Emphasis_Song));
  if(!dest) return NULL;

  dest->file     = strdupnull(src->file);
  dest->artist   = strdupnull(src->artist);
  dest->title    = strdupnull(src->title);
  dest->album    = strdupnull(src->album);
  dest->track    = strdupnull(src->track);
  dest->name     = strdupnull(src->name);
  dest->date     = strdupnull(src->date);
  dest->genre    = strdupnull(src->genre);
  dest->composer = strdupnull(src->composer);

  dest->time     = src->time;
  dest->pos      = src->pos;
  dest->id       = src->id;

  return dest;
}


Eina_List *
convert_mpd_data(MpdData * src)
{
  int loop = 1;
  Eina_List *dest = NULL;
  Emphasis_Data *data;
  MpdData *first;

  if (!src)
    return NULL;

  first = src;
  while (loop)
    {

      data = emphasis_data_new();
      switch (src->type)
        {
        case MPD_DATA_TYPE_NONE:
          /* TODO something ? */
          break;
        case MPD_DATA_TYPE_TAG:
          data->type = EMPHASIS_DATA_TYPE_TAG;
          data->tag  = strdupnull(src->tag);
          break;
        case MPD_DATA_TYPE_DIRECTORY:
          data->type = EMPHASIS_DATA_TYPE_DIRECTORY;
          data->directory = strdupnull(src->directory);
          break;
        case MPD_DATA_TYPE_SONG:
          data->type = EMPHASIS_DATA_TYPE_SONG;
          data->song = convert_mpd_song(src->song);
          break;
        case MPD_DATA_TYPE_PLAYLIST:
          data->type = EMPHASIS_DATA_TYPE_PLAYLIST;
          data->playlist = strdupnull(src->playlist);
          break;
        case MPD_DATA_TYPE_OUTPUT_DEV:
          /* TODO something ? */
          break;
        }
      dest = eina_list_append(dest, data);

      if (mpd_data_is_last(src))
        {
          loop = 0;
        }
      else
        {
          src = mpd_data_get_next(src);
        }
    }
  src = first;

  return dest;
}
