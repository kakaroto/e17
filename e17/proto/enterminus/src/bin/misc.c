#include "term.h"

static void strupper(char *str)
{
   char *i;
   for(i = str; *i != '\0'; i++)
     *i = toupper(*i);
}

int term_timers(void *data) {
   ecore_job_add(term_redraw, data);
   return 1;
}
