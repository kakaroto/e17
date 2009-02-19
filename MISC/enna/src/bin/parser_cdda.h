#ifndef PARSER_CDDA_H
#define PARSER_CDDA_H

typedef struct cdda_track_s {
  unsigned int min;
  unsigned int sec;
  unsigned int frame;
  char *name;
} cdda_track_t;

typedef struct cdda_s {
  unsigned long id;
  char *artist;
  char *title;
  char *ext_data;
  char *genre;
  int year;
  int length;
  unsigned int total_tracks;
  cdda_track_t **tracks;
} cdda_t;

cdda_t *parse_cdda (const char *device);
void cdda_free (cdda_t *c);

#endif /* PARSER_CDDA_H */
