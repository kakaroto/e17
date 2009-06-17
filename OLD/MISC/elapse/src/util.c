#include "Elapse.h"

void debug(int level, Elapse *elapse, char *message)
{
    if (level <= elapse->conf.debug ) {
        printf("Debug [%d/%d] %s\n",
               elapse->conf.debug, level, message);
    }
}
