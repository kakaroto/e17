#ifndef _EFL_STARS_H_
#define _EFL_STARS_H_


#include "forward.h"
#include "game.h"


Star *stars_new(Game *g, int nbr);
void  stars_free(Star *stars, int nbr);


#endif /* _EFL_STARS_H_ */
