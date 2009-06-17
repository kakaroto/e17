#include "Elapse.h"

int elapse_time_set(Elapse *elapse)
{
    struct 	tm *ts;
    time_t	ctime;
    char	str[PATH_MAX];

    ctime = time(NULL);
    ts = localtime(&ctime);
    if (elapse->conf.format) {
        strftime(str, PATH_MAX, elapse->conf.format, ts);
    } else {
        strncpy(str, asctime(ts), PATH_MAX);
        str[strlen(str)-1] = '\0';
    }

    edje_object_part_text_set(elapse->edje, "time", str);

    return 1;
}
