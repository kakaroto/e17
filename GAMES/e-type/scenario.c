#include "config.h"

#include <Eina.h>
#include <Evas.h>

#include "game.h"
#include "powerup.h"
#include "alien.h"
#include "scenario.h"

typedef struct _Action
{
  int count;
  Alien_Type alien_type;
  Powerup_Type powerup_type;
  Evas_Coord x;
} Action;

struct _Scenario
{
  Game *g;
  Eina_List *actions;
};

static Action *
action_new(int count, Alien_Type alien_type, Powerup_Type powerup_type, Evas_Coord x)
{
  Action *a;

  a = (Action *)calloc(1, sizeof(Action));
  if (!a) return NULL;

  a->count = count;
  a->alien_type = alien_type;
  a->powerup_type = powerup_type;
  a->x = x;

  return a;
}

static void
action_free(Action *a)
{
  if (!a) return;

  free(a);
}

Scenario *
scenario_new(Game *g)
{
  Scenario *s;

  s = (Scenario *)calloc(1, sizeof(Scenario));
  if (!s) return NULL;

  s->g = g;

  return s;
}

void
scenario_free(Scenario *s)
{
  char      *data;

  if (!s) return;

  EINA_LIST_FREE(s->actions, data)
    if (data) action_free((Action *)data);

  free(s);
}

void
scenario_add(Scenario *s, int count, Alien_Type alien_type, Powerup_Type powerup_type, Evas_Coord x)
{
  Action *a = action_new(count, alien_type, powerup_type, x);
  if (!a) return;

  s->actions = eina_list_append(s->actions, a);
}

void
scenario_del(Scenario *s, Action *a)
{
  if (!s || !a) return;

  s->actions = eina_list_remove(s->actions, a);
  action_free(a);
}

void
scenario_launch(Scenario *s, int count)
{
  Eina_List *l = NULL;
  void *data;

  EINA_LIST_FOREACH(s->actions, l, data)
    {
      Action *action = (Action *)data;
      if (action->count == count)
        {
          Alien *alien;

          printf("found action at %d\n", count);
          alien = alien_new(s->g, action->alien_type, action->powerup_type, action->x);
          if (!alien)
            printf(" ** %d %d", action->alien_type, action->x);
          game_alien_append(s->g, alien);
          alien_attack(alien);
          break;
        }
    }
}
