#ifndef _EFL_EXPLOSION_H_
#define _EFL_EXPLOSION_H_


#include <Evas.h>

#include "forward.h"


Explosion *explosion_new(Evas *evas,
                         const char *name,
                         void *data);
Eina_Bool  explosion_launch(Explosion *e, Evas_Coord hot_x, Evas_Coord hot_y);
void       explosion_free(Explosion *e);


#endif /* _EFL_EXPLOSION_H_ */
