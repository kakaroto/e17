#ifndef _EFL_SHIP_H_
#define _EFL_SHIP_H_


#include <Evas.h>

#include "forward.h"
#include "game.h"


typedef enum
{
  SHIP_UPGRADE_FRONT_SINGLE = 1 << 0,
  SHIP_UPGRADE_FRONT_DOUBLE = 1 << 1,
  SHIP_UPGRADE_FRONT_TRIPLE = 1 << 2,
  SHIP_UPGRADE_HORIZ_LEFT   = 1 << 3,
  SHIP_UPGRADE_HORIZ_RIGHT  = 1 << 4,
  SHIP_UPGRADE_SIDE_LEFT    = 1 << 5,
  SHIP_UPGRADE_SIDE_RIGHT   = 1 << 6,
  SHIP_UPGRADE_REAR_SINGLE  = 1 << 7,
  SHIP_UPGRADE_REAR_CIRCLE  = 1 << 8
} Ship_Upgrade;


Ship     *ship_new(Game *g);
void      ship_free(Ship *s);
void      ship_upgrade_set(Ship *s, Ship_Upgrade upg);
void      ship_upgrade_unset(Ship *s, Ship_Upgrade upg);
Eina_Bool ship_explode(Ship *s, Evas_Coord hot_x, Evas_Coord hot_y);
void      ship_geometry_get(Ship *s, Evas_Coord *x, Evas_Coord *y, Evas_Coord *w, Evas_Coord *h);


#endif /* _EFL_SHIP_H_ */
