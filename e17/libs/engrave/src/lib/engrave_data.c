#include <Engrave.h>

Engrave_Data *
engrave_data_new(char *key, char *value)
{
  Engrave_Data *data;
  data = NEW(Engrave_Data, 1);
  data->key = (key ? strdup(key) : NULL);
  data->value = (value ? strdup(value) : NULL);
  return data;
}

