#include "Etox_private.h"
#include "Etox.h"

Etox_Obstacle
etox_obstacle_add(Etox e, double x, double y, double w, double h)
{
  Etox_Obstacle obst;

  if (!e || !w || !h)
    return NULL;

  if (!e->obstacles)
    {
      e->obstacles = ewd_list_new();
      ewd_list_set_free_cb(e->obstacles, EWD_FREE_CB(free));
    }

  obst = malloc(sizeof(struct _Etox_Obstacle));
  obst->x = x;
  obst->y = y;
  obst->w = w;
  obst->h = h;

  ewd_list_append(e->obstacles, obst);

  e->etox_objects.dirty = 1;
  _etox_update(e);

  return obst;
}

void
etox_obstacle_set(Etox e, Etox_Obstacle obst,
		  double x, double y, double w, double h)
{
  if (!e || !obst || !w || !h)
    return;

  obst->x = x;
  obst->y = y;
  obst->w = w;
  obst->h = h;

  e->etox_objects.dirty = 1;
  _etox_update(e);
}

void
etox_obstacle_del(Etox e, Etox_Obstacle obst)
{
  if (!e || !obst)
    return;

  if (ewd_list_goto(e->obstacles, obst))
    ewd_list_remove(e->obstacles);
  FREE(obst);

  e->etox_objects.dirty = 1;
  _etox_update(e);
}
