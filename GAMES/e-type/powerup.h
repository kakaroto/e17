#ifndef _EFL_POWERUP_H_
#define _EFL_POWERUP_H_


#include <Evas.h>

#include "forward.h"
#include "game.h"


typedef enum
{
  POWERUP_TYPE_NONE,
  POWERUP_TYPE_1,
} Powerup_Type;

Powerup *powerup_new(Game *g, Powerup_Type type, Evas_Coord x, Evas_Coord y);
void     powerup_free(Powerup *p);


#endif /* _EFL_POWERUP_H_ */
