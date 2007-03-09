#include "emphasis.h"
#include "emphasis_data_list.h" 

Emphasis_Data *
emphasis_data_new(void)
{
  Emphasis_Data *data;

  data = malloc(sizeof(Emphasis_Data));
  if(!data) return NULL;
  data->song = NULL;
  data->tag = NULL;
  data->directory = NULL;
  data->playlist = NULL;

  return data;
}

void
print_evas_list_stats(Evas_List *list)
{
  Evas_List *p;
  p = list;
  Emphasis_Data *data;
  int count = 0;

  while (p)
    {
      data = evas_list_data(p);
      switch (data->type)
        {
        case MPD_DATA_TYPE_NONE:
          printf("MPD_DATA_TYPE_NONE\n");
          break;
        case MPD_DATA_TYPE_TAG:
          printf("MPD_DATA_TYPE_TAG\n");
          break;
        case MPD_DATA_TYPE_DIRECTORY:
          printf("MPD_DATA_TYPE_DIRECTORY\n");
          break;
        case MPD_DATA_TYPE_SONG:
          printf("MPD_DATA_TYPE_SONG\n");
          break;
        case MPD_DATA_TYPE_PLAYLIST:
          printf("MPD_DATA_TYPE_PLAYLIST\n");
          break;
        }
      p = evas_list_next(p);
      count++;
    }
  printf("total numbers of elements : %d\n", count);
}

void
emphasis_song_free(Emphasis_Song *song)
{
#undef  EMP_EL_FREE
#define EMP_EL_FREE(data) if(data) free(data)
  if(song)
    {
      EMP_EL_FREE(song->file    );
      EMP_EL_FREE(song->artist  );
      EMP_EL_FREE(song->title   );
      EMP_EL_FREE(song->album   );
      EMP_EL_FREE(song->track   );
      EMP_EL_FREE(song->name    );
      EMP_EL_FREE(song->date    );
      EMP_EL_FREE(song->genre   );
      EMP_EL_FREE(song->composer);

      free(song);
    }
}

void
emphasis_data_free(Emphasis_Data *data)
{
  if(data)
    {
      if(data->song) { emphasis_song_free(data->song); }
      EMP_EL_FREE(data->tag);
      EMP_EL_FREE(data->playlist);
      EMP_EL_FREE(data->directory);
      free(data);
    }
}

/* TODO check this */
void
emphasis_list_free(Evas_List *list)
{
  Emphasis_Data *data;

  if (!list) return;
  list = evas_list_last(list);
  while (evas_list_prev(list))
    {
      data = evas_list_data(list);
      list = evas_list_prev(list);
      if (!data) continue;
      if (data->song)
        {
          if (data->song->file)     { free(data->song->file);     } 
          if (data->song->artist)   { free(data->song->artist);   }
          if (data->song->title)    { free(data->song->title);    }
          if (data->song->album)    { free(data->song->album);    }
          if (data->song->track)    { free(data->song->track);    }
          if (data->song->name)     { free(data->song->name);     }
          if (data->song->date)     { free(data->song->date);     }
          if (data->song->genre)    { free(data->song->genre);    }
          if (data->song->composer) { free(data->song->composer); }
          free(data->song);
        }
      if (data->tag)       { free(data->tag);       }
      if (data->playlist)  { free(data->playlist);  }
      if (data->directory) { free(data->directory); }
      free(data);
    }
  data = evas_list_data(list);
  free(data);
  list = evas_list_free(list);
}

Evas_List *
emphasis_list_concatenate(Evas_List *head, Evas_List *tail)
{
  Evas_List *list;

  list = evas_list_concatenate(head, tail);
  emphasis_list_free(tail);

  return list;
}

