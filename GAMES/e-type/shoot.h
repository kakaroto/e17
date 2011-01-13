#ifndef _EFL_SHOOT_H_
#define _EFL_SHOOT_H_


#include <Evas.h>

#include "forward.h"
#include "game.h"


typedef enum
{
  SHOOT_DIR_UP,
  SHOOT_DIR_DOWN,
  SHOOT_DIR_LEFT,
  SHOOT_DIR_RIGHT,
  SHOOT_DIR_TOP_LEFT,
  SHOOT_DIR_TOP_RIGHT,
  SHOOT_DIR_BOTTOM_LEFT,
  SHOOT_DIR_BOTTOM_RIGHT,
  SHOOT_DIR_UP_SHIP,
  SHOOT_DIR_DOWN_ALIEN,
  SHOOT_LAST
} Shoot_Dir;


Shoot *shoot_new(Game *g, Shoot_Dir dir, Evas_Coord *w, Evas_Coord *h);
void   shoot_free(Shoot *s);
void   shoot_start(Shoot *s, Evas_Coord x, Evas_Coord y, double fps);


#endif /* _EFL_SHOOT_H_ */
