#include <unistd.h>
#include <Eet.h>
#include "emphasis.h"
#include "emphasis_cover_db.h"

void
emphasis_cover_init(Emphasis_Gui *gui)
{
  pthread_t thread;

  pthread_create(&thread, NULL, emphasis_cover_thread_process, gui);
}

void *
emphasis_cover_thread_process(void *data)
{
  Emphasis_Gui *gui;
  Cover_Info *ci;
  char *cover_path;
  char *artist, *album;

  gui = data;
  while (1)
    {
      if (gui->cover_queue)
        {
          ci = gui->cover_queue;

          artist = ci->artist;
          album  = ci->album;
          strescape(artist);
          strescape(album);
          gui->cover_queue = NULL;
          
          cover_path = emphasis_cover_db_search(artist, album);
          emphasis_player_cover_set(gui->player, cover_path);
          if (cover_path != NULL) { free(cover_path); }
          //emphasis_player_cover_size_update(gui->player);

          free(artist);
          free(album);
          free(ci);
        }
      else
        {
          usleep(200);
        }
    }
}

char *
emphasis_cover_db_add(const char *artist, const char *album, 
                      void *cover, int size)
{
  Eet_File *ef;
  FILE *cover_file;
  char *cover_path;
  char *config_path;
  char *key;

  asprintf(&config_path, "%s/.e/apps/emphasis/cover.eet", getenv("HOME"));
  if (access(config_path, F_OK))
    {
      ef = eet_open(config_path, EET_FILE_MODE_WRITE);
      if (!ef)
        {
          fprintf(stderr, "Can't write %s\n", config_path);
          return NULL;
        }
    }
  else
    {
      ef = eet_open(config_path, EET_FILE_MODE_READ_WRITE);
      if (!ef)
        {
          fprintf(stderr, "Can't read write %s\n", config_path);
          return NULL;
        }
    }

  asprintf(&key, "/%s/%s", artist, album);
  if (cover)
    {
      asprintf(&cover_path, "%s/.e/apps/emphasis/%s-%s.jpg", 
               getenv("HOME"), artist, album);
      cover_file = fopen(cover_path, "wb");
      if (!cover_file)
        {
          fprintf(stderr,
                  "Cover: Unable to open the file \"%s\" to save the cover\n",
                  cover_path);
          free(cover_path);
          return NULL;
        }
      fwrite(cover, size, sizeof(char), cover_file);
      fclose(cover_file);
      eet_write(ef, key, cover_path, (strlen(cover_path)+1) * sizeof(char), 0);
      eet_clearcache();
    }
  else
    {
      asprintf(&cover_path, "not found");
      eet_write(ef, key, cover_path, (strlen(cover_path)+1) * sizeof(char), 0);
      eet_clearcache();
      free(cover_path);
      cover_path = NULL;
    }

  eet_close(ef);

  free(config_path);
  free(key);
  free(cover);
  return cover_path;
}


char *
emphasis_cover_db_search(const char *artist, const char *album)
{
  char *config_path;
  char *cover_path = NULL;
  char *key;
  int size;
  void *cover = NULL;
  Eet_File *ef;

  asprintf(&config_path, "%s/.e/apps/emphasis/cover.eet", getenv("HOME"));

  ef = eet_open(config_path, EET_FILE_MODE_READ);

  asprintf(&key, "/%s/%s", artist, album);

  cover_path = eet_read(ef, key, &size);
  eet_close(ef);

  if (!cover_path)
    {
      cover = emphasis_cover_file_get_from_amazon(artist, album, &size);
      cover_path = emphasis_cover_db_add(artist, album, cover, size);
    }
  if (cover_path && !strcmp(cover_path, "not found"))
    {
      free(cover_path);
      cover_path = NULL;
    }

  free(config_path);
  free(key);

  return cover_path;
}
