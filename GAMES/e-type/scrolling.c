#include "config.h"

#include <stdlib.h>

#include <Evas.h>
#include <Ecore.h>
#include <Edje.h>

#include "game.h"
#include "scenario.h"
#include "scrolling.h"


typedef struct
{
  Evas_Object **objs;
  int *heights;
  int n_min;
  int n_max;
  Scrolling_Pos pos;
} Elements;

struct _Scrolling
{
  Game *g;
  Elements *elts[3];
  Evas_Object *nebula;
  int count;
};


static Eina_Bool
_nebula_move_cb(void *data)
{
  Scrolling *s;
  Evas_Coord x;
  Evas_Coord y;
  Evas_Coord h;
  Evas_Coord gw;
  Evas_Coord gh;

  s = (Scrolling *)data;

  game_size_get(s->g, &gw, &gh);
  evas_object_geometry_get(s->nebula, &x, &y, NULL, &h);
  y++;
  if (y >= gh) y = -h;
  evas_object_move(s->nebula, x, y);

  return EINA_TRUE;
}

static void
_elements_move(Elements *elts, Evas_Coord gw, Evas_Coord gh, int count)
{
  Evas_Coord x;
  Evas_Coord y;
  int i;

  for (i = elts->n_min; i <= elts->n_max; i++)
    {
      evas_object_geometry_get(elts->objs[i], &x, &y, NULL, NULL);
      y++;
      evas_object_move(elts->objs[i], x, y);

      if (i == elts->n_min)
        {
          if (y >= gh)
            {
              evas_object_del(elts->objs[i]);
              elts->n_min++;
            }
        }
    }

  if (elts->heights[elts->n_min + 1] == count)
    {
      evas_object_del(elts->objs[elts->n_min]);
      elts->n_min++;
    }

  if ((gh -1 - elts->heights[elts->n_max + 1] + count) == -1)
    {
      Evas_Coord w;

      elts->n_max++;
      evas_object_geometry_get(elts->objs[elts->n_max], NULL, NULL, &w, NULL);
      switch (elts->pos)
        {
        case SCROLLING_LEFT:
          evas_object_move(elts->objs[elts->n_max], 0, gh - elts->heights[elts->n_max + 1] - 1 + count);
          break;
        case SCROLLING_RIGHT:
          evas_object_move(elts->objs[elts->n_max], gw - w, gh - elts->heights[elts->n_max + 1] - 1 + count);
          break;
        case SCROLLING_CENTER:
          evas_object_move(elts->objs[elts->n_max], 0, gh - elts->heights[elts->n_max + 1] - 1 + count);
          break;
        default:
          evas_object_move(elts->objs[elts->n_max], 0, gh - elts->heights[elts->n_max + 1] - 1 + count);
          break;
        }
      evas_object_show(elts->objs[elts->n_max]);
    }
}

static Eina_Bool
_scrolling_move_cb(void *data)
{
  Scrolling *s;
  Evas_Coord gw;
  Evas_Coord gh;

  s = (Scrolling *)data;
  s->count++;

  game_size_get(s->g, &gw, &gh);

  _elements_move(s->elts[0], gw, gh, s->count);
  _elements_move(s->elts[1], gw, gh, s->count);

  scenario_launch(game_scenario_get(s->g), s->count);

  return EINA_TRUE;
}

