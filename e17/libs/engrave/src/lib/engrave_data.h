#ifndef ENGRAVE_DATA_H
#define ENGRAVE_DATA_H

typedef struct _Engrave_Data Engrave_Data;
struct _Engrave_Data
{
  char *key;
  char *value;
  int int_value;
};

Engrave_Data * engrave_data_new(char *key, char *value);

#endif

