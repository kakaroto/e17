#include <Engrave.h>

Engrave_Font *
engrave_font_new(char *path, char *name)
{
    Engrave_Font *ef;
    ef = NEW(Engrave_Font, 1);
    ef->name = (name ? strdup(name) : NULL);
    ef->path = (path ? strdup(path) : NULL);
    return ef;
}


