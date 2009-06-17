#ifndef EMPHASIS_COVER_DB
#define EMPHASIS_COVER_DB

void emphasis_cover_init(Emphasis_Gui *gui);
void *emphasis_cover_thread_process(void *data);
char *emphasis_cover_get(char *artist, char *album);
char *emphasis_cover_db_search(const char *artist, const char *album);
char *emphasis_cover_db_add(const char *artist, const char *album,
                            void *cover, int size);
#endif /* EMPHASIS_COVER_DB */
