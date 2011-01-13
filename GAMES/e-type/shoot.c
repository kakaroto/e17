#include "config.h"

#include <Evas.h>
#include <Ecore.h>
#include <Edje.h>

#include "game.h"
#include "alien.h"
#include "ship.h"
#include "shoot.h"


struct _Shoot
{
  Game *g;
  Evas_Object *o;
  Shoot_Dir dir;
  Evas_Coord hot_x;
  Evas_Coord hot_y;
};


static Eina_Bool
_shoot_cb(void *data)
{
  Shoot *s;
  Evas_Coord x;
  Evas_Coord y;
  Evas_Coord w;
  Evas_Coord h;
  Evas_Coord gw;
  Evas_Coord gh;

  if (!data) return EINA_FALSE;

  s = (Shoot *)data;
  game_size_get(s->g, &gw, &gh);

  if (s->dir == SHOOT_DIR_UP_SHIP)
    {
      evas_object_geometry_get(s->o, &x, &y, NULL, &h);
      y--;
      evas_object_move(s->o, x, y);

      /* collision avec alien */
      {
        Eina_List *aliens;
        Eina_List *l = NULL;
        Alien *alien;

        aliens = game_aliens_get(s->g);
        EINA_LIST_FOREACH(aliens, l, alien)
          {
            int status;

            status = alien_explode(alien, x + s->hot_x, y + s->hot_y);
            if (status >= 0)
              {
                printf("touche %p\n", alien);
                game_shoot_remove(s->g, s);
                shoot_free(s);
                return EINA_FALSE;
              }
          }
      }

      if ((y + h) < 0)
        {
          game_shoot_remove(s->g, s);
          shoot_free(s);
          return EINA_FALSE;
        }
    }

  if (s->dir == SHOOT_DIR_DOWN_ALIEN)
    {
      Ship *ship;

      evas_object_geometry_get(s->o, &x, &y, NULL, &h);
      y++;
      evas_object_move(s->o, x, y);

      /* collision with the ship */
      ship = game_ship_get(s->g);
      if (ship_explode(ship, x + s->hot_x, y + s->hot_y))
        {
          printf("touche vaisseau\n");
          game_shoot_remove(s->g, s);
          shoot_free(s);
          return EINA_FALSE;
        }

      if (y >= gh)
        {
          game_shoot_remove(s->g, s);
          shoot_free(s);
          return EINA_FALSE;
        }
    }

  if (s->dir == SHOOT_DIR_UP)
    {
      evas_object_geometry_get(s->o, &x, &y, NULL, &h);
      y--;
      evas_object_move(s->o, x, y);

      if ((y + h) < 0)
        {
          shoot_free(s);
          return EINA_FALSE;
        }
    }

  if (s->dir == SHOOT_DIR_DOWN)
    {
      evas_object_geometry_get(s->o, &x, &y, NULL, NULL);
      y++;
      evas_object_move(s->o, x, y);

      if (y >= gh)
        {
          shoot_free(s);
          return EINA_FALSE;
        }
    }

  if (s->dir == SHOOT_DIR_LEFT)
    {
      evas_object_geometry_get(s->o, &x, &y, &w, NULL);
      x--;
      evas_object_move(s->o, x, y);

      if ((x + w) < 0)
        {
          shoot_free(s);
          return EINA_FALSE;
        }
    }

  if (s->dir == SHOOT_DIR_RIGHT)
    {
      evas_object_geometry_get(s->o, &x, &y, NULL, NULL);
      x++;
      evas_object_move(s->o, x, y);

      if (x >= gw)
        {
          shoot_free(s);
          return EINA_FALSE;
        }
    }

  if (s->dir == SHOOT_DIR_TOP_LEFT)
    {
      evas_object_geometry_get(s->o, &x, &y, &w, &h);
      x--;
      y--;
      evas_object_move(s->o, x, y);

      if (((x + w) < 0) || ((y + h) < 0))
        {
          shoot_free(s);
          return EINA_FALSE;
        }
    }

  if (s->dir == SHOOT_DIR_TOP_RIGHT)
    {
      evas_object_geometry_get(s->o, &x, &y, NULL, &h);
      x++;
      y--;
      evas_object_move(s->o, x, y);

      if ((x >= gw) || ((y + h) < 0))
        {
          shoot_free(s);
          return EINA_FALSE;
        }
    }

  if (s->dir == SHOOT_DIR_BOTTOM_LEFT)
    {
      evas_object_geometry_get(s->o, &x, &y, NULL, &h);
      x--;
      y++;
      evas_object_move(s->o, x, y);

      if (((x + w) < 0) || (y >= gh))
        {
          shoot_free(s);
          return EINA_FALSE;
        }
    }

  if (s->dir == SHOOT_DIR_BOTTOM_RIGHT)
    {
      evas_object_geometry_get(s->o, &x, &y, NULL, &h);
      x++;
      y++;
      evas_object_move(s->o, x, y);

      if ((x >= gw) || (y >= gh))
        {
          shoot_free(s);
          return EINA_FALSE;
        }
    }

  return EINA_TRUE;
}

Shoot *
shoot_new(Game *g, Shoot_Dir dir, Evas_Coord *w, Evas_Coord *h)
{
  Shoot *s;

  if (!g) return NULL;

  if (!w || !h)
    {
      printf("ERROR : parameters must be non NULL !!!\n");
      return NULL;
    }

  if (dir >= SHOOT_LAST)
    return NULL;

  s = (Shoot *)calloc(1, sizeof(Shoot));
  if (!s) return NULL;

  s->g = g;
  s->dir = dir;

  s->o = edje_object_add(game_evas_get(g));
  if (dir == SHOOT_DIR_UP_SHIP)
    {
      edje_object_file_set(s->o, "theme.edj", "shoot_ship1");
      s->hot_x = 2;
      s->hot_y = 0;
    }

  if (dir == SHOOT_DIR_DOWN_ALIEN)
    {
      edje_object_file_set(s->o, "theme.edj", "shoot_alien1");
      s->hot_x = 2;
      s->hot_y = 10;
    }

  if ((dir == SHOOT_DIR_UP) || (dir == SHOOT_DIR_DOWN))
    {
      edje_object_file_set(s->o, "theme.edj", "shoot_v");
      s->hot_x = 4;
      s->hot_y = 0;
    }

  if ((dir == SHOOT_DIR_LEFT) || (dir == SHOOT_DIR_RIGHT))
    edje_object_file_set(s->o, "theme.edj", "shoot_h");

  if ((dir == SHOOT_DIR_TOP_LEFT) || (dir == SHOOT_DIR_BOTTOM_RIGHT))
    edje_object_file_set(s->o, "theme.edj", "shoot_tl");

  if ((dir == SHOOT_DIR_TOP_RIGHT) || (dir == SHOOT_DIR_BOTTOM_LEFT))
    edje_object_file_set(s->o, "theme.edj", "shoot_tr");

  edje_object_size_min_get(s->o, w, h);
  evas_object_resize(s->o, *w, *h);
  evas_object_layer_set(s->o, 11);

  game_shoot_append(g, s);

  return s;
}

void
shoot_free(Shoot *s)
{
  if (!s) return;

  evas_object_del(s->o);
  free(s);
}

void
shoot_start(Shoot *s, Evas_Coord x, Evas_Coord y, double fps)
{
  if (!s) return;

  evas_object_move(s->o, x, y);
  evas_object_show(s->o);

  ecore_timer_add(fps, _shoot_cb, s);
}
