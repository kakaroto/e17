#include "config.h"

#include <Eina.h>
#include <Evas.h>
#include <Ecore.h>
#include <Edje.h>

#include "game.h"
#include "ship.h"
#include "powerup.h"


struct _Powerup
{
  Game *g;
  Evas_Object *o;
  double time_start;
};


static Eina_Bool
_power_move_cb(void *data)
{
  Eina_Rectangle r1;
  Eina_Rectangle r2;
  Powerup *pu = (Powerup *)data;
  Ship *ship;
  double time_current;
  Evas_Coord x;
  Evas_Coord y;
  Evas_Coord w;
  Evas_Coord h;
  Evas_Coord sx;
  Evas_Coord sy;
  Evas_Coord sw;
  Evas_Coord sh;
  Evas_Coord gh;

  evas_object_geometry_get(pu->o, &x, &y, &w, &h);
  time_current = ecore_time_get();
  if ((time_current - pu->time_start) >= 2.0)
    {
      y++;
    }
  else
    {
      /* FIXME:
       * find an algo that move the powerup toward the
       * center of the screen, but a bit randomly (like
       * xenon 2)
       */
    }
  evas_object_move(pu->o, x, y);

  game_size_get(pu->g, NULL, &gh);

  ship = game_ship_get(pu->g);
  ship_geometry_get(ship, &sx, &sy, &sw, &sh);
  r1.x = x;
  r1.y = y;
  r1.w = w;
  r1.h = h;
  r2.x = sx;
  r2.y = sy;
  r2.w = sw;
  r2.h = sh;
  if (eina_rectangles_intersect(&r1, &r2) || (y >= gh))
    {
      /* FIXME: improve ship powerups */

      printf("collision : %p\n", data);
      game_powerup_remove(pu->g, pu);
      powerup_free(pu);
      return EINA_FALSE;
    }

  return EINA_TRUE;
}


Powerup *
powerup_new(Game *g, Powerup_Type type, Evas_Coord x, Evas_Coord y)
{
  Powerup *pu;
  const char *group;
  Evas_Coord w;
  Evas_Coord h;

  switch (type)
    {
    case POWERUP_TYPE_NONE:
      return NULL;
    case POWERUP_TYPE_1:
      group = "crystal1";
      break;
    default:
      group = "crystal1";
      break;
    }

  pu = (Powerup *)calloc(1, sizeof(Powerup));
  if (!pu) return NULL;

  pu->o = edje_object_add(game_evas_get(g));
  edje_object_file_set(pu->o, "theme.edj", group);
  edje_object_size_max_get(pu->o, &w, &h);
  evas_object_move(pu->o, x - w / 2, y - h / 2);
  evas_object_resize(pu->o, w, h);
  evas_object_show(pu->o);
  edje_object_signal_emit(pu->o, "start", "");

  ecore_timer_add(0.005, _power_move_cb, pu);
  pu->time_start = ecore_time_get();
  pu->g = g;
  game_powerup_append(g, pu);

  return pu;
}

void
powerup_free(Powerup *pu)
{
  if (!pu)  return;

  evas_object_del(pu->o);
  free(pu);
}
