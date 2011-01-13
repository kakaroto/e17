#include "config.h"

#include <stdlib.h>

#include <Evas.h>
#include <Edje.h>

#include "game.h"
#include "powerup.h"
#include "alien.h"
#include "scenario.h"
#include "scrolling.h"
#include "ship.h"
#include "shoot.h"
#include "stars.h"


#define MAX_STARS 20
#define MAX_OBJ 100


struct _Game
{
  Evas *evas;
  Evas_Coord w;
  Evas_Coord h;

  Star *stars;
  Ship *ship;
  Scrolling *scrolling;
  Scenario *scenario;
  Eina_List *aliens;
  Eina_List *shoots;
  Eina_List *powerups;
};


Game *
game_new(Evas *evas)
{
  Game *g;
  Evas_Object *o;

  g = (Game *)calloc(1, sizeof(Game));
  if (!g) return NULL;

  g->evas = evas;

  /* black background */

  o = edje_object_add(evas);
  if (!edje_object_file_set(o, "theme.edj", "background"))
    goto game_free;

  g->w = atoi(edje_object_data_get(o, "bg_w"));
  g->h = atoi(edje_object_data_get(o, "bg_h"));
  evas_object_resize(o, g->w, g->h);
  evas_object_move(o, 0, 0);
  evas_object_layer_set(o, -10);
  evas_object_show(o);

  g->stars = stars_new(g, MAX_STARS);
  if (!g->stars)
    goto game_free;

  g->scrolling = scrolling_new(g, MAX_OBJ);
  if (!g->scrolling)
    goto stars_free;

  g->ship = ship_new(g);
  if (!g->ship)
    goto scrolling_free;
  ship_upgrade_set(g->ship, SHIP_UPGRADE_FRONT_SINGLE);
/*   ship_upgrade_set(g->ship, SHIP_UPGRADE_REAR_CIRCLE); */
/*   ship_upgrade_set(g->ship, SHIP_UPGRADE_REAR_SINGLE); */

  g->scenario = scenario_new(g);
  if (!g->scenario)
    goto ship_free;

  scenario_add(g->scenario, 50, ALIEN_TYPE_BLUE, POWERUP_TYPE_1, 200);
  scenario_add(g->scenario, 250, ALIEN_TYPE_RED, POWERUP_TYPE_1, 400);
  scenario_add(g->scenario, 450, ALIEN_TYPE_GREEN, POWERUP_TYPE_NONE, 300);

  scrolling_start(g->scrolling);

  return g;

 ship_free:
  ship_free(g->ship);
 scrolling_free:
  scrolling_free(g->scrolling);
 stars_free:
  stars_free(g->stars, MAX_STARS);
 game_free:
  free(g);

  return NULL;
}

void
game_free(Game *g)
{
  void *data;

  if (!g) return;

  scenario_free(g->scenario);
  EINA_LIST_FREE(g->powerups, data)
    powerup_free((Powerup *)data);
  EINA_LIST_FREE(g->shoots, data)
    shoot_free((Shoot *)data);
  EINA_LIST_FREE(g->aliens, data)
    alien_free((Alien *)data);
  ship_free(g->ship);
  scrolling_free(g->scrolling);
  stars_free(g->stars, MAX_STARS);
  free(g);
}

Evas *
game_evas_get(Game *g)
{
  return g->evas;
}

void
game_size_get(Game *g, Evas_Coord *w, Evas_Coord *h)
{
  if (w) *w = g->w;
  if (h) *h = g->h;
}

void
game_alien_append(Game *g, Alien *a)
{
  if (!a) return;

  g->aliens = eina_list_append(g->aliens, a);
}

void
game_alien_remove(Game *g, Alien *a)
{
  if (!a) return;

  g->aliens = eina_list_remove(g->aliens, a);
}

void
game_shoot_append(Game *g, Shoot *s)
{
  if (!s) return;

  g->shoots = eina_list_append(g->shoots, s);
}

void
game_shoot_remove(Game *g, Shoot *s)
{
  if (!s) return;

  g->shoots = eina_list_remove(g->shoots, s);
}

void
game_powerup_append(Game *g, Powerup *p)
{
  if (!p) return;

  g->powerups = eina_list_append(g->powerups, p);
}

void
game_powerup_remove(Game *g, Powerup *p)
{
  if (!p) return;

  g->powerups = eina_list_remove(g->powerups, p);
}

Eina_List *
game_aliens_get(Game *g)
{
  if (!g) return NULL;

  return g->aliens;
}

Eina_List *
game_shoots_get(Game *g)
{
  if (!g) return NULL;

  return g->shoots;
}

Scenario *
game_scenario_get(Game *g)
{
  if (!g) return NULL;

  return g->scenario;
}

Ship *
game_ship_get(Game *g)
{
  if (!g) return NULL;

  return g->ship;
}

Eina_List *
game_powerups_get(Game *g)
{
  if (!g) return NULL;

  return g->powerups;
}
