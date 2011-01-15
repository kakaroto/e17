#include "config.h"

#include <stdlib.h>

#include <Evas.h>
#include <Ecore.h>
#include <Edje.h>

#include "game.h"
#include "shoot.h"
#include "explosion.h"
#include "powerup.h"
#include "alien.h"


struct _Alien
{
  Game *g;
  Evas_Object *o;
  Alien_Type type;
  Powerup_Type ptype;
  Ecore_Timer *timer_move;
  Ecore_Timer *timer_shoot;
  Explosion *e;
  int power;
  Eina_Bool can_shoot : 1;
};


static Eina_Bool
_alien_move_cb(void *data)
{
  Alien *a = (Alien *)data;
  Evas_Coord x;
  Evas_Coord y;
  Evas_Coord gh;

  game_size_get(a->g, NULL, &gh);
  evas_object_geometry_get(a->o, &x, &y, NULL, NULL);
  evas_object_move(a->o, x, y + 1);
  if (y > gh)
    {
      game_alien_remove(a->g, a);
      alien_free(a);
      return EINA_FALSE;
    }

  return EINA_TRUE;
}

static Eina_Bool
_alien_shoot_cb(void *data)
{
  Alien *a;
  Shoot *shoot;
  Evas_Coord x;
  Evas_Coord y;
  Evas_Coord w;
  Evas_Coord h;
  Evas_Coord sw;
  Evas_Coord sh;

  a = (Alien *)data;

  evas_object_geometry_get(a->o, &x, &y, &w, &h);
  shoot = shoot_new(a->g, SHOOT_DIR_DOWN_ALIEN, &sw, &sh);
  if (!shoot) return EINA_FALSE;

  shoot_start(shoot, x + (w - sw) / 2, y + h, 0.01);
  return EINA_TRUE;
}


Alien *
alien_new(Game *g, Alien_Type type, Powerup_Type ptype, Evas_Coord x)
{
  Alien *a;

  a = (Alien *)calloc(1, sizeof(Alien));
  if (!a) return NULL;

  printf("alien: %p\n", a);

  if (type == ALIEN_TYPE_BLUE)
    {
      a->o = edje_object_add(game_evas_get(g));
      edje_object_file_set(a->o, "theme.edj", "alien1");
      evas_object_resize(a->o, 49, 49);
      evas_object_move(a->o, x, -49);
    }
  else if (type == ALIEN_TYPE_RED)
    {
      a->o = edje_object_add(game_evas_get(g));
      edje_object_file_set(a->o, "theme.edj", "alien2");
      evas_object_resize(a->o, 50, 50);
      evas_object_move(a->o, x, -50);
    }
  else if (type == ALIEN_TYPE_GREEN)
    {
      a->o = edje_object_add(game_evas_get(g));
      edje_object_file_set(a->o, "theme.edj", "alien3");
      evas_object_resize(a->o, 89, 77);
      evas_object_move(a->o, x, -77);
    }
  else
    {
      evas_object_del(a->o);
      free(a);
      return NULL;
    }
  evas_object_layer_set(a->o, 10);
  evas_object_show(a->o);

  a->power = atoi(edje_object_data_get(a->o, "power"));
  a->can_shoot = atoi(edje_object_data_get(a->o, "can_shoot"));

  a->e = explosion_new(game_evas_get(g),
                       edje_object_data_get(a->o, "explosion"),
                       a->o);

  a->g = g;
  a->type = type;
  a->ptype = ptype;

  return a;
}

void
alien_free(Alien *a)
{
  if (!a) return;

  if (a->timer_shoot) ecore_timer_del(a->timer_shoot);
  if (a->timer_move) ecore_timer_del(a->timer_move);
  evas_object_del(a->o);
  explosion_free(a->e);
  free(a);
}

void
alien_attack(Alien *a)
{
  if (a->timer_move) return;

  a->timer_move = ecore_timer_add(0.03, _alien_move_cb, a);
  if (a->can_shoot) a->timer_shoot = ecore_timer_add(0.9, _alien_shoot_cb, a);
}

int
alien_explode(Alien *a, Evas_Coord hot_x, Evas_Coord hot_y)
{
  if (!a) return EINA_FALSE;

  if (explosion_launch(a->e, hot_x, hot_y))
    {
      a->power--;
      if (a->power == 0)
        {
          Powerup *pu;
          Evas_Coord x;
          Evas_Coord y;
          Evas_Coord w;
          Evas_Coord h;

          evas_object_geometry_get(a->o, &x, &y, &w, &h);
          pu = powerup_new(a->g, a->ptype, x + w / 2, y + h / 2);
          game_alien_remove(a->g, a);
          alien_free(a);
          return 0;
        }
      return a->power;
    }

  return -1;
}