static Elements *
elements_new(Evas *evas, Evas_Coord gw, Evas_Coord gh, int nbr, Scrolling_Pos pos)
{
  Elements *elts;
  int i;

  if (nbr <= 0) return NULL;

  elts = (Elements *)calloc(1, sizeof(Elements));
  if (!elts) return NULL;

  elts->objs = (Evas_Object **)calloc(nbr, sizeof(Evas_Object *));
  if (!elts->objs)
    goto free_elts;

  elts->heights = (int *)calloc(nbr, sizeof(int));
  if (!elts->heights)
    goto free_objs;

  elts->pos = pos;

  srand(time(NULL) + pos);
  i = 0;
  while (i < nbr)
    {
      int r;
      Evas_Coord h;

      elts->objs[i] = evas_object_rectangle_add(evas);
      evas_object_layer_set(elts->objs[i], 5);

      r = rand();
      if (r < (RAND_MAX / 3))
        {
          evas_object_resize(elts->objs[i], 20, 40);
          evas_object_color_set(elts->objs[i], 255, 0, 0, 255);
        }
      else if (r < 2 * (RAND_MAX / 3))
        {
          evas_object_resize(elts->objs[i], 30, 60);
          evas_object_color_set(elts->objs[i], 0, 255, 0, 255);
        }
      else
        {
          evas_object_resize(elts->objs[i], 15, 80);
          evas_object_color_set(elts->objs[i], 0, 0, 255, 255);
        }

      if (i != 0)
        {
          evas_object_geometry_get(elts->objs[i - 1], NULL, NULL, NULL, &h);
          elts->heights[i] = elts->heights[i - 1] + h;
        }

      i++;
    }

  i = 0;
  elts->n_min = 0;
  while (1)
    {
      if (elts->heights[i] < gh)
        {
          Evas_Coord w;

          evas_object_geometry_get(elts->objs[i], NULL, NULL, &w, NULL);
          switch (elts->pos)
            {
            case SCROLLING_LEFT:
              evas_object_move(elts->objs[i], 0, gh - elts->heights[i + 1] - 1);
              break;
            case SCROLLING_RIGHT:
              evas_object_move(elts->objs[i], gw - w, gh - elts->heights[i + 1] - 1);
              break;
            case SCROLLING_CENTER:
              evas_object_move(elts->objs[i], 0, gh - elts->heights[i + 1] - 1);
              break;
            default:
              evas_object_move(elts->objs[i], 0, gh - elts->heights[i + 1] - 1);
              break;
            }
          evas_object_show(elts->objs[i]);
          i++;
        }
      else
        {
          elts->n_max = i - 1;
          break;
        }
    }

  return elts;

 free_objs:
  free(elts->objs);
 free_elts:
  free(elts);

  return NULL;
}

static void
elements_free(Elements *elts)
{
  if (!elts) return;

  free(elts->heights);
  free(elts->objs);
  free(elts);
}

Scrolling *
scrolling_new(Game *g, int nbr)
{
  Scrolling *s;
  Evas_Coord gw;
  Evas_Coord gh;

  if (nbr <= 0) return NULL;

  s = (Scrolling *)calloc(1, sizeof(Scrolling));
  if (!s) return NULL;

  game_size_get(g, &gw, &gh);

  s->elts[0] = elements_new(game_evas_get(g), gw, gh, nbr, SCROLLING_LEFT);
  if (!s->elts[0])
    goto free_s;

  s->elts[1] = elements_new(game_evas_get(g), gw, gh, nbr, SCROLLING_RIGHT);
  if (!s->elts[1])
    goto free_elts_0;

  s->g = g;

  s->nebula = edje_object_add(game_evas_get(s->g));
  if (edje_object_file_set(s->nebula, "theme.edj", "nebula"))
    {
      double r;
      Evas_Coord w;
      Evas_Coord h;

      srand(time(NULL));
      edje_object_size_min_get(s->nebula, &w, &h);
      r = (double)(gw - 40 - w) * (double)rand() / (double)RAND_MAX;
      evas_object_move (s->nebula, 20 + (Evas_Coord)r, -h);
      evas_object_resize(s->nebula, w, h);
      evas_object_layer_set(s->nebula, -8);
      evas_object_show (s->nebula);
      ecore_timer_add(0.06, _nebula_move_cb, s);
    }

  return s;

 free_elts_0:
  elements_free(s->elts[0]);
 free_s:
  free(s);

  return NULL;
}

void
scrolling_free(Scrolling *s)
{
  if (!s) return;

  if (s->elts[0]) elements_free(s->elts[0]);
  if (s->elts[1]) elements_free(s->elts[1]);
  if (s->elts[2]) elements_free(s->elts[2]);
  free(s);
}

void
scrolling_start(Scrolling *s)
{
  if (!s) return;

  ecore_timer_add(0.01, _scrolling_move_cb, s);
}
