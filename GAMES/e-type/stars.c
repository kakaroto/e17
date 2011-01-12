#include "config.h"

#include <stdlib.h>

#include <Evas.h>
#include <Ecore.h>
#include <Edje.h>

#include "game.h"
#include "stars.h"


struct _Star
{
  Game *g;
  Evas_Object *o;
  double interval;
  Ecore_Timer *timer;
};


static Star star_init(Game *g, Eina_Bool start);

static Eina_Bool
_star_cb(void *data)
{
  Star *star = (Star *)data;
  Evas_Coord x;
  Evas_Coord y;
  Evas_Coord gh;

  game_size_get(star->g, NULL, &gh);
  evas_object_geometry_get(star->o, &x, &y, NULL, NULL);
  evas_object_move(star->o, x, ++y);

  if (y > gh)
    {
      Star tmp;

      tmp = star_init(star->g, EINA_FALSE);

      evas_object_del(star->o);
      star->o = tmp.o;
      star->interval = tmp.interval;
      ecore_timer_interval_set(star->timer, star->interval);
    }

  return EINA_TRUE;
}

static Star
star_init(Game *g, Eina_Bool start)
{
  Star s;
  char buf[10];
  Evas_Object *o;
  Evas_Coord sx;
  Evas_Coord sy = 0;
  Evas_Coord gw;
  Evas_Coord gh;
  double r;
  int type;

  s.g = g;
  game_size_get(g, &gw, &gh);

  r = rand();
  if (r < (RAND_MAX / 4))
    type = 0;
  else if (r < (RAND_MAX / 2))
    type = 1;
  else if (r < (3 * (RAND_MAX / 4)))
    type = 2;
  else
    type = 3;

  r = (double)rand();
  sx = (Evas_Coord)((r * (double)gw) / RAND_MAX);

  if (start)
    {
      r = (double)rand();
      sy = (Evas_Coord)((r * (double)gh) / RAND_MAX);
    }

  sprintf(buf, "star%d", type);
  o = edje_object_add(game_evas_get(g));
  if (!edje_object_file_set(o, "theme.edj", buf))
    printf("merde\n");
  evas_object_move (o, sx, sy);
  evas_object_layer_set(o, type - 5);
  evas_object_show (o);

  s.o = o;

  s.interval = atof(edje_object_data_get(o, "interval"));

  return s;
}


Star *
stars_new(Game *g, int nbr)
{
  Star *stars;
  int i;

  if (nbr <= 0) return NULL;

  stars = (Star *)calloc(nbr, sizeof(Star));
  if (!stars) return NULL;

  for (i = 0; i < nbr; i++)
    {
      stars[i] = star_init(g, EINA_TRUE);
      stars[i].timer = ecore_timer_add(stars[i].interval, _star_cb, stars + i);
    }

  return stars;
}

void
stars_free(Star *stars, int nbr)
{
  int i;

  if (!stars) return;

  for (i = 0; i < nbr; i++)
    {
      ecore_timer_del(stars[i].timer);
      evas_object_del(stars[i].o);
    }
  free(stars);
}
