#include "erss.h"

typedef enum {
  ERSS_PARSE_SUCC=0,
  ERSS_PARSE_FAIL=-1,
  ERSS_PARSE_EMPTY=-2
} erss_parse_error;

int erss_parse (Erss_Feed *);
int erss_parse_free (Erss_Feed *);
