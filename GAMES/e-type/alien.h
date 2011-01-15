#ifndef _EFL_ALIEN_H_
#define _EFL_ALIEN_H_


#include <Evas.h>

#include "forward.h"
#include "game.h"
#include "powerup.h"


typedef enum
{
  ALIEN_TYPE_BLUE,
  ALIEN_TYPE_RED,
  ALIEN_TYPE_GREEN
} Alien_Type;

Alien     *alien_new(Game *g, Alien_Type type, Powerup_Type ptype, Evas_Coord x);
void       alien_free(Alien *a);
void       alien_attack(Alien *a);
int        alien_explode(Alien *a, Evas_Coord hot_x, Evas_Coord hot_y, int power);
Eina_List *alien_shoots_get(Alien *a);


#endif /* _EFL_ALIEN_H_ */
